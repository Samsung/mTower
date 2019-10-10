/**************************************************************************//**
 * @file     VerifyNuBL3x.c
 * @version  V3.00
 * @brief    This source file is used to authenticate the NuBL32 and NuBL33.
 *
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"
#include "partition_M2351.h"
#include "NuBL2.h"

#define printf(...)
#define DBG_EN      0


static void BytesSwap(char *buf, int32_t len)
{
    int32_t i;
    char    tmp;

    for(i = 0; i < (len / 2); i++)
    {
        tmp = buf[len - i - 1];
        buf[len - i - 1] = buf[i];
        buf[i] = tmp;
    }
}

void Cal_SHA256_Flash(uint32_t u32Addr, uint32_t u32Bytes, uint32_t *pu32Digest)
{
    volatile int32_t    i, addr, bytes, data;
    
    addr = u32Addr;
    bytes = u32Bytes;
        
    /* Enable CRYPTO module clock */
    CLK->AHBCLK |= CLK_AHBCLK_CRPTCKEN_Msk;
    
    /* Reset CRYPTO module */
    SYS->IPRST0 |= SYS_IPRST0_CRPTRST_Msk;
    SYS->IPRST0 &= ~SYS_IPRST0_CRPTRST_Msk;
        
    CRPT->HMAC_CTL = (SHA_MODE_SHA256 << CRPT_HMAC_CTL_OPMODE_Pos) | CRPT_HMAC_CTL_INSWAP_Msk | CRPT_HMAC_CTL_OUTSWAP_Msk;
    CRPT->HMAC_DMACNT = 64;
    CRPT->HMAC_CTL |= CRPT_HMAC_CTL_START_Msk;

    /* Start to calculate ... */
    while(bytes > 0)
    {
        if(bytes < 64)
            CRPT->HMAC_DMACNT = bytes;

        if(CRPT->HMAC_STS & CRPT_HMAC_STS_DATINREQ_Msk)
        {
            data = inpw(addr);
            addr += 4;
            bytes -= 4;

            if(bytes <= 0)
                bytes = 0;

            /* bytes means remain byte counts */
            if(bytes != 0)
            {
                CRPT->HMAC_DATIN = data;
            }
            else
            {
                /* It's last word ... *-* */
                CRPT->HMAC_CTL |= CRPT_HMAC_CTL_START_Msk | CRPT_HMAC_CTL_DMALAST_Msk;
                CRPT->HMAC_DATIN = data;
                while(CRPT->HMAC_STS & CRPT_HMAC_STS_BUSY_Msk);

                for(i=0; i<8; i++)
                    pu32Digest[i] = *(uint32_t *)((uint32_t) & (CRPT->HMAC_DGST[0]) + (i * 4));
            }
        }
    }
    
#if (DBG_EN == 1) // enable for debug
    printf("Cal_SHA256_Flash\n");
    printf("    0x%08x\n", pu32Digest[0]);
    printf("    0x%08x\n", pu32Digest[1]);
    printf("    0x%08x\n", pu32Digest[2]);
    printf("    0x%08x\n", pu32Digest[3]);
    printf("    0x%08x\n", pu32Digest[4]);
    printf("    0x%08x\n", pu32Digest[5]);
    printf("    0x%08x\n", pu32Digest[6]);
    printf("    0x%08x\n", pu32Digest[7]);
#endif    
}

