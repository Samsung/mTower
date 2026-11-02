// SPDX-License-Identifier: BSD-2-Clause
/*
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

//#include <types_ext.h>
//#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <arm.h>
//#include <assert.h>
//#include <kernel/mutex.h>
//#include <kernel/panic.h>
#include <kernel/pseudo_ta.h>
#include <kernel/tee_common.h>
//#include nkernel/tee_misc.h>
#include <kernel/tee_ta_manager.h>
//#include <kernel/tee_time.h>
//#include <kernel/thread.h>
#include <kernel/user_ta.h>
//#include <mm/core_mmu.h>
//#include <mm/core_memprot.h>
//#include <mm/mobj.h>
//#include <mm/tee_mmu.h>
//#include <tee/tee_svc_cryp.h>
//#include <tee/tee_obj.h>
//#include <tee/tee_svc_storage.h>
//#include <tee_api_types.h>
#include <trace.h>
//#include <utee_types.h>
//#include <util.h>

//#include "tee_types.h"
//#include "tee_client_api.h"
//#include "tee.h"

#include "printf.h"

struct tee_ta_session *current_session;
/* This mutex protects the critical section in tee_ta_init_session */
//struct mutex tee_ta_mutex = MUTEX_INITIALIZER;
struct tee_ta_ctx_head tee_ctxes = TAILQ_HEAD_INITIALIZER(tee_ctxes);

//#ifndef CFG_CONCURRENT_SINGLE_INSTANCE_TA
//static struct condvar tee_ta_cv = CONDVAR_INITIALIZER;
//static int tee_ta_single_instance_thread = THREAD_ID_INVALID;
//static size_t tee_ta_single_instance_count;
//#endif

//#ifdef CFG_CONCURRENT_SINGLE_INSTANCE_TA
//static void lock_single_instance(void)
//{
//}
//
//static void unlock_single_instance(void)
//{
//}
//
//static bool has_single_instance_lock(void)
//{
//	return false;
//}
//#else
//static void lock_single_instance(void)
//{
//	/* Requires tee_ta_mutex to be held */
//	if (tee_ta_single_instance_thread != thread_get_id()) {
//		/* Wait until the single-instance lock is available. */
//		while (tee_ta_single_instance_thread != THREAD_ID_INVALID)
//			condvar_wait(&tee_ta_cv, &tee_ta_mutex);
//
//		tee_ta_single_instance_thread = thread_get_id();
//		assert(tee_ta_single_instance_count == 0);
//	}
//
//	tee_ta_single_instance_count++;
//}
//
//static void unlock_single_instance(void)
//{
//	/* Requires tee_ta_mutex to be held */
//	assert(tee_ta_single_instance_thread == thread_get_id());
//	assert(tee_ta_single_instance_count > 0);
//
//	tee_ta_single_instance_count--;
//	if (tee_ta_single_instance_count == 0) {
//		tee_ta_single_instance_thread = THREAD_ID_INVALID;
//		condvar_signal(&tee_ta_cv);
//	}
//}
//
//static bool has_single_instance_lock(void)
//{
//	/* Requires tee_ta_mutex to be held */
//	return tee_ta_single_instance_thread == thread_get_id();
//}
//#endif
//
//static bool tee_ta_try_set_busy(struct tee_ta_ctx *ctx)
//{
//	bool rc = true;
//
//	if (ctx->flags & TA_FLAG_CONCURRENT)
//		return true;
//
//	mutex_lock(&tee_ta_mutex);
//
//	if (ctx->flags & TA_FLAG_SINGLE_INSTANCE)
//		lock_single_instance();
//
//	if (has_single_instance_lock()) {
//		if (ctx->busy) {
//			/*
//			 * We're holding the single-instance lock and the
//			 * TA is busy, as waiting now would only cause a
//			 * dead-lock, we release the lock and return false.
//			 */
//			rc = false;
//			if (ctx->flags & TA_FLAG_SINGLE_INSTANCE)
//				unlock_single_instance();
//		}
//	} else {
//		/*
//		 * We're not holding the single-instance lock, we're free to
//		 * wait for the TA to become available.
//		 */
//		while (ctx->busy)
//			condvar_wait(&ctx->busy_cv, &tee_ta_mutex);
//	}
//
//	/* Either it's already true or we should set it to true */
//	ctx->busy = true;
//
//	mutex_unlock(&tee_ta_mutex);
//	return rc;
//}
//
//static void tee_ta_set_busy(struct tee_ta_ctx *ctx)
//{
//	if (!tee_ta_try_set_busy(ctx))
//		panic();
//}
//
//static void tee_ta_clear_busy(struct tee_ta_ctx *ctx)
//{
//	if (ctx->flags & TA_FLAG_CONCURRENT)
//		return;
//
//	mutex_lock(&tee_ta_mutex);
//
//	assert(ctx->busy);
//	ctx->busy = false;
//	condvar_signal(&ctx->busy_cv);
//
//	if (ctx->flags & TA_FLAG_SINGLE_INSTANCE)
//		unlock_single_instance();
//
//	mutex_unlock(&tee_ta_mutex);
//}
//
//static void dec_session_ref_count(struct tee_ta_session *s)
//{
//	assert(s->ref_count > 0);
//	s->ref_count--;
//	if (s->ref_count == 1)
//		condvar_signal(&s->refc_cv);
//}
//
//void tee_ta_put_session(struct tee_ta_session *s)
//{
//	mutex_lock(&tee_ta_mutex);
//
//	if (s->lock_thread == thread_get_id()) {
//		s->lock_thread = THREAD_ID_INVALID;
//		condvar_signal(&s->lock_cv);
//	}
//	dec_session_ref_count(s);
//
//	mutex_unlock(&tee_ta_mutex);
//}

