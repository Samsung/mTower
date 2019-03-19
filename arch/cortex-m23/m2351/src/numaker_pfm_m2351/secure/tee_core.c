/**
 * @file        arch/arm/m2351/src/numaker_pfm_m2351/secure/tee_core.c
 * @brief       Provides functionality to start secure world, initialize secure
 *              and normal worlds, pass to execution to normal world.
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
#include <arm_cmse.h>
#include <stdio.h>
#include "M2351.h"
#include "partition_M2351.h"

/* GP TEE client API */
#include "tee_types.h"
#include "tee_client_api.h"
#include "tee.h"

/* Pre-processor Definitions. */


/* Private Types. */
/* Any types, enums, structures or unions used by the file are defined here. */
/* typedef for NonSecure callback functions */
typedef __NONSECURE_CALL int32_t (*NonSecure_funcptr)(uint32_t);
typedef int32_t (*Secure_funcptr)(uint32_t);

/* Private Function Prototypes. */
/* Prototypes of all static functions in the file are provided here. */


/* Private Data. */
/* All static data definitions appear here. */


/* Public Data. */
/* All data definitions with global scope appear here. */


/* Public Function Prototypes */


/* Private Functions. */
TEEC_Result tee_ioctl_open_session(/*ctx,*/ struct tee_ioctl_buf_data *buf_data);
TEEC_Result tee_ioctl_invoke(/*ctx,*/ struct tee_ioctl_buf_data *buf_data);
TEEC_Result tee_ioctl_close_session(/*ctx,*/ struct tee_ioctl_close_session_arg *arg);

/* Public Functions. */

/* Secure functions exported to NonSecure application. Must place in Non-secure
 * Callable */

/**
 * @brief         ioctl - .
 *
 * @param         None
 *
 * @returns       None
 */
__NONSECURE_ENTRY
int32_t ioctl(uint32_t cmd, struct tee_ioctl_buf_data *buf_data)
{
  printf("Secure ioctl: cmd = %x\n", cmd);

  switch (cmd) {
//  case TEE_IOC_VERSION:
//    return tee_ioctl_version(ctx, uarg);
//  case TEE_IOC_SHM_ALLOC:
//    return tee_ioctl_shm_alloc(ctx, uarg);
//  case TEE_IOC_SHM_REGISTER:
//    return tee_ioctl_shm_register(ctx, uarg);
//  case TEE_IOC_SHM_REGISTER_FD:
//    return tee_ioctl_shm_register_fd(ctx, uarg);
  case TEE_IOC_OPEN_SESSION:
    return tee_ioctl_open_session(/*ctx,*/ buf_data);
  case TEE_IOC_INVOKE:
    return tee_ioctl_invoke(/*ctx,*/ buf_data);
//  case TEE_IOC_CANCEL:
//    return tee_ioctl_cancel(ctx, uarg);
  case TEE_IOC_CLOSE_SESSION:
    return tee_ioctl_close_session(/*ctx,*/ buf_data);
//  case TEE_IOC_SUPPL_RECV:
//    return tee_ioctl_supp_recv(ctx, uarg);
//  case TEE_IOC_SUPPL_SEND:
//    return tee_ioctl_supp_send(ctx, uarg);
  default:
    return TEEC_ERROR_ITEM_NOT_FOUND;
  }

  return TEEC_SUCCESS;
}

/**
 * struct tee_ioctl_open_session_arg - Open session argument
 * @uuid: [in] UUID of the Trusted Application
 * @clnt_uuid:  [in] UUID of client
 * @clnt_login: [in] Login class of client, TEE_IOCTL_LOGIN_* above
 * @cancel_id:  [in] Cancellation id, a unique value to identify this request
 * @session:  [out] Session id
 * @ret:  [out] return value
 * @ret_origin  [out] origin of the return value
 * @num_params  [in] number of parameters following this struct
 */
//struct tee_ioctl_open_session_arg {
//  uint8_t uuid[TEE_IOCTL_UUID_LEN];
//  uint8_t clnt_uuid[TEE_IOCTL_UUID_LEN];
//  uint32_t clnt_login;
//  uint32_t cancel_id;
//  uint32_t session;
//  uint32_t ret;
//  uint32_t ret_origin;
//  uint32_t num_params;

static void uuid_print(uint8_t d[TEE_IOCTL_UUID_LEN])
{
//  printf("UUID = %08x\n",
//      ((uint32_t) (d[0] << 24)) | (uint32_t) (d[1] << 16)
//          | (uint32_t) (d[2] << 8) | (uint32_t) d[3]);

  TEEC_UUID s;
  int i;
  s.timeLow = ((uint32_t)d[0] << 24);
  s.timeLow |= ((uint32_t)d[1] << 16);
  s.timeLow |= ((uint32_t)d[2] << 8);
  s.timeLow |= ((uint32_t)d[3]);
  s.timeMid = ((uint16_t)d[4] << 8);
  s.timeMid |= ((uint16_t)d[5]);
  s.timeHiAndVersion = ((uint16_t)d[6] << 8);
  s.timeHiAndVersion |= ((uint16_t)d[7]);
  memcpy(s.clockSeqAndNode, d + 8, sizeof(s.clockSeqAndNode));

  printf("UUID = %08x-%04x-%04x", s.timeLow, s.timeMid, s.timeHiAndVersion);
  for (i = 0; i < 8; ++i) {
    printf("-%02x",s.clockSeqAndNode[i]);
  }
  printf("\n");
}

TEEC_Result tee_ioctl_open_session(/*ctx,*/ struct tee_ioctl_buf_data *buf_data)
{
  struct tee_ioctl_open_session_arg *arg;
  struct tee_ioctl_param *params;

  arg = buf_data->buf_ptr;
  params = (struct tee_ioctl_param *)(arg + 1);
  printf("arg->num_params = %d\n", arg->num_params);
  printf("arg->session = %d\n", arg->session);
  arg->session = 2;
  printf("arg->session = %d\n", arg->session);

  uuid_print(arg->uuid);

  return TEEC_SUCCESS;
}

TEEC_Result tee_ioctl_invoke(/*ctx,*/ struct tee_ioctl_buf_data *buf_data)
{
  struct tee_ioctl_invoke_arg *arg;
  struct tee_ioctl_param *params;

  arg = buf_data->buf_ptr;
  params = (struct tee_ioctl_param *)(arg + 1);
  printf("arg->num_params = %d\n", arg->num_params);
  printf("arg->session = %d\n", arg->session);
  printf("params->u.value.a = %d\n", params->u.value.a);
  params->u.value.a++;


  return TEEC_SUCCESS;
}

TEEC_Result tee_ioctl_close_session(/*ctx,*/ struct tee_ioctl_close_session_arg *arg)
{
//  struct tee_ioctl_close_session_arg *arg;

//  arg = buf_data->buf_ptr;
  printf("arg->session = %d\n", arg->session);
  arg->session = 0;

  return TEEC_SUCCESS;
}