void Cal_SHA256_SRAM(uint32_t u32Addr, uint32_t u32Bytes, uint32_t *pu32Digest)
{
    /* Enable CRYPTO module clock */
    CLK_EnableModuleClock(CRPT_MODULE);
    
    /* Reset CRYPTO module */
    SYS_ResetModule(CRPT_MODULE);
    
    SHA_ENABLE_INT(CRPT);

    /*---------------------------------------
     *  SHA-256
     *---------------------------------------*/
    XSHA_Open(XCRPT, SHA_MODE_SHA256, SHA_IN_OUT_SWAP, 0);
    
    XSHA_SetDMATransfer(XCRPT, u32Addr, u32Bytes);

    XSHA_Start(XCRPT, CRYPTO_DMA_ONE_SHOT);
    while(SHA_GET_INT_FLAG(CRPT) == 0) {}
    SHA_CLR_INT_FLAG(CRPT);

    XSHA_Read(XCRPT, pu32Digest);
        
#if (DBG_EN == 1) // enable for debug
    printf("Cal_SHA256_SRAM\n");
    printf("    0x%08x\n", pu32Digest[0]);
    printf("    0x%08x\n", pu32Digest[1]);
    printf("    0x%08x\n", pu32Digest[2]);
    printf("    0x%08x\n", pu32Digest[3]);
    printf("    0x%08x\n", pu32Digest[4]);
    printf("    0x%08x\n", pu32Digest[5]);
    printf("    0x%08x\n", pu32Digest[6]);
    printf("    0x%08x\n", pu32Digest[7]);
#endif        
}

static uint32_t *SetAES256Key(uint32_t *key)
{
// AES: 1234567800000000000000000000000000000000000000000000000000accdef
    key[0] = 0x12345678;
    key[1] = 0x00000000;
    key[2] = 0x00000000;
    key[3] = 0x00000000;
    key[4] = 0x00000000;
    key[5] = 0x00000000;
    key[6] = 0x00000000;
    key[7] = 0x00accdef;
    
    return key;
}

static uint32_t *SetAESIV(uint32_t *iv)
{
// IV: 12345678000000000000000000accdef
    iv[0] = 0x12345678;
    iv[1] = 0x00000000;
    iv[2] = 0x00000000;
    iv[3] = 0x00accdef;
    
    return iv;
}