static struct tee_ta_session *find_session(uint32_t id,
			struct tee_ta_session_head *open_sessions)
{
	struct tee_ta_session *s;

	TAILQ_FOREACH(s, open_sessions, link) {
    if ((uint32_t)s == id) {
//      if ((vaddr_t)s == id)
			return s;}
	}
	return NULL;
}

struct tee_ta_session *tee_ta_get_session(uint32_t id, bool exclusive,
			struct tee_ta_session_head *open_sessions)
{
	struct tee_ta_session *s;
  (void) exclusive;
//	mutex_lock(&tee_ta_mutex);
//
//	while (true) {
		s = find_session(id, open_sessions);
//		if (!s)
//			break;
//		if (s->unlink) {
//			s = NULL;
//			break;
//		}
//		s->ref_count++;
//		if (!exclusive)
//			break;
//
//		assert(s->lock_thread != thread_get_id());
//
//		while (s->lock_thread != THREAD_ID_INVALID && !s->unlink)
//			condvar_wait(&s->lock_cv, &tee_ta_mutex);
//
//		if (s->unlink) {
//			dec_session_ref_count(s);
//			s = NULL;
//			break;
//		}
//
//		s->lock_thread = thread_get_id();
//		break;
//	}
//
//	mutex_unlock(&tee_ta_mutex);
	return s;
}

static void tee_ta_unlink_session(struct tee_ta_session *s,
			struct tee_ta_session_head *open_sessions)
{
//	mutex_lock(&tee_ta_mutex);
//
//	assert(s->ref_count >= 1);
//	assert(s->lock_thread == thread_get_id());
//	assert(!s->unlink);
//
//	s->unlink = true;
//	condvar_broadcast(&s->lock_cv);
//
//	while (s->ref_count != 1)
//		condvar_wait(&s->refc_cv, &tee_ta_mutex);
//
	TAILQ_REMOVE(open_sessions, s, link);

//	mutex_unlock(&tee_ta_mutex);
}

/*
 * tee_ta_context_find - Find TA in session list based on a UUID (input)
 * Returns a pointer to the session
 */
//static struct tee_ta_ctx *tee_ta_context_find(const TEE_UUID *uuid)
//{
//  struct tee_ta_ctx *ctx;
//
//  TAILQ_FOREACH(ctx, &tee_ctxes, link)
//  {
//    if (memcmp(&ctx->uuid, uuid, sizeof(TEE_UUID)) == 0) {
//      printf("\nUUID is founded\n");
//      return ctx;
//   }
//  }
//
//  return NULL;
//}

