/*
 * Copyright (c) 2017, Linaro Limited
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

//#include <err.h>
#include <stdio.h>
#include <string.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* To the the UUID (found the the TA's h-file(s)) */
#include <test_ta.h>

#define AES_TEST_BUFFER_SIZE	64 //4096
#define AES_TEST_KEY_SIZE	16

#define DECODE			0
#define ENCODE			1

#define NORMAL  "\033[0m"
#define BLACK   "\033[0;30m1"
#define RED     "\033[0;31m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[0;33m"
#define BLUE    "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN    "\033[0;36m"
#define GRAY    "\033[0;37m"

/* TEE resources */
struct test_ctx {
	TEEC_Context ctx;
	TEEC_Session sess;
};

static int total_test = 0;
static int success = 0;

static void prepare_tee_session(struct test_ctx *ctx)
{
	TEEC_UUID uuid = TA_TEST_UUID;
	uint32_t origin;
	TEEC_Result res;

	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx->ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	/* Open a session with the TA */
	res = TEEC_OpenSession(&ctx->ctx, &ctx->sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, origin);
}

static void terminate_tee_session(struct test_ctx *ctx)
{
	TEEC_CloseSession(&ctx->sess);
	TEEC_FinalizeContext(&ctx->ctx);
}

static void prepare_aes(struct test_ctx *ctx, int encode)
{
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;

	total_test += 7;

	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
					 TEEC_VALUE_INPUT,
					 TEEC_VALUE_INPUT,
					 TEEC_NONE);

	op.params[0].value.a = TA_AES_ALGO_CTR;
	op.params[1].value.a = TA_AES_SIZE_128BIT;
	op.params[2].value.a = encode ? TA_AES_MODE_ENCODE :
					TA_AES_MODE_DECODE;

	res = TEEC_InvokeCommand(&ctx->sess, TA_AES_CMD_PREPARE,
				 &op, &origin);
	if (res != TEEC_SUCCESS) {
		errx(1, "TEEC_InvokeCommand(PREPARE) failed 0x%x origin 0x%x",
			res, origin);
    printf(NORMAL "Check TEE_AllocateOperation " RED "[FAILED]\n" NORMAL);
    printf(NORMAL "Check TEE_Malloc " RED "[FAILED]\n" NORMAL);
    printf(NORMAL "Check TEE_InitRefAttribute " RED "[FAILED]\n" NORMAL);
    printf(NORMAL "Check TEE_AllocateTransientObject " RED "[FAILED]\n" NORMAL);
    printf(NORMAL "Check TEE_PopulateTransientObject " RED "[FAILED]\n" NORMAL);
    printf(NORMAL "Check TEE_SetOperationKey " RED "[FAILED]\n" NORMAL);
	} else {
    printf(NORMAL "Check TEE_AllocateOperation " GREEN "[OK]\n" NORMAL);
    printf(NORMAL "Check TEE_Malloc " GREEN "[OK]\n" NORMAL);
    printf(NORMAL "Check TEE_InitRefAttribute " GREEN "[OK]\n" NORMAL);
    printf(NORMAL "Check TEE_AllocateTransientObject " GREEN "[OK]\n" NORMAL);
    printf(NORMAL "Check TEE_PopulateTransientObject " GREEN "[OK]\n" NORMAL);
    printf(NORMAL "Check TEE_SetOperationKey " GREEN "[OK]\n" NORMAL);
    success += 7;
	}
}

