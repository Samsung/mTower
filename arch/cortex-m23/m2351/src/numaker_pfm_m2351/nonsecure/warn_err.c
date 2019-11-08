/**
 * @file        arch/arm/m2351/src/numaker_pfm_m2351/nonsecure/warn_err.c
 * @brief       Provides functionality to start, initialize, load and execute
 *              FreeRTOS tasks
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
#include <string.h>
#include <stdarg.h>

/* Pre-processor Definitions. */
/* All C pre-processor macros are defined here. */

/* Private Types. */
/* Any types, enums, structures or unions used by the file are defined here. */

/* Private Function Prototypes. */
/* Prototypes of all static functions in the file are provided here. */

/* NonSecure functions used for callbacks */


/* Private Data. */
/* All static data definitions appear here. */


/* Public Data. */
/* All data definitions with global scope appear here. */


/* Extern Function Prototypes */


/* Private Functions. */


/* Public Functions. */
/**
 * @brief         errx - .
 *
 * @param         None
 *
 * @returns       None
 */

//#define  errx(i ,format, ...) printf(format "\n", ##__VA_ARGS__)
void errx(int eval, const char *fmt, ...)
{
  va_list va;

  (void)eval;
  va_start(va, fmt);
  vfprintf(stdout, fmt, va);
  fflush(stdout);
  va_end(va);

//  vTaskDelete(NULL);
}