/*
 * Threat model and client-identity policy (see docs/client-identity-acl.md)
 * ------------------------------------------------------------------------
 * mTower treats the entire Normal World as a single trust domain: the TEE
 * itself provides no inter-client separation between non-secure clients.
 * To nevertheless restrict which login methods may reach which Trusted
 * Applications, a compile-time Access Control List is enforced on the
 * session-open path. It is intentionally hardcoded in the TEE core
 * (Normal World cannot add, remove or alter entries at runtime).
 */
struct ta_acl_entry {
	const TEE_UUID ta_uuid;
	const uint32_t allowed_login_methods;
};

/*
 * ta_acl - hardcoded per-TA access control list.
 *
 * Each entry maps a TA UUID to the set of client login methods that are
 * permitted to open a session to it. A login method is allowed when the
 * corresponding TEE_IOCTL_LOGIN_* bit (value 1 << login) is set in
 * allowed_login_methods. TEE_LOGIN_PUBLIC is 0x0, so PUBLIC access is
 * implied by any entry that does not set TEE_ACL_DENY_PUBLIC.
 *
 * !!! IMPORTANT !!! This table is a HARD CODED restriction. To grant a
 * client access to a TA, add or edit an entry here and rebuild the secure
 * world. Entries are matched by full 16-byte UUID comparison; TAs not
 * listed are DENIED for ALL non-secure clients (default-deny).
 *
 * Current policy:
 *   - hello_world, AES, HOTP: only TEEC_LOGIN_APPLICATION (the intended
 *     demo/test client applications). LOGIN_PUBLIC is rejected.
 *   - test TA: public (CI/test suite convenience).
 */
#define TEE_ACL_LOGIN_PUBLIC_BIT		(1u << 0)	/* login == 0 */
#define TEE_ACL_LOGIN_USER_BIT		(1u << 1)
#define TEE_ACL_LOGIN_GROUP_BIT		(1u << 2)
#define TEE_ACL_LOGIN_APPLICATION_BIT	(1u << 4)
#define TEE_ACL_LOGIN_USER_APP_BIT	(1u << 5)
#define TEE_ACL_LOGIN_GROUP_APP_BIT	(1u << 6)

/* TEE_LOGIN_USER_APPLICATION / TEE_LOGIN_GROUP_APPLICATION are not defined
 * in the in-tree tee_api_defines.h; use their GP-defined numeric values. */
#ifndef TEE_LOGIN_USER_APPLICATION
#define TEE_LOGIN_USER_APPLICATION	0x00000005
#endif
#ifndef TEE_LOGIN_GROUP_APPLICATION
#define TEE_LOGIN_GROUP_APPLICATION	0x00000006
#endif

static const struct ta_acl_entry ta_acl[] = {
	/* TA_HELLO_WORLD_UUID { 8aaaf200-2450-11e4-abe2-0002a5d5c51b } */
	{
		{ 0x8aaaf200, 0x2450, 0x11e4,
			{ 0xab, 0xe2, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b } },
		TEE_ACL_LOGIN_APPLICATION_BIT,
	},
	/* TA_AES_UUID { 5dbac793-f574-4871-8ad3-0433 1ec17f24 } */
	{
		{ 0x5dbac793, 0xf574, 0x4871,
			{ 0x8a, 0xd3, 0x04, 0x33, 0x1e, 0xc1, 0x7f, 0x24 } },
		TEE_ACL_LOGIN_APPLICATION_BIT,
	},
	/* TA_HOTP_UUID { 484d4143-2d53-4841-3120-4a6f636b6542 } */
	{
		{ 0x484d4143, 0x2d53, 0x4841,
			{ 0x31, 0x20, 0x4a, 0x6f, 0x63, 0x6b, 0x65, 0x42 } },
		TEE_ACL_LOGIN_APPLICATION_BIT,
	},
	/* TA_TEST_UUID { 5dbac744-f574-4871-8ad3-0433 1ec17f24 } */
	{
		{ 0x5dbac744, 0xf574, 0x4871,
			{ 0x8a, 0xd3, 0x04, 0x33, 0x1e, 0xc1, 0x7f, 0x24 } },
		TEE_ACL_LOGIN_PUBLIC_BIT,
	},
};

