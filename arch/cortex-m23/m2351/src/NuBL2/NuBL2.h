/**************************************************************************//**
 * @file     NuBL2.h
 * @version  V3.00
 * @brief    NuBL2 header file.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

#include "printf.h"

#define NUBL32_FW_INFO_BASE     0x00038000ul    // 224K   
#define NUBL33_FW_INFO_BASE     0x10078000ul    // 480K (Non-secure flash region)  


typedef struct
{
    uint32_t    u32Start;   /* 32-bits */
    uint32_t    u32Size;    /* 32-bits */
} __attribute__((packed)) FW_REGION_T;

typedef struct
{
    uint32_t        u32AuthCFGs;        /* 32-bits */
                        /* 
                            bit[1:0]:   Reserved
                            bit[2]:     1: Info Hash includes PDID / 0: Not include PDID
                            bit[3]:     1: Info Hash includes UID / 0: Not include UID
                            bit[4]:     1: Info Hash inculdes UCID / 0: Not include UCID
                            bit[31:5]:  Reserved
                        */
    uint32_t        u32FwRegionLen;     /* 32-bits */
    FW_REGION_T     au32FwRegion[1];    /* (8*1) bytes */
    uint32_t        u32ExtInfoLen;      /* 32-bits */
    uint32_t        au32ExtInfo[3];     /* 12-bytes */    
} __attribute__((packed)) METADATA_T;

typedef struct
{
    ECC_PUBKEY_T    pubkey;             /* 64-bytes (256-bits + 256-bits) */
    
    METADATA_T      mData;              /* includes authenticate configuration, F/W regions and extend info */ 
        
    uint32_t        au32FwHash[8];      /* 32-bytes (256-bits) */
        
    ECDSA_SIGN_T    sign;               /* 64-bytes (256-bits R + 256-bits S) */    
} __attribute__((packed)) FW_INFO_T;


int32_t VerifyNuBL3x(uint32_t *pu32FwInfo, uint32_t u32InfoBase);

extern const uint32_t g_InitialFWInfo[]; // A global variable to store NuBL2 FWINFO address, declared in FwInfo.c

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/