static int32_t IdentifyNuBL3xPubKey(uint32_t *pu32FwInfo, uint32_t u32InfoBase)
{
    FW_INFO_T       *pFwInfo;
    ECC_PUBKEY_T    PubKey;
    uint32_t        au32Hash0[8], au32Hash1[8], au32KeyBuf[8];
    
    if(u32InfoBase == NUBL32_FW_INFO_BASE)
    {
        /* Get encrypted NuBL32 public key and verify its hash value */
        extern uint32_t g_NuBL32KeyStart, g_NuBL32KeyEnd;           // declared in NuBL3xKeyStorage.s
        extern uint32_t g_NuBL32KeyHashStart, g_NuBL32KeyHashEnd;   // declared in NuBL3xKeyStorage.s
        memcpy((void *)&PubKey, (void *)&g_NuBL32KeyStart, ((uint32_t)&g_NuBL32KeyEnd-(uint32_t)&g_NuBL32KeyStart));
        memcpy((void *)&au32Hash0, (void *)&g_NuBL32KeyHashStart, ((uint32_t)&g_NuBL32KeyHashEnd-(uint32_t)&g_NuBL32KeyHashStart));
    }
    else if (u32InfoBase == NUBL33_FW_INFO_BASE)
    {
        /* Get encrypted NuBL33 public key and verify its hash value */
        extern uint32_t g_NuBL33KeyStart, g_NuBL33KeyEnd;           // declared in NuBL3xKeyStorage.s
        extern uint32_t g_NuBL33KeyHashStart, g_NuBL33KeyHashEnd;   // declared in NuBL3xKeyStorage.s
        memcpy((void *)&PubKey, (void *)&g_NuBL33KeyStart, ((uint32_t)&g_NuBL33KeyEnd-(uint32_t)&g_NuBL33KeyStart));
        memcpy((void *)&au32Hash0, (void *)&g_NuBL33KeyHashStart, ((uint32_t)&g_NuBL33KeyHashEnd-(uint32_t)&g_NuBL33KeyHashStart));
    }
    else
    {
        printf("\n\tInvalid F/W info base address.\n");
        return -1;
    }
    Cal_SHA256_SRAM((uint32_t)&PubKey, sizeof(ECC_PUBKEY_T), (uint32_t *)au32Hash1);
    if(memcmp(au32Hash0, au32Hash1, sizeof(au32Hash0)) != 0)
    {
        printf("\n\tVerify public key hash FAIL.\n");
        return -1;
    }
#if (DBG_EN == 1) // enable for debug
    printf("key, encrypted\n");
    printf("    0x%08x\n", PubKey.au32Key0[0]);
    printf("    0x%08x\n", PubKey.au32Key0[1]);
    printf("    0x%08x\n", PubKey.au32Key0[2]);
    printf("    0x%08x\n", PubKey.au32Key0[3]);
    printf("    0x%08x\n", PubKey.au32Key0[4]);
    printf("    0x%08x\n", PubKey.au32Key0[5]);
    printf("    0x%08x\n", PubKey.au32Key0[6]);
    printf("    0x%08x\n", PubKey.au32Key0[7]);
    printf("    0x%08x\n", PubKey.au32Key1[0]);
    printf("    0x%08x\n", PubKey.au32Key1[1]);
    printf("    0x%08x\n", PubKey.au32Key1[2]);
    printf("    0x%08x\n", PubKey.au32Key1[3]);
    printf("    0x%08x\n", PubKey.au32Key1[4]);
    printf("    0x%08x\n", PubKey.au32Key1[5]);
    printf("    0x%08x\n", PubKey.au32Key1[6]);
    printf("    0x%08x\n", PubKey.au32Key1[7]);
#endif    
    
    
    /* Get NuBL3x F/W info */
    pFwInfo = (FW_INFO_T *)pu32FwInfo;
    
    
    /* Decrypted NuBL3x public key and verify with F/W info */
    
    /* Enable CRYPTO module clock */
    CLK_EnableModuleClock(CRPT_MODULE);
    
    AES_ENABLE_INT(CRPT);
    
    XAES_Open(XCRPT, 0, 0, AES_MODE_CFB, AES_KEY_SIZE_256, AES_IN_OUT_SWAP);
    XAES_SetKey(XCRPT, 0, SetAES256Key(au32KeyBuf), AES_KEY_SIZE_256);
    XAES_SetInitVect(XCRPT, 0, SetAESIV(au32KeyBuf));
    XAES_SetDMATransfer(XCRPT, 0, (uint32_t)&PubKey, (uint32_t)&PubKey, sizeof(ECC_PUBKEY_T));

    XAES_Start(XCRPT, 0, CRYPTO_DMA_ONE_SHOT);
    while(AES_GET_INT_FLAG(CRPT) == 0) {}
    AES_CLR_INT_FLAG(CRPT);       
#if (DBG_EN == 1) // enable for debug
    printf("key, decrypted\n");
    printf("    0x%08x\n", PubKey.au32Key0[0]);
    printf("    0x%08x\n", PubKey.au32Key0[1]);
    printf("    0x%08x\n", PubKey.au32Key0[2]);
    printf("    0x%08x\n", PubKey.au32Key0[3]);
    printf("    0x%08x\n", PubKey.au32Key0[4]);
    printf("    0x%08x\n", PubKey.au32Key0[5]);
    printf("    0x%08x\n", PubKey.au32Key0[6]);
    printf("    0x%08x\n", PubKey.au32Key0[7]);
    printf("    0x%08x\n", PubKey.au32Key1[0]);
    printf("    0x%08x\n", PubKey.au32Key1[1]);
    printf("    0x%08x\n", PubKey.au32Key1[2]);
    printf("    0x%08x\n", PubKey.au32Key1[3]);
    printf("    0x%08x\n", PubKey.au32Key1[4]);
    printf("    0x%08x\n", PubKey.au32Key1[5]);
    printf("    0x%08x\n", PubKey.au32Key1[6]);
    printf("    0x%08x\n", PubKey.au32Key1[7]);
#endif    
        
    if(memcmp((void *)&pFwInfo->pubkey.au32Key0[0], &PubKey, sizeof(ECC_PUBKEY_T)) != 0)
    {
        printf("\n\tCompare public key FAIL.\n");
        return -2;
    }
    
    return 0;
}