/*
 * ta_acl_allowed - is a session-open by login method 'login' to the TA
 * identified by 'uuid' permitted?
 *
 * Note: the login value is the GP TEEC_LOGIN_* / TEE_IOCTL_LOGIN_* constant
 * (0 == PUBLIC, 1 == USER, ...). TEE_LOGIN_PUBLIC (0) maps to
 * TEE_ACL_LOGIN_PUBLIC_BIT (1u << 0), so the bit index is the login value
 * for all GP-defined login methods (0, 1, 2, 4, 5, 6).
 *
 * Returns true if allowed, false if denied (default-deny for unlisted TAs).
 */
static bool ta_acl_allowed(const TEE_UUID *ta_uuid, uint32_t login)
{
	size_t i;
	uint32_t bit;

	/* Unknown login methods are always denied. */
	if (login != TEE_LOGIN_PUBLIC && login != TEE_LOGIN_USER &&
	    login != TEE_LOGIN_GROUP && login != TEE_LOGIN_APPLICATION &&
	    login != TEE_LOGIN_USER_APPLICATION &&
	    login != TEE_LOGIN_GROUP_APPLICATION)
		return false;

	bit = 1u << login;

	for (i = 0; i < sizeof(ta_acl) / sizeof(ta_acl[0]); i++) {
		if (memcmp(&ta_acl[i].ta_uuid, ta_uuid, sizeof(TEE_UUID)) == 0)
			return (ta_acl[i].allowed_login_methods & bit) != 0;
	}

	/* Default-deny: TA not in the ACL table. */
	return false;
}

//* check if requester (client ID) matches session initial client */
static TEE_Result check_client(struct tee_ta_session *s, const TEE_Identity *id)
{
	if (id == KERN_IDENTITY)
		return TEE_SUCCESS;

	if (id == NSAPP_IDENTITY) {
		if (s->clnt_id.login == TEE_LOGIN_TRUSTED_APP) {
			DMSG("nsec tries to hijack TA session");
			return TEE_ERROR_ACCESS_DENIED;
		}
		return TEE_SUCCESS;
	}

	if (memcmp(&s->clnt_id, id, sizeof(TEE_Identity)) != 0) {
		DMSG("client id mismatch");
		return TEE_ERROR_ACCESS_DENIED;
	}
	return TEE_SUCCESS;
}

///*
// * Check if invocation parameters matches TA properties
// *
// * @s - current session handle
// * @param - already identified memory references hold a valid 'mobj'.
// *
// * Policy:
// * - All TAs can access 'non-secure' shared memory.
// * - All TAs can access TEE private memory (seccpy)
// * - Only SDP flagged TAs can accept SDP memory references.
// */
//#ifndef CFG_SECURE_DATA_PATH
//static bool check_params(struct tee_ta_session *sess __unused,
//			 struct tee_ta_param *param __unused)
//{
//	/*
//	 * When CFG_SECURE_DATA_PATH is not enabled, SDP memory references
//	 * are rejected at OP-TEE core entry. Hence here all TAs have same
//	 * permissions regarding memory reference parameters.
//	 */
//	return true;
//}
//#else
//static bool check_params(struct tee_ta_session *sess,
//			 struct tee_ta_param *param)
//{
//	int n;
//
//	/*
//	 * When CFG_SECURE_DATA_PATH is enabled, OP-TEE entry allows SHM and
//	 * SDP memory references. Only TAs flagged SDP can access SDP memory.
//	 */
//	if (sess->ctx->flags & TA_FLAG_SECURE_DATA_PATH)
//		return true;
//
//	for (n = 0; n < TEE_NUM_PARAMS; n++) {
//		uint32_t param_type = TEE_PARAM_TYPE_GET(param->types, n);
//		struct param_mem *mem = &param->u[n].mem;
//
//		if (param_type != TEE_PARAM_TYPE_MEMREF_INPUT &&
//		    param_type != TEE_PARAM_TYPE_MEMREF_OUTPUT &&
//		    param_type != TEE_PARAM_TYPE_MEMREF_INOUT)
//			continue;
//		if (!mem->size)
//			continue;
//		if (mobj_is_sdp_mem(mem->mobj))
//			return false;
//	}
//	return true;
//}
//#endif
//
//static void set_invoke_timeout(struct tee_ta_session *sess,
//				      uint32_t cancel_req_to)
//{
//	TEE_Time current_time;
//	TEE_Time cancel_time;
//
//	if (cancel_req_to == TEE_TIMEOUT_INFINITE)
//		goto infinite;
//
//	if (tee_time_get_sys_time(&current_time) != TEE_SUCCESS)
//		goto infinite;
//
//	if (ADD_OVERFLOW(current_time.seconds, cancel_req_to / 1000,
//			 &cancel_time.seconds))
//		goto infinite;
//
//	cancel_time.millis = current_time.millis + cancel_req_to % 1000;
//	if (cancel_time.millis > 1000) {
//		if (ADD_OVERFLOW(current_time.seconds, 1,
//				 &cancel_time.seconds))
//			goto infinite;
//
//		cancel_time.seconds++;
//		cancel_time.millis -= 1000;
//	}
//
//	sess->cancel_time = cancel_time;
//	return;
//
//infinite:
//	sess->cancel_time.seconds = UINT32_MAX;
//	sess->cancel_time.millis = UINT32_MAX;
//}
//
/*-----------------------------------------------------------------------------
 * Close a Trusted Application and free available resources
 *---------------------------------------------------------------------------*/
