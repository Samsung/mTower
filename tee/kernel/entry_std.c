// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2015-2016, Linaro Limited
 * All rights reserved.
 * Copyright (c) 2014, STMicroelectronics International N.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* Included Files. */
#include <arm_cmse.h>
#include <stdio.h>
#include <string.h>

#include "printf.h"

// #include "M2351.h"
// #include "partition_M2351.h"

/* GP TEE client API */
#include "tee_types.h"
#include "tee_client_api.h"
#include "tee.h"
#include "tee/uuid.h"

#include <kernel/tee_ta_manager.h>

/* Pre-processor Definitions. */


/* Private Types. */
/* Any types, enums, structures or unions used by the file are defined here. */
/* typedef for NonSecure callback functions */
typedef __attribute__((cmse_nonsecure_call)) int32_t (*NonSecure_funcptr)(uint32_t);
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
TEEC_Result tee_ioctl_close_session(/*ctx,*/ struct tee_ioctl_buf_data *buf_data);

/*
 * nsec_check - verify that [p, p+len) lies entirely in Non-Secure memory.
 *
 * All pointers and lengths reaching this file originate in the Non-Secure
 * world (the caller of the cmse_nonsecure_entry ioctl() below). Before the
 * secure world dereferences any of them it must confirm the whole range is
 * attributed Non-Secure, otherwise a malicious Non-Secure caller could point
 * into secure RAM and have the secure world read/write it on its behalf.
 *
 * cmse_check_address_range() consults the SAU/IDAU for the given range and
 * returns NULL if any part is not Non-Secure or if the range crosses an
 * attribution boundary / overflows. Only CMSE_NONSECURE (AU attribution) is
 * requested: the security-relevant property is "this is Non-Secure memory".
 * CMSE_MPU_READ/READWRITE are intentionally NOT requested because the
 * Non-Secure MPU is not configured in mTower, so a permission check could
 * spuriously reject valid buffers. The 'write' argument is kept for a possible
 * future tightening if a Non-Secure MPU is ever set up.
 *
 * Returns p on success, NULL on failure. A zero-length range is trivially OK
 * (nothing is dereferenced).
 */