int32_t VerifyNuBL3x(uint32_t *pu32FwInfo, uint32_t u32InfoBase)
{
    FW_INFO_T   *pFwInfo;
    uint32_t    u32Start, u32Size, au32Hash[8];
    uint32_t    tmp[8];
    char m[65], Qx[65], Qy[65], R[65], S[65];
    
    
    /* Get NuBL3x F/W info */
    pFwInfo = (FW_INFO_T *)pu32FwInfo;
        
    /*---------------------------------------------------------------------------------------------------------*/
    /*  Verify NuBL3x identity (cehck NuBL3x public key)                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    if(IdentifyNuBL3xPubKey((uint32_t *)pFwInfo, u32InfoBase) < 0)
    {
        printf("\n\tIdentify F/W public key FAIL!!\n");
        return -1;
    }
    else
    {
        printf("\nIdentify F/W public key PASS.\n");
    }

        
    /*---------------------------------------------------------------------------------------------------------*/
    /*  Verify NuBL3x F/W info ECDSA signature                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Calculate message (NuBL3x F/W info hash) */
    u32Start = (uint32_t)pFwInfo;
    u32Size  = sizeof(FW_INFO_T) - sizeof(ECDSA_SIGN_T);
    Cal_SHA256_SRAM(u32Start, u32Size, (uint32_t *)au32Hash);
    memcpy((void*)tmp, (uint32_t *)au32Hash, sizeof(tmp));   
    BytesSwap((char*)tmp,  sizeof(tmp));
    XECC_Reg2Hex(64, tmp, m);

    /* Get Qx */
    memcpy((void*)tmp, (uint32_t *)pFwInfo->pubkey.au32Key0, sizeof(tmp));
    BytesSwap((char*)tmp,  sizeof(tmp));
    XECC_Reg2Hex(64, tmp, Qx);

    /* Get Qy */
    memcpy((void*)tmp, (uint32_t *)pFwInfo->pubkey.au32Key1, sizeof(tmp));
    BytesSwap((char*)tmp,  sizeof(tmp));
    XECC_Reg2Hex(64, tmp, Qy);

    /* Get R */
    memcpy((void*)tmp, (uint32_t *)pFwInfo->sign.au32R, sizeof(tmp));
    BytesSwap((char*)tmp,  sizeof(tmp));
    XECC_Reg2Hex(64, tmp, R);

    /* Get S */
    memcpy((void*)tmp, (uint32_t *)pFwInfo->sign.au32S, sizeof(tmp));
    BytesSwap((char*)tmp,  sizeof(tmp));
    XECC_Reg2Hex(64, tmp, S);
    
#if (DBG_EN == 1) // enable for debug
    printf("Input:\n");
    printf(" m\t%s\n", m);
    printf(" Qx\t%s\n", Qx);
    printf(" Qy\t%s\n", Qy);
    printf(" R\t%s\n", R);
    printf(" S\t%s\n", S);
#endif    

    ECC_ENABLE_INT(CRPT);
    if(XECC_VerifySignature(XCRPT, CURVE_P_256, m, Qx, Qy, R, S) < 0)
    {
        printf("\n\tF/W info ECDSA signature verification FAIL!!\n");
        return -1;
    }
    else
    {
        printf("\nF/W info ECDSA signature verification PASS.\n");
    }


    /*---------------------------------------------------------------------------------------------------------*/
    /*  Verify NuBL3x F/W integrity                                                                            */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Calculate F/W hash */
    u32Start = (uint32_t)pFwInfo->mData.au32FwRegion[0].u32Start;
    u32Size  = (uint32_t)pFwInfo->mData.au32FwRegion[0].u32Size;
    Cal_SHA256_Flash(u32Start, u32Size, (uint32_t *)au32Hash);
    if(memcmp((void*)&pFwInfo->au32FwHash[0], au32Hash, sizeof(au32Hash)) != 0)
    {
        printf("\n\tVerify F/W hash integrity FAIL.\n");
        return -1;
    }
    else
    {
        printf("\nVerify F/W hash integrity PASS.\n\n");
    }
    
    return 0UL;
}

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/