TEE_Result tee_ta_close_session(struct tee_ta_session *csess,
				struct tee_ta_session_head *open_sessions,
				const TEE_Identity *clnt_id)
{
	struct tee_ta_session *sess;
	struct tee_ta_ctx *ctx;

	if (!csess)
		return TEE_ERROR_ITEM_NOT_FOUND;

	sess = tee_ta_get_session((uint32_t)csess, true, open_sessions);

	if (!sess) {
		return TEE_ERROR_ITEM_NOT_FOUND;
	}

	/* Check that the caller is authorized to close this session */
	if (check_client(sess, clnt_id) != TEE_SUCCESS) {
		return TEE_ERROR_ACCESS_DENIED;
	}

	ctx = sess->ctx;
	DMSG("Destroy session");

	ctx->ops->enter_close_session(sess);

	tee_ta_unlink_session(sess, open_sessions);
	TEE_Free(sess);

	DMSG("Destroy TA ctx");
	TAILQ_REMOVE(&tee_ctxes, ctx, link);
	ctx->ops->destroy(ctx);

	return TEE_SUCCESS;
}

//static TEE_Result tee_ta_init_session_with_context(struct tee_ta_ctx *ctx,
//			struct tee_ta_session *s)
//{
//	/*
//	 * If TA isn't single instance it should be loaded as new
//	 * instance instead of doing anything with this instance.
//	 * So tell the caller that we didn't find the TA it the
//	 * caller will load a new instance.
//	 */
//	if ((ctx->flags & TA_FLAG_SINGLE_INSTANCE) == 0)
//		return TEE_ERROR_ITEM_NOT_FOUND;
//
//	/*
//	 * The TA is single instance, if it isn't multi session we
//	 * can't create another session unless its reference is zero
//	 */
//	if (!(ctx->flags & TA_FLAG_MULTI_SESSION) && ctx->ref_count)
//		return TEE_ERROR_BUSY;
//
//	DMSG("Re-open TA %pUl", (void *)&ctx->uuid);
//
//	ctx->ref_count++;
//	s->ctx = ctx;
//	return TEE_SUCCESS;
//}


static TEE_Result tee_ta_init_session(TEE_ErrorOrigin *err,
				struct tee_ta_session_head *open_sessions,
				const TEE_UUID *uuid,
				struct tee_ta_session **sess)
{
	TEE_Result res;
//	struct tee_ta_ctx *ctx;
//  struct tee_ta_session *s = calloc(1, sizeof(struct tee_ta_session));

// TEE_Malloc(sizeof(struct pseudo_ta_ctx), TEE_MALLOC_FILL_ZERO);
//   struct tee_ta_session *s = malloc(sizeof(struct tee_ta_session));
  struct tee_ta_session *s = TEE_Malloc(sizeof(struct tee_ta_session), TEE_MALLOC_FILL_ZERO);
  *err = TEE_ORIGIN_TEE;
  if (!s)
    return TEE_ERROR_OUT_OF_MEMORY;

