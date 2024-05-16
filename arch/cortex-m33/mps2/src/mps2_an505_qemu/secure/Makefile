############################################################################
# arch/cortex-m33/mps2/src/mps2_an505_qemu/secure/Makefile
#
# Copyright (c) 2022 Samsung Electronics Co., Ltd. All Rights Reserved.
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
-include Make.defs

CFLAGS += -I../ -I./

CFLAGS += -I$(TOPDIR)/arch/cortex-m33/mps2/src/Device/ARM/MPS2/Include
CFLAGS += -I$(TOPDIR)/arch/cortex-m33/mps2/src/CMSIS/Driver
CFLAGS += -I$(TOPDIR)/arch/cortex-m33/mps2/src/Driver/Include

CFLAGS += -I$(TOPDIR)/include/mtower
CFLAGS += -I../../CMSIS/Include
CFLAGS += -I../../StdDriver/inc
CFLAGS += -I../../Device/
CFLAGS += -I../../Device/ARM/ARMCM33/Include
CFLAGS += -I../../CMSIS/Include
CFLAGS += -I../common

CFLAGS += -I$(TOPDIR)/tee_client/public
CFLAGS += -I$(TOPDIR)/tee_client/libteec/include/freertos
CFLAGS += -I$(TOPDIR)/tee/lib/libutee/include
CFLAGS += -I$(TOPDIR)/tee/lib/libutils/ext/include
CFLAGS += -I$(TOPDIR)/tee/lib/libutils/isoc/include
CFLAGS += -I$(TOPDIR)/tee/include

ifeq ($(CONFIG_APPS_HELLO_WORLD),y)
CFLAGS += -I$(TOPDIR)/apps/hello_world
endif

ifeq ($(CONFIG_APPS_AES),y)
CFLAGS += -I$(TOPDIR)/apps/aes
endif

ifeq ($(CONFIG_APPS_HOTP),y)
CFLAGS += -I$(TOPDIR)/apps/hotp
endif

ifeq ($(CONFIG_APPS_TEST),y)
CFLAGS += -I$(TOPDIR)/apps/test
endif

TZ_VTOR_TABLE_ADDR = 0x00200000

CFLAGS += -DARMCM33_DSP_FP_TZ \
    -DTZ_VTOR_TABLE_ADDR=$(TZ_VTOR_TABLE_ADDR) \
    -specs=nano.specs -specs=nosys.specs \
    -Wl,--gc-sections \
	-nostartfiles -ffreestanding

SECURE_LINKER_ARGS = \
    -Xlinker --sort-section=alignment \
    -Xlinker --cmse-implib \
    -Xlinker -Map=output.map

CFLAGS += -DIOTKit_ARMv8MML

CFLAGS += -DDEBUG_PORT=$(subst ",,$(CONFIG_SECURE_DEBUG_UART))

OBJDIR = $(TOPDIR)/build/secure$(subst $(TOPDIR),,$(shell pwd))

MTOWER_S = $(TOPDIR)/mtower_s$(EXEEXT)

ASRCS_S = $(CHIP_ASRCS_S) $(CMN_ASRCS)
AOBJS_S = $(addprefix $(OBJDIR)/, $(ASRCS_S:.S=$(OBJEXT)))

CSRCS_S = $(CHIP_CSRCS_S) $(CMN_CSRCS)
COBJS_S = $(addprefix $(OBJDIR)/, $(CSRCS_S:.c=$(OBJEXT)))

OBJS_S = $(AOBJS_S) $(COBJS_S)

LIBPATHS = -L. -L"$(TOPDIR)/lib"

LIBS =  -lm -lc -lnosys -lcrypt_s
#-lStdDriver_s

$(AOBJS_S): $(OBJDIR)/%$(OBJEXT): %.S
	$(Q) mkdir -p $(OBJDIR)/$(dir $<)
	@echo "AS: $<"
	$(Q) $(CC) -c $(AFLAGS) -mcmse $< -o $@
#	$(Q) $(CC) -c $(AFLAGS) -mcmse -DHeap_Size=0x00000700 -DStack_Size=0x00000B00 $< -o $@

$(COBJS_S): $(OBJDIR)/%$(OBJEXT): %.c
	$(Q) mkdir -p $(OBJDIR)/$(dir $<)
	@echo "CC: $<"
	$(Q) $(CC) -c $(CFLAGS) -mcmse $< -o $@

mtower_s$(EXEEXT): $(OBJS_S)
#	$(Q) $(MAKE) -C ../../StdDriver/src/ TOPDIR="$(TOPDIR)" WORLD=secure libStdDriver_s.a
	$(Q) $(MAKE) -C $(TOPDIR)/crypto/ TOPDIR="$(TOPDIR)" WORLD=secure libcrypt_s.a
	@echo "CC: mTower_s$(EXEEXT)"
	$(Q) $(CC) $(CFLAGS) -mcmse -Tsecure.ld $(OBJS_S) $(LIBPATHS) $(LIBS) -Wl,--out-implib=$(OBJDIR)/libnsc$(LIBEXT) -Wl,--cmse-implib -Wl,-Map=output.map -o $(OBJDIR)/bl32.elf
#	$(Q) $(CC) $(CFLAGS) -mcmse -Wl,--section-start=.text=$(CONFIG_START_ADDRESS_BL32) -Tsecure.ld $(OBJS_S) $(LIBPATHS) $(LIBS) -o $(OBJDIR)/bl32.elf
#	$(Q) $(CC) $(CFLAGS) -Wl,--unresolved-symbols=ignore-all -Wl,--section-start=.gnu.sgstubs=0x3f000 -Wl,--out-implib=$(OBJDIR)/libnsc$(LIBEXT) -Wl,--cmse-implib $(OBJS_S) $(LIBPATHS) $(LIBS)
# $(Q) $(CC) $(CFLAGS) -Wl,--section-start=.text=$(CONFIG_START_ADDRESS_BL32) -Wl,--unresolved-symbols=ignore-all -Wl,--out-implib=$(OBJDIR)/libnsc$(LIBEXT) -Wl,--cmse-implib -Tgcc_arm.ld $(OBJS_S) $(LIBPATHS) $(LIBS)
# $(Q) $(CC) $(CFLAGS) -Wl,--out-implib=$(OBJDIR)/libnsc$(LIBEXT) -Wl,--cmse-implib -Tgcc_arm.ld $(OBJS_S) $(LIBPATHS) $(LIBS)
	$(Q) rm -f a.out
	$(Q) $(OBJCOPY) -S -O binary $(OBJDIR)/bl32.elf $(OBJDIR)/bl32.bin
	$(Q) cp -f $(OBJDIR)/libnsc$(LIBEXT) $(TOPDIR)/lib/

clean:
	$(Q) $(MAKE) -C ../../NuBL2 TOPDIR="$(TOPDIR)" clean

distclean: clean

