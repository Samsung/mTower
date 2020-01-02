/**
 * @file        tools/fwinfogen.c
 * @brief       .
 *
 * @copyright   Copyright (c) 2019 Samsung Electronics Co., Ltd. All Rights Reserved.
 * @author      Taras Drozdovskyi t.drozdovsky@samsung.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Included Files. */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/sha.h>
#include <openssl/ec.h>      // for EC_GROUP_new_by_curve_name, EC_GROUP_free, EC_KEY_new, EC_KEY_set_group, EC_KEY_generate_key, EC_KEY_free
#include <openssl/ecdsa.h>   // for ECDSA_do_sign, ECDSA_do_verify
#include <openssl/obj_mac.h> // for NID_secp192k1

#include "config.h"
//#include "version.h"

/* Pre-processor Definitions. */
#define BUILD_MONTH_IS_JAN (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_FEB (__DATE__[0] == 'F')
#define BUILD_MONTH_IS_MAR (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')
#define BUILD_MONTH_IS_APR (__DATE__[0] == 'A' && __DATE__[1] == 'p')
#define BUILD_MONTH_IS_MAY (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')
#define BUILD_MONTH_IS_JUN (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_JUL (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')
#define BUILD_MONTH_IS_AUG (__DATE__[0] == 'A' && __DATE__[1] == 'u')
#define BUILD_MONTH_IS_SEP (__DATE__[0] == 'S')
#define BUILD_MONTH_IS_OCT (__DATE__[0] == 'O')
#define BUILD_MONTH_IS_NOV (__DATE__[0] == 'N')
#define BUILD_MONTH_IS_DEC (__DATE__[0] == 'D')

#define BUILD_MONTH \
    ( \
        (BUILD_MONTH_IS_JAN) ? 1 : \
        (BUILD_MONTH_IS_FEB) ? 2 : \
        (BUILD_MONTH_IS_MAR) ? 3 : \
        (BUILD_MONTH_IS_APR) ? 4 : \
        (BUILD_MONTH_IS_MAY) ? 5 : \
        (BUILD_MONTH_IS_JUN) ? 6 : \
        (BUILD_MONTH_IS_JUL) ? 7 : \
        (BUILD_MONTH_IS_AUG) ? 8 : \
        (BUILD_MONTH_IS_SEP) ? 9 : \
        (BUILD_MONTH_IS_OCT) ? 16 : \
        (BUILD_MONTH_IS_NOV) ? 17 : \
        (BUILD_MONTH_IS_DEC) ? 18 : \
        /* error default */    0 \
    )

#define BUILD_DAY_CH0 ((__DATE__[4] >= '0') ? (__DATE__[4]) : '0')
#define BUILD_DAY_CH1 (__DATE__[ 5])

#define DATE_COMPILE  (((__DATE__[7] - '0') << 28) \
    | (((unsigned int)(__DATE__[8] - '0')) << 24) \
    | (((unsigned int)(__DATE__[9] - '0')) << 20) \
    | (((unsigned int)(__DATE__[10] - '0')) << 16) \
    | (((unsigned int)(BUILD_MONTH)) << 8) \
    | (((unsigned int)(((__DATE__[4] >= '0') ? (__DATE__[4]) : '0') - '0')) << 4) \
    | (((unsigned int)__DATE__[5] - '0')))

#define BL33_METADATA_ADDRESS (0x10080000 - CONFIG_START_ADDRESS_BL33 - 0x8000)


/** Instruction Code of "B ."  */

/* Private Types. */
/* Any types, enums, structures or unions used by the file are defined here. */
/* typedef for NonSecure callback functions */

/**
 * @details    ECC public key structure
 */
typedef struct {
  uint32_t au32Key0[8]; /* 256-bits */
  uint32_t au32Key1[8]; /* 256-bits */
}__attribute__((packed)) ECC_PUBKEY_T;

/**
 * @details    ECC ECDSA signature structure
 */