  memset(s,0,sizeof(struct tee_ta_session));

//	s->cancel_mask = true;
//	condvar_init(&s->refc_cv);
//	condvar_init(&s->lock_cv);
//	s->lock_thread = THREAD_ID_INVALID;
//	s->ref_count = 1;
//
//
//	/*
//	 * We take the global TA mutex here and hold it while doing
//	 * RPC to load the TA. This big critical section should be broken
//	 * down into smaller pieces.
//	 */
//
//
//	mutex_lock(&tee_ta_mutex);
	TAILQ_INSERT_TAIL(open_sessions, s, link);

	/* Look for already loaded TA */
//	ctx = tee_ta_context_find(uuid);
//	if (ctx) {
//		res = tee_ta_init_session_with_context(ctx, s);
//		if (res == TEE_SUCCESS || res != TEE_ERROR_ITEM_NOT_FOUND)
//			goto out;
//	}

	/* Look for static TA */
	res = tee_ta_init_pseudo_ta_session(uuid, s);
	if (res == TEE_SUCCESS || res != TEE_ERROR_ITEM_NOT_FOUND)
		goto out;

	/* Look for user TA */
	res = tee_ta_init_user_ta_session(uuid, s);

out:
	if (res == TEE_SUCCESS) {
		*sess = s;
	} else {
		TAILQ_REMOVE(open_sessions, s, link);
		TEE_Free(s);
	}
//	mutex_unlock(&tee_ta_mutex);
	return res;
}

TEE_Result tee_ta_open_session(TEE_ErrorOrigin *err,
			       struct tee_ta_session **sess,
			       struct tee_ta_session_head *open_sessions,
			       const TEE_UUID *uuid,
			       const TEE_Identity *clnt_id,
			       uint32_t cancel_req_to,
			       struct tee_ta_param *param)
{
	TEE_Result res;
	struct tee_ta_session *s = NULL;
	struct tee_ta_ctx *ctx;
	(void) cancel_req_to;

	/*
	 * Non-secure clients are subject to the hardcoded per-TA ACL
	 * (ta_acl[], see docs/client-identity-acl.md): a session may only
	 * be opened when the login method the client presented is listed
	 * for the target TA. Default-deny for TAs not present in the table.
	 * Teecore-internal callers (KERN_IDENTITY) and TA-to-TA requests
	 * (a real TEE_Identity with TEE_LOGIN_TRUSTED_APP) are not subject
	 * to this table.
	 */
	if (clnt_id == NSAPP_IDENTITY &&
	    !ta_acl_allowed(uuid, TEE_LOGIN_PUBLIC)) {
		DMSG("ACL: TEEC_LOGIN_PUBLIC denied for TA %pUl",
		     (void *)uuid);
		*err = TEE_ORIGIN_TEE;
		return TEE_ERROR_ACCESS_DENIED;
	} else if (clnt_id != NSAPP_IDENTITY && clnt_id != KERN_IDENTITY &&
		   clnt_id->login != TEE_LOGIN_TRUSTED_APP &&
		   !ta_acl_allowed(uuid, clnt_id->login)) {
		DMSG("ACL: login method %u denied for TA %pUl",
		     clnt_id->login, (void *)uuid);
		*err = TEE_ORIGIN_TEE;
		return TEE_ERROR_ACCESS_DENIED;
	}

	res = tee_ta_init_session(err, open_sessions, uuid, &s);
	if (res != TEE_SUCCESS) {
		DMSG("init session failed 0x%x", res);
		return res;
	}

	ctx = s->ctx;

	/*
	 * Save identity of the owner of the session.
	 *
	 * clnt_id may be a sentinel instead of a real TEE_Identity pointer and
	 * must never be dereferenced in those cases:
	 *  - NSAPP_IDENTITY (NULL): the caller is a Normal-World client. The
	 *    Normal World is treated as a single trust domain (no inter-client
	 *    separation is provided by the TEE), so every non-secure client
	 *    shares one anonymous TEE_LOGIN_PUBLIC identity.
	 *  - KERN_IDENTITY ((TEE_Identity *)-1): a teecore-internal request.
	 *    Kernel-owned sessions must not be hijackable from the Normal
	 *    World, so tag them TEE_LOGIN_TRUSTED_APP; check_client() then
	 *    rejects NSAPP_IDENTITY access to them ("nsec tries to hijack TA
	 *    session").
	 */
	if (clnt_id == NSAPP_IDENTITY) {
		memset(&s->clnt_id, 0, sizeof(s->clnt_id));
		s->clnt_id.login = TEE_LOGIN_PUBLIC;
	} else if (clnt_id == KERN_IDENTITY) {
		memset(&s->clnt_id, 0, sizeof(s->clnt_id));
		s->clnt_id.login = TEE_LOGIN_TRUSTED_APP;
	} else {
		s->clnt_id = *clnt_id;
	}

