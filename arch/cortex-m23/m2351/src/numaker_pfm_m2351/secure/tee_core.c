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

#include <tee_ta_manager.h>

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
//  printf("Secure ioctl: cmd = %x\n", cmd);

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

/* Sessions opened from normal world */
struct tee_ta_session_head tee_open_sessions =
TAILQ_HEAD_INITIALIZER(tee_open_sessions);

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
  TEE_Result res;
  TEE_ErrorOrigin err;
  struct tee_ta_session *sess = NULL;
  struct tee_ta_param *param = NULL;

  struct tee_ioctl_open_session_arg *arg;
  struct tee_ioctl_param *params;
  TEE_UUID uuid;

  arg = buf_data->buf_ptr;
  params = (struct tee_ioctl_param *)(arg + 1);

  uuid_print(arg->uuid);

  tee_uuid_from_octets(&uuid, arg->uuid);
//  tee_ta_context_register(arg->uuid);

  param = (struct tee_ta_param *)(arg + 1);

  res = tee_ta_open_session(&err, &sess, &tee_open_sessions, &uuid, param);

  if (res != TEE_SUCCESS)
    sess = NULL;

  if (sess)
    arg->session = sess;
  else
    arg->session = 0;
  arg->ret = res;
  arg->ret_origin = err;

  return TEEC_SUCCESS;
}

TEEC_Result tee_ioctl_invoke(/*ctx,*/ struct tee_ioctl_buf_data *buf_data)
{
  TEE_ErrorOrigin err;

  struct tee_ioctl_invoke_arg *arg;
  struct tee_ta_session *sess;
  struct tee_ta_param *param = NULL;

  arg = buf_data->buf_ptr;
  param = (struct tee_ta_param *)(arg + 1);

  sess = tee_ta_get_session(arg->session, true, &tee_open_sessions);

  tee_ta_invoke_command(&err, sess, arg->func, param);

  return TEEC_SUCCESS;
}

TEEC_Result tee_ioctl_close_session(/*ctx,*/ struct tee_ioctl_close_session_arg *arg)
{
  struct tee_ta_session *s;

  s = (struct tee_ta_session *)arg->session;

  tee_ta_close_session(s, &tee_open_sessions, NULL /* NSAPP_IDENTITY*/);

  return TEEC_SUCCESS;
}