typedef struct {
  uint32_t au32R[8]; /* 256-bits */
  uint32_t au32S[8]; /* 256-bits */
}__attribute__((packed)) ECDSA_SIGN_T;

typedef struct {
  uint32_t u32Start; /* 32-bits */
  uint32_t u32Size; /* 32-bits */
}__attribute__((packed)) FW_REGION_T;

typedef struct {
  uint32_t u32AuthCFGs; /* 32-bits */
  /*
   bit[1:0]:   Reserved
   bit[2]:     1: Info Hash includes PDID / 0: Not include PDID
   bit[3]:     1: Info Hash includes UID / 0: Not include UID
   bit[4]:     1: Info Hash inculdes UICD / 0: Not include UICD
   bit[31:5]:  Reserved
   */
  uint32_t u32FwRegionLen; /* 32-bits */
  FW_REGION_T au32FwRegion[1]; /* (8*1) bytes */
  uint32_t u32ExtInfoLen; /* 32-bits */
  uint32_t au32ExtInfo[3]; /* 12-bytes */
}__attribute__((packed)) METADATA_T;

typedef struct {
  ECC_PUBKEY_T pubkey; /* 64-bytes (256-bits + 256-bits) */

  METADATA_T mData; /* includes authenticate configuration, F/W regions and extend info */

  uint32_t au32FwHash[8]; /* 32-bytes (256-bits) */

  ECDSA_SIGN_T sign; /* 64-bytes (256-bits R + 256-bits S) */
}__attribute__((packed)) FW_INFO_T;

/**
 * @details    ECC ECDSA key structure
 */
typedef struct {
  uint8_t Qx[32]; /* 256-bits */
  uint8_t Qy[32]; /* 256-bits */
  uint8_t d[32]; /* 256-bits */
}__attribute__((packed)) ECC_KEY_T;

/* Private Function Prototypes. */
/* Prototypes of all static functions in the file are provided here. */

/* Private Data. */
/* All static data definitions appear here. */

/* Public Data. */
/* All data definitions with global scope appear here. */
const char header[] = "\n"
    "const uint32_t g_InitialFWinfo[] =\n"
    "{\n"
    "  /* public key - 64-bytes (256-bits + 256-bits) */\n";

/* Public Function Prototypes */

/* Private Functions. */

void sha256(unsigned char *data, unsigned int data_len, unsigned char *hash)
{
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, data, data_len);
  SHA256_Final(hash, &sha256);
}

static int sign_pFwInfo(FW_INFO_T *pFwInfo, ECC_KEY_T *ecdsa_key)
{
  int ret = -1;
  EC_KEY *eckey = EC_KEY_new();
  if (NULL == eckey) {
    printf("Failed to create new EC Key\n");
    return -1;
  }

  EC_GROUP *ecgroup = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);
  if (NULL == ecgroup) {
    printf("Failed to create new EC Group\n");
    goto exit;
  }

  if (EC_KEY_set_group(eckey, ecgroup) != 1) {
    printf("Failed to set group for EC Key\n");
    goto exit;
  }

  memcpy((void *) pFwInfo->pubkey.au32Key0, (void *) ecdsa_key->Qx, 32);
  memcpy((void *) pFwInfo->pubkey.au32Key1, (void *) ecdsa_key->Qy, 32);

  BIGNUM* x = BN_bin2bn((void *) ecdsa_key->Qx, 32, NULL);
  BIGNUM* y = BN_bin2bn((void *) ecdsa_key->Qy, 32, NULL);
  BIGNUM* d = BN_bin2bn((void *) ecdsa_key->d, 32, NULL);

  EC_KEY_set_private_key(eckey, d);
  EC_KEY_set_public_key_affine_coordinates(eckey, x, y);

  uint32_t au32HeadHash[8];
  unsigned int u32Size = sizeof(FW_INFO_T) - sizeof(ECDSA_SIGN_T);
  sha256((unsigned char *) pFwInfo, u32Size, (unsigned char *) au32HeadHash);

  ECDSA_SIG *signature = ECDSA_do_sign((unsigned char *) au32HeadHash, u32Size,
      eckey);
  if (NULL == signature) {
    printf("Failed to generate EC Signature\n");
  } else {
    if (ECDSA_do_verify((unsigned char *) au32HeadHash, u32Size, signature,
        eckey) != 1) {
      printf("Failed to verify EC Signature\n");
    } else {
      BIGNUM* r = signature->r;
      BIGNUM* s = signature->s;

//      printf("d: %s\n", BN_bn2hex(d));
//      printf("X: %s\n", BN_bn2hex(x));
//      printf("Y: %s\n", BN_bn2hex(y));
//      printf("R: %s\n", BN_bn2hex(r));
//      printf("S: %s\n", BN_bn2hex(s));

      BN_bn2bin(r, (unsigned char *) pFwInfo->sign.au32R);
      BN_bn2bin(s, (unsigned char *) pFwInfo->sign.au32S);
      BN_free(x);
      BN_free(y);
      BN_free(d);
      BN_free(r);
      BN_free(s);
    }
  }
  ret = 0;
  exit: EC_GROUP_free(ecgroup);
  EC_KEY_free(eckey);

  return ret;
}

