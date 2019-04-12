/**
 * @file        /mTower/apps/hello_world/ca/hello_world_ns.c
 * @brief       <Short file description>
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
 * @bug
 */

/* Included Files. */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/* GP TEE client API */
#include "tee_types.h"
#include "tee_client_api.h"
/* TA UUID */
#include "hello_world2_ta.h"

/* Pre-processor Definitions. */
/* All C pre-processor macros are defined here. */


/* Private Types. */
/* Any types, enums, structures or unions used by the file are defined here. */


/* Private Function Prototypes. */
/* Prototypes of all static functions in the file are provided here. */
int tee_hello_world2(void);

/* Private Data. */
/* All static data definitions appear here. */


/* Public Data. */
/* All data definitions with global scope appear here. */

/* Private Functions. */

/**
 * @brief         <Static function name> - Description of the operation
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

/* All static functions in the file are defined in this grouping. Each is
 * preceded by a function header similar to the above.
 */

/* Public Functions. */

/**
 * @brief         <Function name > - Description of the operation of
 *                the static function.
 *
 * @param value   [in]/[out] A list of input parameters, one-per-line,
 *                appears here along with a description of each input
 *                parameter.
 *
 * @returns       Description of the value returned by this function
 *                (if any), including an enumeration of all possible
 *                error values.
 */
//#define  errx(i ,format, ...) printf(format "\n", ##__VA_ARGS__)

int tee_hello_world2()
{

  TEEC_Result res = 0;
  TEEC_Context ctx;
  TEEC_Session sess;
  TEEC_Operation op;
  TEEC_UUID uuid = TA_HELLO_WORLD_UUID;
  uint32_t err_origin;

  /* Initialize a context connecting us to the TEE */
  res = TEEC_InitializeContext(NULL, &ctx);
  if (res != TEEC_SUCCESS)
    errx(1, "TEEC_InitializeContext failed with code 0x%x", res);
  else
    printf("TEEC_InitializeContext successfully with code 0x%x\n", res);

  /*
   * Open a session to the "hello world" TA, the TA will print "hello
   * world!" in the log when the session is created.
   */
  res = TEEC_OpenSession(&ctx, &sess, &uuid,
             TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
  if (res != TEEC_SUCCESS)
    errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
      res, err_origin);
  else
    printf("TEEC_Opensession successfully with code 0x%x\n", res);

  /*
   * Execute a function in the TA by invoking it, in this case
   * we're incrementing a number.
   *
   * The value of command ID part and how the parameters are
   * interpreted is part of the interface provided by the TA.
   */

  /* Clear the TEEC_Operation struct */
  memset(&op, 0, sizeof(op));

  /*
   * Prepare the argument. Pass a value in the first parameter,
   * the remaining three parameters are unused.
   */
  op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
           TEEC_NONE, TEEC_NONE);
  op.params[0].value.a = 42;

  /*
   * TA_HELLO_WORLD_CMD_DEC_VALUE is the actual function in the TA to be
   * called.
   */
  printf("Invoking TA to decrement %d\n", op.params[0].value.a);
  res = TEEC_InvokeCommand(&sess, TA_HELLO_WORLD_CMD_DEC_VALUE, &op,
         &err_origin);
  if (res != TEEC_SUCCESS)
    errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
      res, err_origin);
  else
    printf("TEEC_InvokeCommand successfully with code 0x%x\n", res);

  printf("TA decremented value to %d\n", op.params[0].value.a);

  /*
   * We're done with the TA, close the session and
   * destroy the context.
   *
   * The TA will print "Goodbye!" in the log when the
   * session is closed.
   */

cleanup3:
  TEEC_CloseSession(&sess);
cleanup2:
  TEEC_FinalizeContext(&ctx);

  return res;
}

/* All global functions in the file are defined here. */