	*sess = s;

	res = ctx->ops->enter_open_session(s, param, err);

	if (res != TEE_SUCCESS)
		tee_ta_close_session(s, open_sessions, KERN_IDENTITY);

	*err = TEE_ORIGIN_TRUSTED_APP;

	if (res != TEE_SUCCESS)
		DMSG("Failed. Return error 0x%x", res);

	return res;
}

TEE_Result tee_ta_invoke_command(TEE_ErrorOrigin *err,
				 struct tee_ta_session *sess,
				 const TEE_Identity *clnt_id,
				 uint32_t cancel_req_to,
				 uint32_t cmd,
				 struct tee_ta_param *param)
{
	TEE_Result res;
	(void) cancel_req_to;

	/*
	 * Defense in depth: never dereference a NULL session. Callers (e.g. the
	 * Non-Secure ioctl veneer) obtain sess from tee_ta_get_session(), which
	 * returns NULL for an unknown session id; without this guard the line
	 * below would fault in the secure world.
	 */
	if (!sess)
		return TEE_ERROR_BAD_PARAMETERS;

	/* Check that the caller is authorized to use this session */
	if (check_client(sess, clnt_id) != TEE_SUCCESS)
		return TEE_ERROR_ACCESS_DENIED;

	res = sess->ctx->ops->enter_invoke_cmd(sess, cmd, param, err);

	return res;
}

//TEE_Result tee_ta_cancel_command(TEE_ErrorOrigin *err,
//				 struct tee_ta_session *sess,
//				 const TEE_Identity *clnt_id)
//{
//	*err = TEE_ORIGIN_TEE;
//
//	if (check_client(sess, clnt_id) != TEE_SUCCESS)
//		return TEE_ERROR_BAD_PARAMETERS; /* intentional generic error */
//
//	sess->cancel = true;
//	return TEE_SUCCESS;
//}
//
//bool tee_ta_session_is_cancelled(struct tee_ta_session *s, TEE_Time *curr_time)
//{
//	TEE_Time current_time;
//
//	if (s->cancel_mask)
//		return false;
//
//	if (s->cancel)
//		return true;
//
//	if (s->cancel_time.seconds == UINT32_MAX)
//		return false;
//
//	if (curr_time != NULL)
//		current_time = *curr_time;
//	else if (tee_time_get_sys_time(&current_time) != TEE_SUCCESS)
//		return false;
//
//	if (current_time.seconds > s->cancel_time.seconds ||
//	    (current_time.seconds == s->cancel_time.seconds &&
//	     current_time.millis >= s->cancel_time.millis)) {
//		return true;
//	}
//
//	return false;
//}
//
//static void update_current_ctx(struct thread_specific_data *tsd)
//{
//	struct tee_ta_ctx *ctx = NULL;
//	struct tee_ta_session *s = TAILQ_FIRST(&tsd->sess_stack);
//
//	if (s) {
//		if (is_pseudo_ta_ctx(s->ctx))
//			s = TAILQ_NEXT(s, link_tsd);
//
//		if (s)
//			ctx = s->ctx;
//	}
//
//	if (tsd->ctx != ctx)
//		tee_mmu_set_ctx(ctx);
//	/*
//	 * If ctx->mmu == NULL we must not have user mapping active,
//	 * if ctx->mmu != NULL we must have user mapping active.
//	 */
//	if (((ctx && is_user_ta_ctx(ctx) ?
//			to_user_ta_ctx(ctx)->mmu : NULL) == NULL) ==
//					core_mmu_user_mapping_is_active())
//		panic("unexpected active mapping");
//}
//
void tee_ta_push_current_session(struct tee_ta_session *sess)
{
//	struct thread_specific_data *tsd = thread_get_tsd();
//
//	TAILQ_INSERT_HEAD(&tsd->sess_stack, sess, link_tsd);
//	update_current_ctx(tsd);

  current_session = sess;

}

