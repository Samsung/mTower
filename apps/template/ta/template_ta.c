/**
 * @file        /mTower/apps/template/ta/template_ta.c
 * @brief       Template of Trusted application.
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
 */

/* Included Files. */
#include <tee_internal_api.h>
#include <trace.h>
#include <template_ta.h>
#include <kernel/pseudo_ta.h>

/* Pre-processor Definitions. */
/* All C pre-processor macros are defined here. */


/* Private Types. */
/* Any types, enums, structures or unions used by the file are defined here. */


/* Private Function Prototypes. */
/* Prototypes of all static functions in the file are provided here. */


/* Private Data. */
/* All static data definitions appear here. */


/* Public Data. */
/* All data definitions with global scope appear here. */

/* Private Functions. */

/**
 * @brief         test - Function that does nothing.
 *
 * @param param_types   [in] encodes the type of each of the Parameters in the
 *                operation.

 * @param params  [in] An array of four Parameters. For each parameter, one of
 *                the memref, tmpref, or value fields must be used depending on
 *                the corresponding parameter type passed in paramTypes as
 *                described in the specification of TEEC_Parameter.
 *
 * @returns       Description of the value returned by this function
 *                (if any), including an enumeration of all possible
 *                error values.
 */
static TEE_Result test(uint32_t param_types, TEE_Param params[4])
{
  uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
      TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);

  DMSG("has been called");

  if (param_types != exp_param_types)
    return TEE_ERROR_BAD_PARAMETERS;

  return TEE_SUCCESS;
}

/* All static functions in the file are defined in this grouping. Each is
 * preceded by a function header similar to the above.
 */

/* Public Functions. */

/**
 * @brief         Template_TA_CreateEntryPoint - Called when the instance of
 *                the TA is created. This is the first call in the TA.
 *
 * @returns       TEE_SUCCESS on success, or error code on failure (see
 *                mTower tee_internal/include/tee_api_defines.h for details).
 */
TEE_Result Template_TA_CreateEntryPoint(void)
{
  DMSG("has been called");

  return TEE_SUCCESS;
}

/**
 * @brief         Template_TA_DestroyEntryPoint - Called when the instance of
 *                the TA is destroyed if the TA has not crashed or panicked.
 *                This is the last call in the TA.
 *
 * @returns       None.
 */
void Template_TA_DestroyEntryPoint(void)
{
  DMSG("has been called");
}

/**
 * @brief         Template_TA_OpenSessionEntryPoint - Called when a new session
 *                is opened to the TA. *sess_ctx can be updated with a value to
 *                be able to identify this session in subsequent calls to the
 *                TA. In this function you will normally do the global
 *                initialization for the TA.
 *
 * @param param_types   [in] encodes the type of each of the Parameters in the
 *                operation.

 * @param params  [in] An array of four Parameters. For each parameter, one of
 *                the memref, tmpref, or value fields must be used depending on
 *                the corresponding parameter type passed in paramTypes as
 *                described in the specification of TEEC_Parameter.

 * @param sess_ctx  [out] A point to a valid open Session.
 *
 * @returns       TEE_SUCCESS on success, or error code on failure (see
 *                mTower tee_internal/include/tee_api_defines.h for details).
 */
TEE_Result Template_TA_OpenSessionEntryPoint(uint32_t param_types,
    TEE_Param __maybe_unused params[4], void __maybe_unused **sess_ctx)
{
  uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
      TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);

  DMSG("has been called");

  if (param_types != exp_param_types)
    return TEE_ERROR_BAD_PARAMETERS;

  /* Unused parameters */
  (void) &params;
  (void) &sess_ctx;

  /* If return value != TEE_SUCCESS the session will not be created. */
  return TEE_SUCCESS;
}

/**
 * @brief         Template_TA_CloseSessionEntryPoint - Called when a session is
 *                closed, sess_ctx hold the value that was assigned by
 *                TA_OpenSessionEntryPoint().
 *
 * @param sess_ctx  [in] a point to close Session.
 *
 * @returns       None.
 */
void Template_TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
  (void) &sess_ctx; /* Unused parameter */

  DMSG("has been called");
}

/**
 * @brief         Template_TA_InvokeCommandEntryPoint - Called when a TA is
 *                invoked. sess_ctx hold that value that was assigned by
 *                TA_OpenSessionEntryPoint(). The rest of the paramters comes
 *                from normal world.
 *
 * @param sess_ctx  [in] A point to a valid open Session.
 *
 * @param param_types   [in] encodes the type of each of the Parameters in the
 *                operation.

 * @param params  [in] An array of four Parameters. For each parameter, one of
 *                the memref, tmpref, or value fields must be used depending on
 *                the corresponding parameter type passed in paramTypes as
 *                described in the specification of TEEC_Parameter.
 *
 * @returns       TEE_SUCCESS on success, or error code on failure (see
 *                mTower tee_internal/include/tee_api_defines.h for details).
 */
TEE_Result Template_TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx,
    uint32_t cmd_id, uint32_t param_types, TEE_Param params[4])
{
  (void) &sess_ctx; /* Unused parameter */

  switch (cmd_id) {
    case TA_TEMPLATE_CMD_TEST:
      return test(param_types, params);
    default:
      return TEE_ERROR_BAD_PARAMETERS;
  }
}

pseudo_ta_register(.uuid = TA_TEMPLATE_UUID, .name = "Template",
    .flags = PTA_DEFAULT_FLAGS | TA_FLAG_CONCURRENT, .open_session_entry_point =
        Template_TA_OpenSessionEntryPoint, .close_session_entry_point =
        Template_TA_CloseSessionEntryPoint, .invoke_command_entry_point =
        Template_TA_InvokeCommandEntryPoint);