static void set_key(struct test_ctx *ctx, char *key, size_t key_sz)
{
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;

  total_test += 2;

  memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
					 TEEC_NONE, TEEC_NONE, TEEC_NONE);

	op.params[0].tmpref.buffer = key;
	op.params[0].tmpref.size = key_sz;

	res = TEEC_InvokeCommand(&ctx->sess, TA_AES_CMD_SET_KEY,
				 &op, &origin);
	if (res != TEEC_SUCCESS) {
		errx(1, "TEEC_InvokeCommand(SET_KEY) failed 0x%x origin 0x%x",
			res, origin);
    printf(NORMAL "Check TEE_ResetTransientObject " RED "[FAILED]\n" NORMAL);
    printf(NORMAL "Check TEE_ResetOperation " RED "[FAILED]\n" NORMAL);
  } else {
    printf(NORMAL "Check TEE_ResetTransientObject " GREEN "[OK]\n" NORMAL);
    printf(NORMAL "Check TEE_ResetOperation " GREEN "[OK]\n" NORMAL);
    success += 2;
  }

}

static void set_iv(struct test_ctx *ctx, char *iv, size_t iv_sz)
{
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;

  total_test += 1;

  memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
					  TEEC_NONE, TEEC_NONE, TEEC_NONE);
	op.params[0].tmpref.buffer = iv;
	op.params[0].tmpref.size = iv_sz;

	res = TEEC_InvokeCommand(&ctx->sess, TA_AES_CMD_SET_IV,
				 &op, &origin);
	if (res != TEEC_SUCCESS) {
		errx(1, "TEEC_InvokeCommand(SET_IV) failed 0x%x origin 0x%x",
			res, origin);
    printf(NORMAL "Check TEE_CipherInit " RED "[FAILED]\n" NORMAL);
  } else {
    printf(NORMAL "Check TEE_CipherInit " GREEN "[OK]\n" NORMAL);
    success += 1;
  }

}

static void cipher_buffer(struct test_ctx *ctx, char *in, char *out, size_t sz)
{
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;

  total_test += 1;

	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
					 TEEC_MEMREF_TEMP_OUTPUT,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].tmpref.buffer = in;
	op.params[0].tmpref.size = sz;
	op.params[1].tmpref.buffer = out;
	op.params[1].tmpref.size = sz;

	res = TEEC_InvokeCommand(&ctx->sess, TA_AES_CMD_CIPHER,
				 &op, &origin);
	if (res != TEEC_SUCCESS) {
		errx(1, "TEEC_InvokeCommand(CIPHER) failed 0x%x origin 0x%x",
			res, origin);
    printf(NORMAL "Check TEE_CipherUpdate " RED "[FAILED]\n" NORMAL);
  } else {
    printf(NORMAL "Check TEE_CipherUpdate " GREEN "[OK]\n" NORMAL);
    success += 1;
  }
}

int check_TEEC_InitializeContext(struct test_ctx *ctx)
{
  TEEC_Result res;
  int result = 0;

  total_test++;
  /* Initialize a context connecting us to the TEE */
  res = TEEC_InitializeContext(NULL, NULL);
  if (res != TEEC_SUCCESS) {
    errx(1, "TEEC_InitializeContext failed with code 0x%x\n", res);
    result++;
  }

  total_test++;
  /* Initialize a context connecting us to the TEE */
  res = TEEC_InitializeContext(NULL, &ctx->ctx);
  if (res != TEEC_SUCCESS) {
    errx(1, "TEEC_InitializeContext failed with code 0x%x\n", res);
  } else {
    result++;
  }

  if (result == 2) {
    printf(NORMAL "Check TEEC_InitializeContext " GREEN "[OK]\n" NORMAL);
  } else {
    printf(NORMAL "Check TEEC_InitializeContext " RED "[FAILED]\n" NORMAL);
  }
  return result;
}