int getFileSize(const char *filename)
{
  int  size = -1;

  FILE* fd = fopen(filename, "rb");
  if (!fd) {
    printf("Failed to open file: %s\n",filename);
    return -1;
  }

  /* Get file size */
  if (fseek(fd, 0, SEEK_END) != 0) {
    printf("Unable to get '%s' file size\n", filename);
    goto exit;
  }

  size = ftell(fd);
  if (size < 0) {
    printf("Stream doesn't support '%s' file positioning\n", filename);
    goto exit;
  }
//  rewind(fd);
exit:
  fclose(fd);
  return size;
}
#ifdef CONFIG_BOOTLOADER2
/**
 * @brief         printFwInfo - .
 *
 * @param pFwInfo [in/out] A pointer to jar file name string.
 *
 * @returns       0 on success or error code on failure.
 */
void printFwInfo(FW_INFO_T *pFwInfo)
{
  printf("%s", header);
  printf("  0x%08x, 0x%08x, 0x%08x, 0x%08x,\n", pFwInfo->pubkey.au32Key0[0],
      pFwInfo->pubkey.au32Key0[1], pFwInfo->pubkey.au32Key0[2],
      pFwInfo->pubkey.au32Key0[3]);
  printf("  0x%08x, 0x%08x, 0x%08x, 0x%08x,\n", pFwInfo->pubkey.au32Key0[4],
      pFwInfo->pubkey.au32Key0[5], pFwInfo->pubkey.au32Key0[6],
      pFwInfo->pubkey.au32Key0[7]);
  printf("  0x%08x, 0x%08x, 0x%08x, 0x%08x,\n", pFwInfo->pubkey.au32Key1[0],
      pFwInfo->pubkey.au32Key1[1], pFwInfo->pubkey.au32Key1[2],
      pFwInfo->pubkey.au32Key1[3]);
  printf("  0x%08x, 0x%08x, 0x%08x, 0x%08x,\n", pFwInfo->pubkey.au32Key1[4],
      pFwInfo->pubkey.au32Key1[5], pFwInfo->pubkey.au32Key1[6],
      pFwInfo->pubkey.au32Key1[7]);
  printf("\n");
  printf(
      "  /* metadata data - includes Mode selection, F/W region and Extend info */\n");
  printf("  0x%08x, 0x%08x, 0x%08x, 0x%08x, // 0x00020000: NuBL32 F/W base\n",
      pFwInfo->mData.u32AuthCFGs, pFwInfo->mData.u32FwRegionLen,
      pFwInfo->mData.au32FwRegion[0].u32Start,
      pFwInfo->mData.au32FwRegion[0].u32Size);
  printf(
      "  0x%08x, 0x%08x, 0x%08x, 0x%08x, // 0x20180824/0x00001111/0x22223333: Extend info\n",
      pFwInfo->mData.u32ExtInfoLen, pFwInfo->mData.au32ExtInfo[0],
      pFwInfo->mData.au32ExtInfo[1], pFwInfo->mData.au32ExtInfo[2]);
  printf("\n");
  printf("  /* FW hash - 32-bytes (256-bits) */\n");
  printf("  0x%08x, 0x%08x, 0x%08x, 0x%08x,\n", pFwInfo->au32FwHash[0],
      pFwInfo->au32FwHash[1], pFwInfo->au32FwHash[2], pFwInfo->au32FwHash[3]);
  printf("  0x%08x, 0x%08x, 0x%08x, 0x%08x,\n", pFwInfo->au32FwHash[4],
      pFwInfo->au32FwHash[5], pFwInfo->au32FwHash[6], pFwInfo->au32FwHash[7]);
  printf("\n");
  printf("  /* FwInfo signature - 64-bytes (256-bits R + 256-bits S) */\n");
  printf("  0x%08x, 0x%08x, 0x%08x, 0x%08x,\n", pFwInfo->sign.au32R[0],
      pFwInfo->sign.au32R[1], pFwInfo->sign.au32R[2], pFwInfo->sign.au32R[3]);
  printf("  0x%08x, 0x%08x, 0x%08x, 0x%08x,\n", pFwInfo->sign.au32R[4],
      pFwInfo->sign.au32R[5], pFwInfo->sign.au32R[6], pFwInfo->sign.au32R[7]);
  printf("  0x%08x, 0x%08x, 0x%08x, 0x%08x,\n", pFwInfo->sign.au32S[0],
      pFwInfo->sign.au32S[1], pFwInfo->sign.au32S[2], pFwInfo->sign.au32S[3]);
  printf("  0x%08x, 0x%08x, 0x%08x, 0x%08x,\n", pFwInfo->sign.au32S[4],
      pFwInfo->sign.au32S[5], pFwInfo->sign.au32S[6], pFwInfo->sign.au32S[7]);
  printf("};\n");
}
#endif
/**
 * @brief         main - entry point of mTower: secure world.
 *
 * @param         None
 *
 * @returns       None (function is not supposed to return)
 */