struct tee_ta_session *tee_ta_pop_current_session(void)
{
//	struct thread_specific_data *tsd = thread_get_tsd();
//	struct tee_ta_session *s = TAILQ_FIRST(&tsd->sess_stack);
//
//	if (s) {
//		TAILQ_REMOVE(&tsd->sess_stack, s, link_tsd);
//		update_current_ctx(tsd);
//	}
  return current_session;
//	return s;
}

TEE_Result tee_ta_get_current_session(struct tee_ta_session **sess)
{
//	struct tee_ta_session *s = TAILQ_FIRST(&thread_get_tsd()->sess_stack);

//	if (!s)
//		return TEE_ERROR_BAD_STATE;
//	*sess = s;
	*sess = current_session;
	return TEE_SUCCESS;
}

//struct tee_ta_session *tee_ta_get_calling_session(void)
//{
//	struct tee_ta_session *s = TAILQ_FIRST(&thread_get_tsd()->sess_stack);
//
//	if (s)
//		s = TAILQ_NEXT(s, link_tsd);
//	return s;
//}
//
//TEE_Result tee_ta_get_client_id(TEE_Identity *id)
//{
//	TEE_Result res;
//	struct tee_ta_session *sess;
//
//	res = tee_ta_get_current_session(&sess);
//	if (res != TEE_SUCCESS)
//		return res;
//
//	if (id == NULL)
//		return TEE_ERROR_BAD_PARAMETERS;
//
//	*id = sess->clnt_id;
//	return TEE_SUCCESS;
//}
//
///*
// * dump_state - Display TA state as an error log.
// */
//static void dump_state(struct tee_ta_ctx *ctx)
//{
//	struct tee_ta_session *s = NULL;
//	bool active __maybe_unused;
//
//	active = ((tee_ta_get_current_session(&s) == TEE_SUCCESS) &&
//		  s && s->ctx == ctx);
//
//	EMSG_RAW("Status of TA %pUl (%p) %s", (void *)&ctx->uuid, (void *)ctx,
//		active ? "(active)" : "");
//	ctx->ops->dump_state(ctx);
//}
//
//void tee_ta_dump_current(void)
//{
//	struct tee_ta_session *s = NULL;
//
//	if (tee_ta_get_current_session(&s) != TEE_SUCCESS) {
//		EMSG("no valid session found, cannot log TA status");
//		return;
//	}
//
//	dump_state(s->ctx);
//}
//
//#if defined(CFG_TA_GPROF_SUPPORT)
//void tee_ta_gprof_sample_pc(vaddr_t pc)
//{
//	struct tee_ta_session *s;
//	struct sample_buf *sbuf;
//	size_t idx;
//
//	if (tee_ta_get_current_session(&s) != TEE_SUCCESS)
//		return;
//	sbuf = s->sbuf;
//	if (!sbuf || !sbuf->enabled)
//		return; /* PC sampling is not enabled */
//
//	idx = (((uint64_t)pc - sbuf->offset)/2 * sbuf->scale)/65536;
//	if (idx < sbuf->nsamples)
//		sbuf->samples[idx]++;
//	sbuf->count++;
//}
//
///*
// * Update user-mode CPU time for the current session
// * @suspend: true if session is being suspended (leaving user mode), false if
// * it is resumed (entering user mode)
// */
//static void tee_ta_update_session_utime(bool suspend)
//{
//	struct tee_ta_session *s;
//	struct sample_buf *sbuf;
//	uint64_t now;
//
//	if (tee_ta_get_current_session(&s) != TEE_SUCCESS)
//		return;
//	sbuf = s->sbuf;
//	if (!sbuf)
//		return;
//	now = read_cntpct();
//	if (suspend) {
//		assert(sbuf->usr_entered);
//		sbuf->usr += now - sbuf->usr_entered;
//		sbuf->usr_entered = 0;
//	} else {
//		assert(!sbuf->usr_entered);
//		if (!now)
//			now++; /* 0 is reserved */
//		sbuf->usr_entered = now;
//	}
//}
//
//void tee_ta_update_session_utime_suspend(void)
//{
//	tee_ta_update_session_utime(true);
//}
//
//void tee_ta_update_session_utime_resume(void)
//{
//	tee_ta_update_session_utime(false);
//}
//#endif