int check_TEEC_OpenSession(struct test_ctx *ctx)
{
  TEEC_UUID uuid = TA_TEST_UUID;
  uint32_t origin;
  TEEC_Result res;
  int result = 0;

  total_test++;
  /* Initialize a context connecting us to the TEE */
  res = TEEC_InitializeContext(NULL, &ctx->ctx);
  if (res != TEEC_SUCCESS) {
    errx(1, "TEEC_InitializeContext failed with code 0x%x\n", res);
  } else {
    result++;
  }

  total_test++;
  /* Open a session with the TA */
  res = TEEC_OpenSession(NULL, &ctx->sess, &uuid,
             TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
  if (res != TEEC_SUCCESS) {
    errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x\n",
      res, origin);
    result++;
  }

  total_test++;
  /* Open a session with the TA */
  res = TEEC_OpenSession(&ctx->ctx, NULL, &uuid,
             TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
  if (res != TEEC_SUCCESS) {
    errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x\n",
      res, origin);
    result++;
  }

  total_test++;
  uuid.timeLow--;
  /* Open a session with the TA */
  res = TEEC_OpenSession(&ctx->ctx, &ctx->sess, &uuid,
             TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
  if (res != TEEC_SUCCESS) {
    errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x\n",
      res, origin);
    result++;
  }

  total_test++;
  uuid.timeLow++;
  /* Open a session with the TA */
  res = TEEC_OpenSession(&ctx->ctx, &ctx->sess, &uuid,
             TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
  if (res != TEEC_SUCCESS)
    errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x\n",
      res, origin);
  else
    result++;

  if (result == 5) {
    printf(NORMAL "Check TEEC_OpenSession " GREEN "[OK]\n" NORMAL);
    printf(NORMAL "Check TEE_Malloc " GREEN "[OK]\n" NORMAL);
    printf(NORMAL "Check TA_OpenSessionEntryPoint " GREEN "[OK]\n" NORMAL);
  } else {
    printf(NORMAL "Check TEEC_OpenSession " RED "[FAILED]\n" NORMAL);
    printf(NORMAL "Check TEE_Malloc " RED "[FAILED]\n" NORMAL);
    printf(NORMAL "Check TA_OpenSessionEntryPoint " RED "[FAILED]\n" NORMAL);
  }

  return result;
}

int check_TEEC_InvokeCommand(struct test_ctx *ctx)
{
  TEEC_Operation op;
  uint32_t origin;
  TEEC_Result res;
  int result = 0;

  memset(&op, 0, sizeof(op));
  /*
   * Prepare the argument. Pass a value in the first parameter,
   * the remaining three parameters are unused.
   */
  op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
           TEEC_NONE, TEEC_NONE);
  op.params[0].value.a = 42;

  total_test++;
  res = TEEC_InvokeCommand(&ctx->sess, TA_AES_CMD_CIPHER + 100,
         &op, &origin);
  if (res != TEEC_SUCCESS) {
    errx(1, "TEEC_InvokeCommand failed 0x%x origin 0x%x\n",
      res, origin);
    result++;
  }

  total_test++;
  res = TEEC_InvokeCommand(NULL, TA_AES_CMD_CIPHER,
         &op, &origin);
  if (res != TEEC_SUCCESS) {
    errx(1, "TEEC_InvokeCommand failed 0x%x origin 0x%x\n",
      res, origin);
    result++;
  }

  total_test++;
  res = TEEC_InvokeCommand(&ctx->sess, TA_AES_CMD_CIPHER,
         NULL, &origin);
  if (res != TEEC_SUCCESS) {
    errx(1, "TEEC_InvokeCommand failed 0x%x origin 0x%x\n",
      res, origin);
    result++;
  }

  total_test++;
  res = TEEC_InvokeCommand(&ctx->sess, TA_AES_CMD_CIPHER,
         &op, &origin);
  if (res != TEEC_SUCCESS) {
    errx(1, "TEEC_InvokeCommand failed 0x%x origin 0x%x\n",
      res, origin);
    result++;
  }

  total_test++;
  memset(&op, 0, sizeof(op));
  op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
           TEEC_VALUE_INPUT,
           TEEC_VALUE_INPUT,
           TEEC_NONE);

  op.params[0].value.a = TA_AES_ALGO_CTR;
  op.params[1].value.a = TA_AES_SIZE_128BIT;
  op.params[2].value.a = TA_AES_MODE_ENCODE;

  res = TEEC_InvokeCommand(&ctx->sess, TA_AES_CMD_PREPARE,
         &op, &origin);
  if (res != TEEC_SUCCESS)
    errx(1, "TEEC_InvokeCommand failed 0x%x origin 0x%x\n",
      res, origin);
  else
    result++;

  if (result == 5) {
    printf(NORMAL "Check TEEC_InvokeCommand " GREEN "[OK]\n" NORMAL);
    printf(NORMAL "Check TA_InvokeCommandEntryPoint " GREEN "[OK]\n" NORMAL);
  } else {
    printf(NORMAL "Check TEEC_InvokeCommand " RED "[FAILED]\n" NORMAL);
    printf(NORMAL "Check TA_InvokeCommandEntryPoint " RED "[FAILED]\n" NORMAL);
  }

  return result;
}

