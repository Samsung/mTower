/**************************************************************************//**
 * @file     FwInfo.c
 * @version  V1.00
 * @brief    NuBL2 FW Info template and provided by NuBL2 developer.
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2017-2020 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"
#include "NuBL2.h"

#include "printf.h"

/*
    Description of FwInfo format:   
    [ Public key ]
        Length: 
            Fixed 64 bytes.
        Content: 
            ECC public key which will be updated according [KEY] Public Key 1 and Public Key 2 in the FwInfo.ini after executing FwSign.exe. 
    [ Metadata data ]
        Length: 
            Fixed 4 words + 1 word Extend Info Length(N) + N words Extend Info Content.
        Content:
            Word-0, the ID Hash Configuration.
                bit[1:0], reserved.
                bit[2], indicates whether Info Hash includes PID hash, which will be updated according FwSign.ini after executing FwSign.exe.
                        If supports PID hash, add chip's PID to [PID] section in the FwInfo.ini as shown below example,
                        [PID]
                        PID=0x00235400
                bit[3], indicates whether Info Hash includes UID hash, which will be updated according FwSign.ini after executing FwSign.exe.
                        If supports UID hash, add chip's UID to [UID] section in the FwInfo.ini as shown below example,
                        [UID]
                        UID0=0x11111111
                        UID1=0x22222222
                        UID2=0x33333333
                bit[4], indicates whether Info Hash includes UCID hash, which will be updated according FwSign.ini after executing FwSign.exe.
                        If supports UCID hash, add chip's UCID to [UCID] section in the FwInfo.ini as shown below example,
                        [UCID]
                        UCID0=0xC1111111
                        UCID1=0xC2222222
                        UCID2=0xC3333333
                        UCID3=0xC4444444
                bit[31:5], reserved.
            Word-1, fixed 8(bytes) to indicate only NuBL2 FW region for secure boot verification.
            Word-2, indicates the NuBL2 FW base address.
            Word-3, indicates the NuBL2 FW size, which will be updated after NuBL2 is successfully built.
            Word-4, indicates the valid Extend Info Length. Must be a word alignment length.         
            Word-5~, the content of Extend Info.            
    [ FW hash ]
        Length: 
            Fixed 32 bytes.
        Content: 
            To store the NuBL2 FW hash.
            The target content will be updated according FW base and size in Metadata after executing FwSign.exe. 
    [ FwInfo signature ]
        Length: 
            Fixed 64 bytes.
        Content: 
            To store the ECDSA signature.
            The target content will be updated according the ECC private key in FwSign.ini after executing FwSign.exe. 
*/
const uint32_t g_InitialFWInfo[] =
{
    /* Public key - 64-bytes (256-bits + 256-bits) */
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,

    /* Metadata data - includes ID Hash Configuration, FW region and Extend Info */
    0x00000001, 0x00000008, 0x00000000, 0x00000000, // Word-2: 0x00000000, NuBL2 FW base
    0x0000000C, 0x20191201, 0x00000000, 0x00000000, // Word-5/6/7: 0x20191201/0x00000000/0x00000000, Extend Info
    
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
