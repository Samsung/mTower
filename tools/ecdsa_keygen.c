/**
 * @file        arch/arm/m2351/src/numaker_pfm_m2351/secure/main.c
 * @brief       Provides functionality to start secure world, initialize secure
 *              and normal worlds, pass to execution to normal world.
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
//#include "config.h"
//#include "version.h"

#include <openssl/ec.h>      // for EC_GROUP_new_by_curve_name, EC_GROUP_free, EC_KEY_new, EC_KEY_set_group, EC_KEY_generate_key, EC_KEY_free
#include <openssl/ecdsa.h>   // for ECDSA_do_sign, ECDSA_do_verify
#include <openssl/obj_mac.h> // for NID_secp192k1

#include <stdlib.h>
#include <string.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

/* Pre-processor Definitions. */
#define AES_BLOCK_SIZE 16
#define AES_KEY_SIZE 32

/** Start address for non-secure boot image */

/* Private Types. */
/* Any types, enums, structures or unions used by the file are defined here. */
/* typedef for NonSecure callback functions */

/**
 * @details    ECC ECDSA key structure
 */
typedef struct {
  uint8_t Qx[32]; /* 256-bits */
  uint8_t Qy[32]; /* 256-bits */
  uint8_t d[32]; /* 256-bits */
}__attribute__((packed)) ECC_KEY_T;

typedef struct _AES_DATA {
  unsigned char key[AES_KEY_SIZE];
  unsigned char iv[AES_BLOCK_SIZE];
} AES_DATA;

typedef struct Message_Struct {
  unsigned char *body;
  int *length;
  AES_DATA *aes_settings;
} Message;

/* Private Function Prototypes. */
/* Prototypes of all static functions in the file are provided here. */

/* Private Data. */
/* All static data definitions appear here. */
uint32_t key[] =
  { 0x78563412, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
      0x00000000, 0xefcdac00 };

uint32_t iv[] =
  { 0x78563412, 0x00000000, 0x00000000, 0xefcdac00 };

/* Public Data. */
/* All data definitions with global scope appear here. */

/* Public Function Prototypes */
Message *message_init(int);
int aes256_init(Message *);
Message *aes256_encrypt(Message *);
void aes_cleanup(AES_DATA *);
void message_cleanup(Message *);

void sha256(unsigned char *data, unsigned int data_len, unsigned char *hash)
{
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, data, data_len);
  SHA256_Final(hash, &sha256);
}

Message *message_init(int length)
{
  Message *ret = malloc(sizeof(Message));
  ret->body = malloc(length);
  ret->length = malloc(sizeof(int));
  *ret->length = length;
  //initialize aes_data
  aes256_init(ret);
  return ret;
}

int aes256_init(Message * input)
{
  AES_DATA *aes_info = malloc(sizeof(AES_DATA));
  //point to new data
  input->aes_settings = aes_info;
  //get rand bytes
  memcpy(input->aes_settings->key, key, AES_KEY_SIZE);
  memcpy(input->aes_settings->iv, iv, AES_KEY_SIZE / 2);

  return 0;
}