int check_TEEC_AllocateSharedMemory(struct test_ctx *ctx)
{
  TEEC_SharedMemory commsSM;
  TEEC_Result res;
  int result = 0;

  total_test++;
  commsSM.size = 10;
  commsSM.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;

  /* Use TEE Client API to allocate the underlying memory buffer. */
  res = TEEC_AllocateSharedMemory(NULL, &commsSM);
  if (res != TEEC_SUCCESS) {
    errx(1, "TEEC_AllocateSharedMemory failed 0x%x \n", res);
    result++;
  }

  total_test++;
  commsSM.size = 10;
  commsSM.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;

  /* Use TEE Client API to allocate the underlying memory buffer. */
  res = TEEC_AllocateSharedMemory(&ctx->ctx, NULL);
  if (res != TEEC_SUCCESS) {
    errx(1, "TEEC_AllocateSharedMemory failed 0x%x \n", res);
    result++;
  }

  total_test++;
  commsSM.size = 100;
  commsSM.flags &= ~(TEEC_MEM_INPUT | TEEC_MEM_OUTPUT);

  /* Use TEE Client API to allocate the underlying memory buffer. */
  res = TEEC_AllocateSharedMemory(&ctx->ctx, &commsSM);
  if (res != TEEC_SUCCESS) {
    errx(1, "TEEC_AllocateSharedMemory failed 0x%x \n", res);
    result++;
  }

//  total_test++;
//  commsSM.size = 18000;
//  commsSM.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
//
//  /* Use TEE Client API to allocate the underlying memory buffer. */
//  res = TEEC_AllocateSharedMemory(&ctx->ctx, &commsSM);
//  if (res != TEEC_SUCCESS) {
//    errx(1, "TEEC_AllocateSharedMemory failed 0x%x \n", res);
//    result++;
//  }

  total_test++;
  commsSM.size = 20;
  commsSM.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;

  /* Use TEE Client API to allocate the underlying memory buffer. */
  res = TEEC_AllocateSharedMemory(&ctx->ctx, &commsSM);
  if (res != TEEC_SUCCESS)
    errx(1, "TEEC_AllocateSharedMemory failed 0x%x \n", res);
  else
    result++;

  if (result == 4) {
    printf(NORMAL "Check TEEC_AllocateSharedMemory " GREEN "[OK]\n" NORMAL);
  } else {
    printf(NORMAL "Check TEEC_AllocateSharedMemory " RED "[FAILED]\n" NORMAL);
  }

  total_test++;
  /* Use TEE Client API to free the memory buffer. */
  TEEC_ReleaseSharedMemory(NULL);
  if((commsSM.buffer_allocated == true) && (commsSM.buffer != NULL))
    result++;

//  if((commsSM.buffer_allocated == true) && (commsSM.buffer != NULL))
//    printf("commsSM.buffer is allocated\n");

  total_test++;
  /* Use TEE Client API to free the memory buffer. */
  TEEC_ReleaseSharedMemory(&commsSM);
  if((commsSM.buffer_allocated == false) && (commsSM.buffer == NULL))
    result++;

  if (result == 6) {
    printf(NORMAL "Check TEEC_ReleaseSharedMemory " GREEN "[OK]\n" NORMAL);
  } else {
    printf(NORMAL "Check TEEC_ReleaseSharedMemory " RED "[FAILED]\n" NORMAL);
  }

  return result;
}