int main(int argc, char** argv)
{
  unsigned char *buf = NULL;
  int ret = -1;
  FILE* fd;
  int img_size = 0;

#ifdef CONFIG_BOOTLOADER2

  ECC_KEY_T ecdsa_key;
  FW_INFO_T pFwInfo;

  pFwInfo.mData.u32AuthCFGs = 0x00000001;
  pFwInfo.mData.u32FwRegionLen = 0x00000008;
  pFwInfo.mData.au32FwRegion[0].u32Start = CONFIG_START_ADDRESS_BL32;
  pFwInfo.mData.au32FwRegion[0].u32Size = 0x00000000;
  pFwInfo.mData.u32ExtInfoLen = 0x0000000c;
  pFwInfo.mData.au32ExtInfo[0] = DATE_COMPILE;
  pFwInfo.mData.au32ExtInfo[1] = 0x00001111;
  pFwInfo.mData.au32ExtInfo[2] = 0x22223333;
#endif

  if (argc != 7) {
    printf("Not enough input parameters for %s\n", argv[0]);
    return -1;
  }

  buf = malloc(1024 * 256);
  if (buf == NULL) {
    printf("Allocation memory error\n");
    goto exit;
  }
  memset((void *) buf, 0, 1024 * 256);

  ////////////////////////////////////////////////////////////////
  // mtower_s.bin
  ////////////////////////////////////////////////////////////////

#ifdef CONFIG_BOOTLOADER2
  int32_t size_bl2;
  if((size_bl2 = getFileSize(argv[1])) < 0) {
    return -1;
  }

  fd = fopen(argv[1], "rb");
  if (!fd) {
    printf("Failed to open file: %s\n",argv[1]);
    return -1;
  }

  if (fread(buf, 1, (size_t) size_bl2, fd) != (size_t) size_bl2) {
    free(buf);
    goto exit;
  }
  fclose(fd);

#endif

#ifdef CONFIG_BOOTLOADER32
  int32_t size_bl32;
  if((size_bl32 = getFileSize(argv[2])) < 0) {
    return -1;
  }

  fd = fopen(argv[2], "rb");
  if (!fd) {
    printf("Failed to open file: %s\n",argv[2]);
    goto exit;
  }
  if (fread(buf + CONFIG_START_ADDRESS_BL32, 1, (size_t) size_bl32, fd) != (size_t) size_bl32)
  {
    free(buf);
    goto exit;
  }
  fclose(fd);
  img_size = size_bl32;
#endif

#ifdef CONFIG_BOOTLOADER2
  sha256(buf + CONFIG_START_ADDRESS_BL32, size_bl32, (unsigned char *) &pFwInfo.au32FwHash);
  pFwInfo.mData.au32FwRegion[0].u32Size = size_bl32;

  fd = fopen(argv[4], "rb");
  if (!fd) {
    printf("Failed to open file: %s\n",argv[4]);
    goto exit;
  }

  if (fread((unsigned char *) &ecdsa_key, sizeof(char), sizeof(ECC_KEY_T), fd)
      != sizeof(ECC_KEY_T))
  {
    return -1;
  }
  fclose(fd);

  sign_pFwInfo(&pFwInfo, &ecdsa_key);

  memcpy(buf + 0x00038000, (unsigned char *) &pFwInfo, sizeof(FW_INFO_T));
  img_size = 0x00038000 + sizeof(FW_INFO_T);
#endif

  fd = fopen(argv[3], "wb");
  if (!fd) {
    printf("Failed to open file: %s\n",argv[3]);
    goto exit;
  }

  if (fwrite(buf, sizeof(char), (size_t) img_size, fd) != (size_t) img_size) {
    free(buf);
    goto exit;
  }

#ifdef CONFIG_BOOTLOADER2
  printf("\n\tSecure firmware info\n");
  printFwInfo(&pFwInfo);
#endif

////////////////////////////////////////////////////////////////
// mtower_ns.bin
////////////////////////////////////////////////////////////////

#ifdef CONFIG_BOOTLOADER33
  memset((void *) buf, 0, 1024 * 256);

  int32_t size_bl33;
  if((size_bl33 = getFileSize(argv[5])) < 0) {
    return -1;
  }

  fd = fopen(argv[5], "rb");
  if (!fd) {
    printf("Failed to open file: %s\n",argv[5]);
    goto exit;
  }
  if (fread(buf, 1, (size_t) size_bl33, fd) != (size_t) size_bl33)
  {
    goto exit;
  }
  fclose(fd);

  img_size = size_bl33;

#ifdef CONFIG_BOOTLOADER2
  sha256(buf, size_bl33, (unsigned char *) &pFwInfo.au32FwHash);
  pFwInfo.mData.au32FwRegion[0].u32Start = CONFIG_START_ADDRESS_BL33;
  pFwInfo.mData.au32FwRegion[0].u32Size = size_bl33;

  sign_pFwInfo(&pFwInfo, &ecdsa_key);
  memcpy(buf + BL33_METADATA_ADDRESS, (unsigned char *) &pFwInfo, sizeof(FW_INFO_T));
  img_size = BL33_METADATA_ADDRESS + sizeof(FW_INFO_T);
#endif

  fd = fopen(argv[6], "wb");
  if (!fd) {
    printf("Failed to open file: %s\n",argv[6]);
    goto exit;
  }

  if (fwrite(buf, sizeof(char), (size_t) img_size, fd) != (size_t) img_size) {
    goto exit;
  }
#endif
  ret = 0;
exit:
  free(buf);
  fclose(fd);

#ifdef CONFIG_BOOTLOADER2
  printf("\n\tNon-secure firmware info\n");
  printFwInfo(&pFwInfo);
#endif

  return ret;
}