static void *nsec_check(void *p, size_t len, int write)
{
  (void)write;

  if (len == 0)
    return p;

  return cmse_check_address_range(p, len, CMSE_NONSECURE);
}

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
__attribute__((cmse_nonsecure_entry))
int32_t ioctl(uint32_t cmd, struct tee_ioctl_buf_data *buf_data)
{
//  printf("Secure ioctl: cmd = %x\n", cmd);

  /* buf_data is itself a Non-Secure pointer: validate before any access. */
  if (!nsec_check(buf_data, sizeof(*buf_data), 0))
    return TEEC_ERROR_ACCESS_DENIED;

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

#define OPTEE_MSG_ATTR_TYPE_NONE    0x0
#define OPTEE_MSG_ATTR_TYPE_VALUE_INPUT   0x1
#define OPTEE_MSG_ATTR_TYPE_VALUE_OUTPUT  0x2
#define OPTEE_MSG_ATTR_TYPE_VALUE_INOUT   0x3
#define OPTEE_MSG_ATTR_TYPE_RMEM_INPUT    0x5
#define OPTEE_MSG_ATTR_TYPE_RMEM_OUTPUT   0x6
#define OPTEE_MSG_ATTR_TYPE_RMEM_INOUT    0x7
#define OPTEE_MSG_ATTR_TYPE_TMEM_INPUT    0x9
#define OPTEE_MSG_ATTR_TYPE_TMEM_OUTPUT   0xa
#define OPTEE_MSG_ATTR_TYPE_TMEM_INOUT    0xb

#define OPTEE_MSG_ATTR_TYPE_MASK    0x000000ff //GENMASK_32(7, 0)

/**
 * struct optee_msg_param_tmem - temporary memory reference parameter
 * @buf_ptr:  Address of the buffer
 * @size: Size of the buffer
 * @shm_ref:  Temporary shared memory reference, pointer to a struct tee_shm
 *
 * Secure and normal world communicates pointers as physical address
 * instead of the virtual address. This is because secure and normal world
 * have completely independent memory mapping. Normal world can even have a
 * hypervisor which need to translate the guest physical address (AKA IPA
 * in ARM documentation) to a real physical address before passing the
 * structure to secure world.
 */
struct optee_msg_param_tmem {
  uint32_t buf_ptr;
  uint32_t size;
  uint32_t shm_ref;
};

/**
 * struct optee_msg_param_rmem - registered memory reference parameter
 * @offs: Offset into shared memory reference
 * @size: Size of the buffer
 * @shm_ref:  Shared memory reference, pointer to a struct tee_shm
 */
struct optee_msg_param_rmem {
  uint32_t offs;
  uint32_t size;
  uint32_t shm_ref;
};

/**
 * struct optee_msg_param_value - values
 * @a: first value
 * @b: second value
 * @c: third value
 */
struct optee_msg_param_value {
  uint32_t a;
  uint32_t b;
//  uint64_t c;
};

/**
 * struct optee_msg_param - parameter
 * @attr: attributes
 * @memref: a memory reference
 * @value: a value
 *
 * @attr & OPTEE_MSG_ATTR_TYPE_MASK indicates if tmem, rmem or value is used in
 * the union. OPTEE_MSG_ATTR_TYPE_VALUE_* indicates value,
 * OPTEE_MSG_ATTR_TYPE_TMEM_* indicates tmem and
 * OPTEE_MSG_ATTR_TYPE_RMEM_* indicates rmem.
 * OPTEE_MSG_ATTR_TYPE_NONE indicates that none of the members are used.
 */
struct optee_msg_param {
  uint32_t attr;
  union {
    struct optee_msg_param_tmem tmem;
    struct optee_msg_param_rmem rmem;
    struct optee_msg_param_value value;
  } u;
};

/**
 * struct optee_msg_arg - call argument
 * @cmd: Command, one of OPTEE_MSG_CMD_* or OPTEE_MSG_RPC_CMD_*
 * @func: Trusted Application function, specific to the Trusted Application,
 *       used if cmd == OPTEE_MSG_CMD_INVOKE_COMMAND
 * @session: In parameter for all OPTEE_MSG_CMD_* except
 *       OPTEE_MSG_CMD_OPEN_SESSION where it's an output parameter instead
 * @cancel_id: Cancellation id, a unique value to identify this request
 * @ret: return value
 * @ret_origin: origin of the return value
 * @num_params: number of parameters supplied to the OS Command
 * @params: the parameters supplied to the OS Command
 *
 * All normal calls to Trusted OS uses this struct. If cmd requires further
 * information than what these fields hold it can be passed as a parameter
 * tagged as meta (setting the OPTEE_MSG_ATTR_META bit in corresponding
 * attrs field). All parameters tagged as meta have to come first.
 *
 * Temp memref parameters can be fragmented if supported by the Trusted OS
 * (when optee_smc.h is bearer of this protocol this is indicated with
 * OPTEE_SMC_SEC_CAP_UNREGISTERED_SHM). If a logical memref parameter is
 * fragmented then all but the last fragment have the
 * OPTEE_MSG_ATTR_FRAGMENT bit set in attrs. Even if a memref is fragmented
 * it will still be presented as a single logical memref to the Trusted
 * Application.
 */
struct optee_msg_arg {
//  uint32_t cmd;
  uint32_t func;
  uint32_t session;
  uint32_t cancel_id;
//  uint32_t pad;
  uint32_t ret;
  uint32_t ret_origin;
  uint32_t num_params;

  /* num_params tells the actual number of element in params */
  struct optee_msg_param params[4];
};

static TEE_Result copy_in_params(const struct optee_msg_param *params,
         uint32_t num_params,
         struct tee_ta_param *ta_param,
         uint32_t *saved_attr)
{
//  TEE_Result res;
  size_t n;
  uint8_t pt[TEE_NUM_PARAMS];

  if (num_params > TEE_NUM_PARAMS)
    return TEE_ERROR_BAD_PARAMETERS;

  memset(ta_param, 0, sizeof(*ta_param));

  for (n = 0; n < num_params; n++) {
    uint32_t attr;
    saved_attr[n] = params[n].attr;

//    if (saved_attr[n] & OPTEE_MSG_ATTR_META)
//      return TEE_ERROR_BAD_PARAMETERS;

    attr = saved_attr[n] & OPTEE_MSG_ATTR_TYPE_MASK;
    switch (attr) {
    case OPTEE_MSG_ATTR_TYPE_NONE:
      pt[n] = TEE_PARAM_TYPE_NONE;
      memset(&ta_param->u[n], 0, sizeof(ta_param->u[n]));
      break;
    case OPTEE_MSG_ATTR_TYPE_VALUE_INPUT:
    case OPTEE_MSG_ATTR_TYPE_VALUE_OUTPUT:
    case OPTEE_MSG_ATTR_TYPE_VALUE_INOUT:
      pt[n] = TEE_PARAM_TYPE_VALUE_INPUT + attr -
        OPTEE_MSG_ATTR_TYPE_VALUE_INPUT;
      ta_param->u[n].val.a = params[n].u.value.a;
      ta_param->u[n].val.b = params[n].u.value.b;
      break;
    case OPTEE_MSG_ATTR_TYPE_TMEM_INPUT:
    case OPTEE_MSG_ATTR_TYPE_TMEM_OUTPUT:
    case OPTEE_MSG_ATTR_TYPE_TMEM_INOUT:
      pt[n] = TEE_PARAM_TYPE_MEMREF_INPUT + attr -
        OPTEE_MSG_ATTR_TYPE_TMEM_INPUT;
      {
        /* Non-Secure caller supplies buf_ptr/size: the whole buffer must be
         * Non-Secure before it is handed to the TA. INPUT needs read access,
         * OUTPUT/INOUT need write access. */
        void *buf = (void *)params[n].u.tmem.buf_ptr;
        uint32_t sz = params[n].u.tmem.size;
        int wr = (attr != OPTEE_MSG_ATTR_TYPE_TMEM_INPUT);

        if (!nsec_check(buf, sz, wr))
          return TEE_ERROR_ACCESS_DENIED;
        ta_param->u[n].mem.buffer = buf;
        ta_param->u[n].mem.size = sz;
      }
//      res = assign_mobj_to_param_mem(params[n].u.tmem.buf_ptr,
//                   params[n].u.tmem.size,
//                   saved_attr[n],
//                   params[n].u.tmem.shm_ref,
//                   &ta_param->u[n].mem);
//      if (res != TEE_SUCCESS)
//        return res;
      break;
    case OPTEE_MSG_ATTR_TYPE_RMEM_INPUT:
    case OPTEE_MSG_ATTR_TYPE_RMEM_OUTPUT:
    case OPTEE_MSG_ATTR_TYPE_RMEM_INOUT:
      pt[n] = TEE_PARAM_TYPE_MEMREF_INPUT + attr -
        OPTEE_MSG_ATTR_TYPE_RMEM_INPUT;
      {
        /* Same attribution check as the tmem case above. */
        void *buf = (void *)params[n].u.rmem.offs;
        uint32_t sz = params[n].u.rmem.size;
        int wr = (attr != OPTEE_MSG_ATTR_TYPE_RMEM_INPUT);

        if (!nsec_check(buf, sz, wr))
          return TEE_ERROR_ACCESS_DENIED;
        ta_param->u[n].mem.buffer = buf;
        ta_param->u[n].mem.size = sz;
      }
      //      printf("\n~~~~~RMEM params[%d].u.tmem.size = %d;\n",n,params[n].u.tmem.size);
//      mem->offs = param->u.rmem.offs;
//      mem->size = param->u.rmem.size;

//      res = set_rmem_param(params + n, &ta_param->u[n].mem);
//      if (res != TEE_SUCCESS)
//        return res;
      break;
    default:
      return TEE_ERROR_BAD_PARAMETERS;
    }
  }

  ta_param->types = TEE_PARAM_TYPES(pt[0], pt[1], pt[2], pt[3]);

  return TEE_SUCCESS;
}

static void copy_out_param(struct tee_ta_param *ta_param, uint32_t num_params,
         struct optee_msg_param *params, uint32_t *saved_attr)
{
  size_t n;

  for (n = 0; n < num_params; n++) {
    switch (TEE_PARAM_TYPE_GET(ta_param->types, n)) {
    case TEE_PARAM_TYPE_MEMREF_OUTPUT:
    case TEE_PARAM_TYPE_MEMREF_INOUT:
      switch (saved_attr[n] & OPTEE_MSG_ATTR_TYPE_MASK) {
      case OPTEE_MSG_ATTR_TYPE_TMEM_OUTPUT:
      case OPTEE_MSG_ATTR_TYPE_TMEM_INOUT:
        params[n].u.tmem.size = ta_param->u[n].mem.size;
        break;
      case OPTEE_MSG_ATTR_TYPE_RMEM_OUTPUT:
      case OPTEE_MSG_ATTR_TYPE_RMEM_INOUT:
        params[n].u.rmem.size = ta_param->u[n].mem.size;
        break;
      default:
        break;
      }
      break;
    case TEE_PARAM_TYPE_VALUE_OUTPUT:
    case TEE_PARAM_TYPE_VALUE_INOUT:
      params[n].u.value.a = ta_param->u[n].val.a;
      params[n].u.value.b = ta_param->u[n].val.b;
      break;
    default:
      break;
    }
  }
}

/*
 * validate_param_memrefs - attribution-check the memref buffers inside a
 * Non-Secure-supplied tee_ta_param.
 *
 * The open-session path passes the tee_ta_param that trails the arg header
 * (arg + 1) straight to the TA WITHOUT going through copy_in_params(), so its
 * memref buffer pointers are raw Non-Secure input. Validating the container is
 * Non-Secure resident does NOT validate the pointers stored inside it, so each
 * memref buffer/size range must be checked here before the TA dereferences it
 * (mirrors the per-memref check copy_in_params() does on the invoke path).
 *
 * Returns TEEC_SUCCESS if every memref lies in Non-Secure memory, otherwise
 * TEEC_ERROR_ACCESS_DENIED.
 */
static TEEC_Result validate_param_memrefs(const struct tee_ta_param *p)
{
  size_t n;

  for (n = 0; n < TEE_NUM_PARAMS; n++) {
    int wr;

    switch (TEE_PARAM_TYPE_GET(p->types, n)) {
    case TEE_PARAM_TYPE_MEMREF_INPUT:
      wr = 0;
      break;
    case TEE_PARAM_TYPE_MEMREF_OUTPUT:
    case TEE_PARAM_TYPE_MEMREF_INOUT:
      wr = 1;
      break;
    default:
      continue;
    }

    if (!nsec_check(p->u[n].mem.buffer, p->u[n].mem.size, wr))
      return TEEC_ERROR_ACCESS_DENIED;
  }

  return TEEC_SUCCESS;
}

TEEC_Result tee_ioctl_open_session(/*ctx,*/ struct tee_ioctl_buf_data *buf_data)
{
  TEE_Result res;
  TEE_ErrorOrigin err;
  struct tee_ta_session *sess = NULL;
  struct tee_ta_param *param = NULL;
  struct tee_ta_param param_copy;

  struct tee_ioctl_open_session_arg *arg;
  //struct tee_ioctl_param *params;
  TEE_UUID uuid;

  /*
   * Snapshot buf_ptr/buf_len once (they must not be re-fetched from Non-Secure
   * memory between validation and use). The buffer must cover the header AND
   * the trailing tee_ta_param that is dereferenced at (arg + 1) below. Validate
   * the whole range (read/write - session/ret/ret_origin are written back)
   * before touching any field. On failure, return without accessing it.
   */
  uint32_t ptr = buf_data->buf_ptr;
  uint32_t len = buf_data->buf_len;

  if (len < sizeof(struct tee_ioctl_open_session_arg) +
            sizeof(struct tee_ta_param) ||
      !nsec_check((void *)ptr, len, 1))
    return TEEC_ERROR_ACCESS_DENIED;

  arg = (struct tee_ioctl_open_session_arg *)ptr;
  //params = (struct tee_ioctl_param *)(arg + 1);

  //uuid_print(arg->uuid);

  tee_uuid_from_octets(&uuid, arg->uuid);
//  tee_ta_context_register(arg->uuid);

  param = (struct tee_ta_param *)(arg + 1);

  /*
   * Snapshot the Non-Secure tee_ta_param into a secure local before validating
   * and using it. This param is passed straight through to the TA, which
   * re-reads types / u[n].mem.buffer several times; validating it in place
   * would leave a double-fetch/TOCTOU window - a Non-Secure interrupt can
   * preempt secure execution and rewrite the buffer between the check and the
   * use. Validate and use the copy; write TA output params back afterwards.
   * (The invoke path is already protected: copy_in_params copies into a secure
   * local.)
   */
  param_copy = *param;

  if (validate_param_memrefs(&param_copy) != TEEC_SUCCESS)
    return TEEC_ERROR_ACCESS_DENIED;

  res = tee_ta_open_session(&err, &sess, &tee_open_sessions, &uuid, &param_copy);

  /*
   * Propagate TA output params back to the validated Non-Secure buffer:
   * pseudo TAs write value params into param->u[n].val via update_out_param;
   * user TAs mutate param->u in place. Both now land in param_copy, so copy it
   * back. *param lies in the already-validated Non-Secure range.
   */
  *param = param_copy;

  if (res != TEE_SUCCESS)
    sess = NULL;

  if (sess)
    arg->session = (uint32_t)sess;
  else
    arg->session = 0;
  arg->ret = res;
  arg->ret_origin = err;

  return TEEC_SUCCESS;
}

TEEC_Result tee_ioctl_invoke(/*ctx,*/ struct tee_ioctl_buf_data *buf_data)
{
  TEE_ErrorOrigin err;
  TEE_Result res;

//  struct tee_ioctl_invoke_arg *arg;
  struct optee_msg_arg *arg;
  struct tee_ta_session *sess;
  struct tee_ta_param param;

  uint32_t saved_attr[TEE_NUM_PARAMS];

  /*
   * Snapshot buf_ptr/buf_len once, then validate that the whole optee_msg_arg
   * (header + inline params[4]) is Non-Secure before reading any field or
   * writing arg->ret back.
   */
  uint32_t ptr = buf_data->buf_ptr;
  uint32_t len = buf_data->buf_len;
  uint32_t num_params;
  uint32_t session;
  uint32_t func;

  if (len < sizeof(struct optee_msg_arg) ||
      !nsec_check((void *)ptr, len, 1))
    return TEEC_ERROR_ACCESS_DENIED;

  arg = (struct optee_msg_arg *)ptr;

  /*
   * Snapshot the scalars once: num_params in particular must not be re-fetched
   * from Non-Secure memory in copy_out_param after copy_in_params bounded it,
   * or a preempting NS interrupt could grow it past TEE_NUM_PARAMS and drive an
   * out-of-bounds walk of the secure-stack arrays.
   */
  num_params = arg->num_params;
  session = arg->session;
  func = arg->func;

  sess = tee_ta_get_session(session, true, &tee_open_sessions);

  res = copy_in_params(arg->params, num_params, &param, saved_attr);
  if (res != TEE_SUCCESS)
    goto out;

  if (!sess) {
    /*
     * A Non-Secure caller can supply an invalid/zero/stale session id.
     * find_session() then returns NULL and, since the guards in
     * tee_ta_invoke_command() are disabled, the secure world would
     * dereference NULL (sess->ctx->...) -> secure-world crash/DoS.
     * Reject before invoking. Placed after copy_in_params so a malformed
     * request still gets the more specific error from parameter validation.
     */
    res = TEEC_ERROR_BAD_PARAMETERS;
    goto out;
  }

  res = tee_ta_invoke_command(&err, sess, arg->func, &param);

  copy_out_param(&param, num_params, arg->params, saved_attr);

out:
  arg->ret = res;
  //arg->ret_origin = err_orig;
  return res;
}

TEEC_Result tee_ioctl_close_session(/*ctx,*/ struct tee_ioctl_buf_data *buf_data)
{
  TEE_Result res;
  struct tee_ta_session *sess;
  struct tee_ioctl_close_session_arg *arg;

  /* Snapshot buf_ptr/buf_len once, validate, then use the snapshot. */
  uint32_t ptr = buf_data->buf_ptr;
  uint32_t len = buf_data->buf_len;

  if (len < sizeof(struct tee_ioctl_close_session_arg) ||
      !nsec_check((void *)ptr, len, 1))
    return TEEC_ERROR_ACCESS_DENIED;

  arg = (struct tee_ioctl_close_session_arg *)ptr;

  sess = (struct tee_ta_session *)arg->session;

  res = tee_ta_close_session(sess, &tee_open_sessions, NULL /* NSAPP_IDENTITY*/);

//  arg->ret = res;
//  arg->ret_origin = TEE_ORIGIN_TEE;

  return res;
}

