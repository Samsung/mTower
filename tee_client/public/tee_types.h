/**
 * @file        /mTower/apps/tee_types.h
 * @brief       Short file description
 *
 * @copyright   Copyright (c) 2018 Samsung Electronics Co., Ltd. All Rights Reserved.
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
 *
 * @todo
 */

#ifndef __APPS_TEE_TYPES_H_
#define __APPS_TEE_TYPES_H_

/* Included Files */
/* All header files are included here. */
#include <stddef.h>
#include <stdint.h>

/* Pre-processor Definitions */
/* All C pre-processor macros are defined here. */

#ifndef false
#define false               (0)
#endif

#ifndef true
#define true                (1)
#endif

#ifndef NULL
#define NULL                ((void *)0)
#endif

/* Public Types */
/* Any types, enumerations, structures or unions are defined here. */
#ifndef __STDINT__

#ifndef _INT8_T_DECLARED
typedef signed char                int8_t;
#endif
typedef unsigned char       uint8_t;

typedef short               int16_t;
typedef unsigned short      uint16_t;

#ifndef _INT32_T_DECLARED
typedef int                 int32_t;
#endif
#ifndef _UINT32_T_DECLARED
typedef unsigned int        uint32_t;
#endif

typedef long long           int64_t;
typedef unsigned long long  uint64_t;

//typedef unsigned long       size_t;
//typedef long                ssize_t;

typedef long                long_t;
typedef unsigned long       ulong_t;

//typedef unsigned long int   uintptr_t;

#endif /* __STDINT__ */

typedef unsigned char       bool;
typedef uint32_t            tee_stat_t;


/* Public Data */
/* All data declarations with global scope appear here, preceded by
 * the definition EXTERN.
 */


/* Inline Functions */
/* Any static inline functions may be defined in this grouping.
 * Each is preceded by a function header similar to the below.
 */

/**
 * @brief         <Inline function name> - Description of the operation
 *                of the static function.
 *
 * @param value   [in]/[out] A list of input parameters, one-per-line,
 *                appears here along with a description of each input
 *                parameter.
 *
 * @returns       Description of the value returned by this function
 *                (if any), including an enumeration of all possible
 *                error values.
 */

/* Public Function Prototypes */
/* All global functions in the file are prototyped here. The keyword
 * extern or the definition EXTERN are never used with function
 * prototypes.
 */

/**
 * @brief         <Global function name> - Description of the operation
 *                of the static function.
 *
 * @param value   [in]/[out] A list of input parameters, one-per-line,
 *                appears here along with a description of each input
 *                parameter.
 *
 * @returns       Description of the value returned by this function
 *                (if any), including an enumeration of all possible
 *                error values.
 */


#endif /* __APPS_TEE_TYPES_H_ */
