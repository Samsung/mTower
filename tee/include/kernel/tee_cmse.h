/**
 * @file        tee/include/kernel/tee_cmse.h
 * @brief       mTower cmse macros definition
 *
 * @copyright   Copyright (c) 2021 Xiaomi Co., Ltd. All Rights Reserved.
 * @author      Jacky Yang yangxinle@xiaomi.com
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
 * 
 **/

#ifndef KERNEL_TEE_CMSE_H
#define KERNEL_TEE_CMSE_H

#include <arm_cmse.h>

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3L)
# if defined (__ICCARM__)
#  define __NONSECURE_ENTRY       __cmse_nonsecure_entry
#  define __NONSECURE_ENTRY_WEAK  __cmse_nonsecure_entry //__weak
#  define __NONSECURE_CALL        __cmse_nonsecure_call
# else
#  define __NONSECURE_ENTRY       __attribute__((cmse_nonsecure_entry))
#  define __NONSECURE_ENTRY_WEAK  __attribute__((cmse_nonsecure_entry,weak))
#  define __NONSECURE_CALL        __attribute__((cmse_nonsecure_call))
# endif
#else
# define __NONSECURE_ENTRY
# define __NONSECURE_ENTRY_WEAK
# define __NONSECURE_CALL
#endif

#endif /* KERNEL_TEE_CMSE_H */