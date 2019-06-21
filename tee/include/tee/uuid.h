/**
 * @file        tee/include/tee/uuid.h
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
 * Copyright (c) 2016, Linaro Limited
 */

#ifndef __TEE_UUID
#define __TEE_UUID

//#include <optee_msg.h>
#include <tee_api_types.h>
//#include <types_ext.h>

/**
 * tee_uuid_to_octets() - serialize a TEE_UUID into an octet string
 * @dst: pointer to octet string
 * @src: pointer TEE_UUID
 */
void tee_uuid_to_octets(uint8_t *dst, const TEE_UUID *src);

/**
 * tee_uuid_from_octets() - de-serialize an octet string into a TEE_UUID
 * @dst: pointer TEE_UUID
 * @src: pointer to octet string
 */
void tee_uuid_from_octets(TEE_UUID *dst, const uint8_t *src);

void uuid_print(uint8_t *d);

#endif /*__TEE_UUID*/
