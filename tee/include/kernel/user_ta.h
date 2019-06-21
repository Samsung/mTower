/**
 * @file        tee/include/kernel/user_ta.h
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

/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2015, Linaro Limited
 */
#ifndef KERNEL_USER_TA_H
#define KERNEL_USER_TA_H

#include <assert.h>
#include <kernel/tee_ta_manager.h>
#include <user_ta_header.h>
//#include <kernel/thread.h>
//#include <mm/tee_mm.h>
#include <tee_api_types.h>
#include <types_ext.h>
#include <util.h>

#define user_ta_register(...) static const struct user_ta_head __head \
      __used __section("user_ta_head_section") = { __VA_ARGS__ }

TAILQ_HEAD(tee_cryp_state_head, tee_cryp_state);
TAILQ_HEAD(tee_obj_head, tee_obj);
TAILQ_HEAD(tee_storage_enum_head, tee_storage_enum);

struct user_ta_ctx {
  const struct user_ta_head *user_ta;

	uaddr_t entry_func;
	uaddr_t exidx_start;	/* 32-bit TA: exception handling index table */
	size_t exidx_size;
	bool is_32bit;		/* true if 32-bit ta, false if 64-bit ta */
	/* list of sessions opened by this TA */
	struct tee_ta_session_head open_sessions;
	/* List of cryp states created by this TA */
	struct tee_cryp_state_head cryp_states;
	/* List of storage objects opened by this TA */
	struct tee_obj_head objects;
	/* List of storage enumerators opened by this TA */
//	struct tee_storage_enum_head storage_enums;
	struct mobj *mobj_code; /* secure world memory */
	struct mobj *mobj_stack; /* stack */
//	uint32_t load_addr;	/* elf load addr (from TAs address space) */
//	struct tee_mmu_info *mmu;	/* Saved MMU information (ddr only) */
//	void *ta_time_offs;	/* Time reference used by the TA */
//	struct tee_pager_area_head *areas;
#if defined(CFG_SE_API)
	struct tee_se_service *se_service;
#endif
#if defined(CFG_WITH_VFP)
	struct thread_user_vfp_state vfp;
#endif
	struct tee_ta_ctx ctx;

};

static inline bool is_user_ta_ctx(struct tee_ta_ctx *ctx)
{
	return 1; //!!(ctx->flags & TA_FLAG_USER_MODE);
}

static inline struct user_ta_ctx *to_user_ta_ctx(struct tee_ta_ctx *ctx)
{
	assert(is_user_ta_ctx(ctx));
	return container_of(ctx, struct user_ta_ctx, ctx);
}

struct user_ta_store_ops;

#define CFG_WITH_USER_TA
#ifdef CFG_WITH_USER_TA
TEE_Result tee_ta_init_user_ta_session(const TEE_UUID *uuid,
			struct tee_ta_session *s);
TEE_Result tee_ta_register_ta_store(struct user_ta_store_ops *ops);
#else
static inline TEE_Result tee_ta_init_user_ta_session(
			const TEE_UUID *uuid __unused,
			struct tee_ta_session *s __unused)
{
	return TEE_ERROR_ITEM_NOT_FOUND;
}

static inline TEE_Result tee_ta_register_ta_store(
			struct user_ta_store_ops *ops __unused)
{
	return TEE_ERROR_NOT_SUPPORTED;
}
#endif

#endif /*KERNEL_USER_TA_H*/