Message *aes256_encrypt(Message * plaintext)
{
  EVP_CIPHER_CTX *enc_ctx;
  Message * encrypted_message;
  int enc_length = *(plaintext->length)
      + (AES_BLOCK_SIZE - *(plaintext->length) % AES_BLOCK_SIZE);

  encrypted_message = message_init(enc_length);
  //set up encryption context
  enc_ctx = EVP_CIPHER_CTX_new();
  EVP_EncryptInit(enc_ctx, EVP_aes_256_cfb(), plaintext->aes_settings->key,
      plaintext->aes_settings->iv);
  //encrypt all the bytes up to but not including the last block
  if (!EVP_EncryptUpdate(enc_ctx, encrypted_message->body, &enc_length,
      plaintext->body, *plaintext->length))
  {
    EVP_CIPHER_CTX_cleanup(enc_ctx);
    printf("EVP Error: couldn't update encryption with plain text!\n");
    return NULL;
  }
  //update length with the amount of bytes written
  *(encrypted_message->length) = enc_length;
  //EncryptFinal will cipher the last block + Padding
  if (!EVP_EncryptFinal_ex(enc_ctx, enc_length + encrypted_message->body,
      &enc_length))
  {
    EVP_CIPHER_CTX_cleanup(enc_ctx);
    printf("EVP Error: couldn't finalize encryption!\n");
    return NULL;
  }
  //add padding to length
  *(encrypted_message->length) += enc_length;
  //no errors, copy over key & iv rather than pointing to the plaintext msg
  memcpy(encrypted_message->aes_settings->key, plaintext->aes_settings->key,
      AES_KEY_SIZE);
  memcpy(encrypted_message->aes_settings->iv, plaintext->aes_settings->iv,
      AES_KEY_SIZE / 2);
  //Free context and return encrypted message
  EVP_CIPHER_CTX_cleanup(enc_ctx);
  return encrypted_message;
}

void aes_cleanup(AES_DATA *aes_data)
{
//    free(aes_data -> iv);
//    free(aes_data -> key);
//    free(aes_data);
}

void message_cleanup(Message *message)
{
  //free message struct
  aes_cleanup(message->aes_settings);
  free(message->length);
  free(message->body);
  free(message);
}

/**
 * @brief         main - entry point of mTower: secure world.
 *
 * @param         None
 *
 * @returns       None (function is not supposed to return)
 */
int main(int argc, char * argv[])
{
  // Initialize openSSL
//  ERR_load_crypto_strings();
//  OpenSSL_add_all_algorithms();
//  OPENSSL_config(NULL);

  EC_KEY *eckey = EC_KEY_new();
  if (NULL == eckey) {
    printf("Failed to create new EC Key\n");
    return -1;
  }

  EC_GROUP *ecgroup = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);
  if (NULL == ecgroup) {
    printf("Failed to create new EC Group\n");
    return -1;
  }

  int set_group_status = EC_KEY_set_group(eckey, ecgroup);
  const int set_group_success = 1;
  if (set_group_success != set_group_status) {
    printf("Failed to set group for EC Key\n");
    return -1;
  }

  const int gen_success = 1;
  int gen_status = EC_KEY_generate_key(eckey);
  if (gen_success != gen_status) {
    printf("Failed to generate EC Key\n");
    return -1;
  }

  const BIGNUM* d = EC_KEY_get0_private_key(eckey);
  const EC_POINT* Q = EC_KEY_get0_public_key(eckey);
  const EC_GROUP* group = EC_KEY_get0_group(eckey);
  BIGNUM* x = BN_new();
  BIGNUM* y = BN_new();

  if (!EC_POINT_get_affine_coordinates_GFp(group, Q, x, y, NULL)) {
    return -1;
  }

//  printf("d: %s\n", BN_bn2hex(d));
//  printf("X: %s\n", BN_bn2hex(x));
//  printf("Y: %s\n", BN_bn2hex(y));

  ECC_KEY_T ecc_ecdsa_key;

  BN_bn2bin(d, &ecc_ecdsa_key.d[0]);
  BN_bn2bin(x, ecc_ecdsa_key.Qx);
  BN_bn2bin(y, ecc_ecdsa_key.Qy);

