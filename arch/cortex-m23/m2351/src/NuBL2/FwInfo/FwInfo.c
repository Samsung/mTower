/**************************************************************************//**
 * @file     FwInfo.c
 * @version  V1.00
 * @brief    NuBL2 F/W Info template and provided by NuBL2 developer.
 *
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"


const uint32_t g_InitialFWinfo[] =
{
    /* public key - 64-bytes (256-bits + 256-bits) */
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,

    /* metadata data - includes Mode selection, F/W region and Extend info */
    0x00000001, 0x00000008, 0x00000000, 0x00000000, // 0x00000000: NuBL2 F/W base
    0x0000000C, 0x20180918, 0x00000000, 0x00000000, // 0x20180918/0x00000000/0x00000000: Extend info 
    
    /* FW hash - 32-bytes (256-bits) */
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,

    /* FwInfo signature - 64-bytes (256-bits R + 256-bits S) */
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
};

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/
