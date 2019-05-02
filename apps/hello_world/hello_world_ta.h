/**
 * @file        /mTower/apps/hello_world/hello_world_ta.h
 * @brief       Definition of hello_world ta and ca.
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
 *
 * @todo
 */

#ifndef __APPS_HELLO_WORLD_HELLO_WORLD_TA_H_
#define __APPS_HELLO_WORLD_HELLO_WORLD_TA_H_

/* Included Files */
/* All header files are included here. */

/* Pre-processor Definitions */
/* All C pre-processor macros are defined here. */
/*
 * This UUID is generated with uuidgen
 * the ITU-T UUID generator at http://www.itu.int/ITU-T/asn1/uuid.html
 */
#define TA_HELLO_WORLD_UUID \
  { 0x8aaaf200, 0x2450, 0x11e4, \
    { 0xab, 0xe2, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b} }

/* The function IDs implemented in this TA */
#define TA_HELLO_WORLD_CMD_INC_VALUE    0
#define TA_HELLO_WORLD_CMD_DEC_VALUE    1


/* Public Types */
/* Any types, enumerations, structures or unions are defined here. */


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


#endif /* __APPS_HELLO_WORLD_HELLO_WORLD_TA_H_ */