int check_TEEC_CloseSession(TEEC_Session *session)
{
  TEEC_Session tmp_session;
  int result = 0;

  total_test++;
  /* Use TEE Client API to close session. */
  TEEC_CloseSession(NULL);
  if(session->session_id)
    result++;

  total_test++;
  /* Use TEE Client API to close session. */
  tmp_session.session_id = session->session_id + 4;
  TEEC_CloseSession(&tmp_session);
  if(session->session_id)
    result++;

  total_test++;
  /* Use TEE Client API to close session. */
  TEEC_CloseSession(session);
  if(session->session_id == 0)
    result++;

  if (result == 3) {
    printf(NORMAL "Check TEEC_CloseSession " GREEN "[OK]\n" NORMAL);
    printf(NORMAL "Check TA_CloseSessionEntryPoint " GREEN "[OK]\n" NORMAL);
    printf(NORMAL "Check TEE_Free " GREEN "[OK]\n" NORMAL);
  } else {
    printf(NORMAL "Check TEEC_CloseSession " RED "[FAILED]\n" NORMAL);
    printf(NORMAL "Check TA_CloseSessionEntryPoint " RED "[FAILED]\n" NORMAL);
    printf(NORMAL "Check TEE_Free " RED "[FAILED]\n" NORMAL);
  }

  return result;
}

int check_TEEC_FinalizeContext(struct test_ctx *ctx)
{
  int result = 0;

  total_test++;
  /* Use TEE Client API to FinalizeContext. */
  TEEC_FinalizeContext(NULL);
  if(ctx->ctx.fd != -1)
    result++;

  total_test++;
  /* Use TEE Client API to FinalizeContext. */
  TEEC_FinalizeContext(&ctx->ctx);
  if(ctx->ctx.fd == -1)
    result++;

  if (result == 2) {
    printf(NORMAL "Check TEEC_FinalizeContext " GREEN "[OK]\n" NORMAL);
  } else {
    printf(NORMAL "Check TEEC_FinalizeContext " RED "[FAILED]\n" NORMAL);
  }

  return result;
}

void tee_tests(void)
{
	success = 0;

	total_test = 0;

  struct test_ctx ctx;
	char key[AES_TEST_KEY_SIZE];
	char iv[AES_TEST_KEY_SIZE];
	char clear[AES_TEST_BUFFER_SIZE];
	char ciph[AES_TEST_BUFFER_SIZE];

	success += check_TEEC_InitializeContext(&ctx);
	success += check_TEEC_OpenSession(&ctx);
	success += check_TEEC_InvokeCommand(&ctx);
	success += check_TEEC_AllocateSharedMemory(&ctx);
  success += check_TEEC_CloseSession(&ctx.sess);
  success += check_TEEC_FinalizeContext(&ctx);

	printf("Prepare session with the TA\n");
	prepare_tee_session(&ctx);

	printf("Prepare encode operation\n");
	prepare_aes(&ctx, ENCODE);


	printf("Load key in TA\n");
	memset(key, 0xa5, sizeof(key)); /* Load some dummy value */
	set_key(&ctx, key, AES_TEST_KEY_SIZE);

	printf("Reset ciphering operation in TA (provides the initial vector)\n");
	memset(iv, 0, sizeof(iv)); /* Load some dummy value */
	set_iv(&ctx, iv, AES_TEST_KEY_SIZE);

	printf("Encode buffer from TA\n");
	memset(clear, 0x5a, sizeof(clear)); /* Load some dummy value */
  memset(ciph, 0x0, sizeof(clear)); /* Load some dummy value */
	cipher_buffer(&ctx, clear, ciph, AES_TEST_BUFFER_SIZE);

	terminate_tee_session(&ctx);
  printf("%d tests successfully completed from %d total tests\n", success, total_test);

}
