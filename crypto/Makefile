############################################################################
# crypto/Makefile
#
# Copyright (c) 2019 Samsung Electronics Co., Ltd. All Rights Reserved.
# Author: Taras Drozdovskyi t.drozdovsky@samsung.com
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
############################################################################

-include $(TOPDIR)/Make.defs
-include $(TOPDIR)/.config

CFLAGS += -I.
CFLAGS += -I./libtomcrypt/include
CFLAGS += -I$(TOPDIR)/tee_client/public
CFLAGS += -I$(TOPDIR)/tee_client/libteec/include/freertos
CFLAGS += -I$(TOPDIR)/tee/lib/libutee/include
CFLAGS += -I$(TOPDIR)/arch/cortex-m23/m2351/src/numaker_pfm_m2351/secure

CFLAGS += -I$(TOPDIR)/tee/lib/libutils/ext/include
CFLAGS += -I$(TOPDIR)/tee/include

ifeq ($(WORLD), secure)
CFLAGS += -mcmse
endif

SRCS  = libtomcrypt/src/tee_ltc_provider.c 
SRCS += libtomcrypt/src/ciphers/aes.c
SRCS += libtomcrypt/src/ciphers/aes_tab.c

SRCS += libtomcrypt/src/misc/crypt/crypt.c
SRCS += libtomcrypt/src/misc/crypt/crypt_cipher_descriptor.c
SRCS += libtomcrypt/src/misc/crypt/crypt_cipher_is_valid.c
SRCS += libtomcrypt/src/misc/crypt/crypt_find_cipher_any.c
SRCS += libtomcrypt/src/misc/crypt/crypt_find_cipher.c
SRCS += libtomcrypt/src/misc/crypt/crypt_find_cipher_id.c
#//srcs-y += crypt_fsa.c
SRCS += libtomcrypt/src/misc/crypt/crypt_register_cipher.c
SRCS += libtomcrypt/src/misc/crypt/crypt_unregister_cipher.c
SRCS += libtomcrypt/src/modes/ctr/ctr_decrypt.c
SRCS += libtomcrypt/src/modes/ctr/ctr_done.c
SRCS += libtomcrypt/src/modes/ctr/ctr_encrypt.c
SRCS += libtomcrypt/src/modes/ctr/ctr_getiv.c
SRCS += libtomcrypt/src/modes/ctr/ctr_setiv.c
SRCS += libtomcrypt/src/modes/ctr/ctr_start.c

SRCS += libtomcrypt/src/modes/ecb/ecb_decrypt.c
SRCS += libtomcrypt/src/modes/ecb/ecb_done.c
SRCS += libtomcrypt/src/modes/ecb/ecb_encrypt.c
SRCS += libtomcrypt/src/modes/ecb/ecb_start.c

SRCS += libtomcrypt/src/misc/crypt/crypt_hash_descriptor.c
SRCS += libtomcrypt/src/misc/crypt/crypt_hash_is_valid.c
SRCS += libtomcrypt/src/misc/crypt/crypt_find_hash_any.c
SRCS += libtomcrypt/src/misc/crypt/crypt_find_hash.c
SRCS += libtomcrypt/src/misc/crypt/crypt_find_hash_id.c
SRCS += libtomcrypt/src/misc/crypt/crypt_find_hash_oid.c
SRCS += libtomcrypt/src/misc/crypt/crypt_register_hash.c
SRCS += libtomcrypt/src/misc/crypt/crypt_unregister_hash.c

SRCS += libtomcrypt/src/mac/hmac/hmac_init.c
SRCS += libtomcrypt/src/mac/hmac/hmac_process.c
SRCS += libtomcrypt/src/mac/hmac/hmac_done.c
#SRCS += libtomcrypt/src/mac/hmac/hmac_memory.c

SRCS += libtomcrypt/src/hashes/sha1.c
SRCS += libtomcrypt/src/hashes/helper/hash_memory.c
SRCS += libtomcrypt/src/misc/zeromem.c

CFLAGS += -DLTC_RIJNDAEL -D_CFG_CRYPTO_WITH_CIPHER -DCFG_CRYPTO_CTR -DLTC_CTR_MODE -DLTC_ECB_MODE -DCFG_CRYPTO_AES

CFLAGS += -D_CFG_CRYPTO_WITH_HASH -DCFG_CRYPTO_SHA1 -DCFG_CRYPTO_HMAC

CFLAGS += -D_CFG_CRYPTO_WITH_MAC -DLTC_HMAC

ifneq ($(WORLD), secure)
CFLAGS += -DDEBUG_PORT=$(subst ",,$(CONFIG_NONSECURE_DEBUG_UART))
OBJDIR = $(TOPDIR)/build/nonsecure$(subst $(TOPDIR),,$(shell pwd))
endif

ifeq ($(WORLD), secure)
CFLAGS += -DDEBUG_PORT=$(subst ",,$(CONFIG_SECURE_DEBUG_UART))
OBJDIR = $(TOPDIR)/build/secure$(subst $(TOPDIR),,$(shell pwd))
endif

OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.c=.o))

current_dir = $(subst $(TOPDIR),,$(shell pwd))

$(OBJDIR)/%.o : %.c
	$(Q) mkdir -p $(OBJDIR)/$(dir $<)
	@echo "CC: $<"
	$(Q) $(CC) $(CFLAGS) -c -o $@ $^

libcrypt_s.a: $(OBJS)
	$(Q) $(AR) $(OBJDIR)/$@ $(OBJS)
	$(Q) cp $(OBJDIR)/$@ $(TOPDIR)/lib/$@

libcrypt_ns.a: $(OBJS)
	$(Q) $(AR) $(OBJDIR)/$@ $(OBJS)
	$(Q) cp $(OBJDIR)/$@ $(TOPDIR)/lib/$@

clean:
	rm -f $(OBJS) libcrypt_s.a