//  for (int i = 0; i != 32; i++)
//    printf(" %02X",ecc_ecdsa_key.d[i]);
//  printf("\n");

  char* buff;
  buff = malloc(strlen(argv[1]) + strlen("ecdsa_keys.bin") + 1);
  buff[0] = 0;
  strcat(buff, argv[1]);
  strcat(buff, "ecdsa_keys.bin");

  FILE* fd = fopen(buff, "wb");
  if (!fd) {
    printf("Failed to open file\n");
    free(buff);
    return -1;
  }
  free(buff);

  if (fwrite((void *) &ecc_ecdsa_key, sizeof(char),
      (size_t) (sizeof(ECC_KEY_T)), fd) != (size_t) (sizeof(ECC_KEY_T)))
  {
    fclose(fd);
    return -1;
  }
  fclose(fd);

  Message *message, *enc_msg;

  message = message_init(64);
  memcpy((char *) message->body, (unsigned char *) ecc_ecdsa_key.Qx, 64);

  enc_msg = aes256_encrypt(message);
  //clean up ssl;
  //  EVP_cleanup();
  //  CRYPTO_cleanup_all_ex_data(); //Stop data leaks
  //  ERR_free_strings();

  buff = malloc(strlen(argv[1]) + strlen("NuBL32PubKeyEncrypted.bin") + 1);
  buff[0] = 0;
  strcat(buff, argv[1]);
  strcat(buff, "NuBL32PubKeyEncrypted.bin");

  fd = fopen(buff, "wb");
  if (!fd) {
    printf("Failed to open file\n");
    free(buff);
    return -1;
  }
  free(buff);

  if (fwrite((void *) enc_msg->body, sizeof(char), 64, fd) != 64) {
    fclose(fd);
    return -1;
  }
  fclose(fd);
//  free(buff);
//  message_cleanup(message);
//  message_cleanup(enc_msg);

  buff = malloc(strlen(argv[1]) + strlen("NuBL32PubKeyEncryptedHash.bin") + 1);
  buff[0] = 0;
  strcat(buff, argv[1]);
  strcat(buff, "NuBL32PubKeyEncryptedHash.bin");

  fd = fopen(buff, "wb");
  if (!fd) {
    printf("Failed to open file\n");
    free(buff);
    return -1;
  }
  free(buff);
  unsigned char PubKeyEncryptedHash[64];
  sha256((void *) enc_msg->body, 64, (unsigned char *) PubKeyEncryptedHash);

  if (fwrite((void *) PubKeyEncryptedHash, sizeof(char), 32, fd) != 32) {
    fclose(fd);
    return -1;
  }
  fclose(fd);
////////////////////
  message = message_init(64);
  memcpy((char *) message->body, (unsigned char *) ecc_ecdsa_key.Qx, 64);

  enc_msg = aes256_encrypt(message);
  //clean up ssl;
  //  EVP_cleanup();
  //  CRYPTO_cleanup_all_ex_data(); //Stop data leaks
  //  ERR_free_strings();

  buff = malloc(strlen(argv[1]) + strlen("NuBL33PubKeyEncrypted.bin") + 1);
  buff[0] = 0;
  strcat(buff, argv[1]);
  strcat(buff, "NuBL33PubKeyEncrypted.bin");

  fd = fopen(buff, "wb");
  if (!fd) {
    printf("Failed to open file\n");
    free(buff);
    return -1;
  }
  free(buff);

  if (fwrite((void *) enc_msg->body, sizeof(char), 64, fd) != 64) {
    fclose(fd);
    return -1;
  }
  fclose(fd);
//  free(buff);
//  message_cleanup(message);
//  message_cleanup(enc_msg);

  buff = malloc(strlen(argv[1]) + strlen("NuBL33PubKeyEncryptedHash.bin") + 1);
  buff[0] = 0;
  strcat(buff, argv[1]);
  strcat(buff, "NuBL33PubKeyEncryptedHash.bin");

  fd = fopen(buff, "wb");
  if (!fd) {
    printf("Failed to open file\n");
    free(buff);
    return -1;
  }
  free(buff);
//  unsigned char PubKeyEncryptedHash[64];
  sha256((void *) enc_msg->body, 64, (unsigned char *) PubKeyEncryptedHash);

  if (fwrite((void *) PubKeyEncryptedHash, sizeof(char), 32, fd) != 32) {
    fclose(fd);
    return -1;
  }
  fclose(fd);

////////////////////
  exit: BN_free(x);
  BN_free(y);
  EC_GROUP_free(ecgroup);
  EC_KEY_free(eckey);

  // clean up ssl;
//  EVP_cleanup();
//  CRYPTO_cleanup_all_ex_data(); //Stop data leaks
//  ERR_free_strings();

  return 0;
}
