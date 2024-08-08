############################################################################
# arch/riscv32/bl808/src/pine64_ox64/nonsecure/Makefile
#
# Copyright (c) 2024 Samsung Electronics Co., Ltd. All Rights Reserved.
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

CFLAGS += -DDEBUG_PORT=$(subst ",,$(CONFIG_NONSECURE_DEBUG_UART))

OBJDIR = $(TOPDIR)/build/nonsecure$(subst $(TOPDIR),,$(shell pwd))

MTOWER_NS = $(TOPDIR)/mtower_ns$(EXEEXT)

ASRCS_NS = $(CHIP_ASRCS_NS) $(CMN_ASRCS)
AOBJS_NS = $(addprefix $(OBJDIR)/, $(ASRCS_NS:.S=$(OBJEXT)))

CSRCS_NS = $(CHIP_CSRCS_NS) $(CMN_CSRCS)
COBJS_NS = $(addprefix $(OBJDIR)/, $(CSRCS_NS:.c=$(OBJEXT)))

OBJS_NS = $(AOBJS_NS) $(COBJS_NS)

LIBPATHS = -L. -L"$(TOPDIR)/lib"

LIBS =  -llibc -lmm -llhal -lstd -lfreertos -Wl,--no-whole-archive -lc -lm

$(AOBJS_NS): $(OBJDIR)/%$(OBJEXT): %.S
	$(Q) mkdir -p $(OBJDIR)/$(dir $<)
	@echo "AS: $<"
#	$(Q) $(CC) -c $(AFLAGS) -DHeap_Size=0x00000700 -DStack_Size=0x00000B00 $< -o $@
	$(Q) $(CC) -c $(CFLAGS) $< -o $@

$(COBJS_NS): $(OBJDIR)/%$(OBJEXT): %.c
	$(Q) mkdir -p $(OBJDIR)/$(dir $<)
	@echo "CC: $<"
	$(Q) $(CC) -c $(CFLAGS) $< -o $@

mtower_ns$(EXEEXT): $(OBJS_NS) 
	$(Q) $(MAKE) -C ../../ TOPDIR="$(TOPDIR)" liblibc.a
	$(Q) $(MAKE) -C ../../ TOPDIR="$(TOPDIR)" libmm.a
	$(Q) $(MAKE) -C ../../ TOPDIR="$(TOPDIR)" liblhal.a
	$(Q) $(MAKE) -C ../../ TOPDIR="$(TOPDIR)" libstd.a
	$(Q) $(MAKE) -C ../../ TOPDIR="$(TOPDIR)" libfreertos.a
	@echo "LD: mTower_ns$(EXEEXT)"
# $(Q) $(CC) $(CFLAGS) -Wl,--section-start=.text=$(CONFIG_START_ADDRESS_BL33) -Tmetal.freertos.lds $(OBJS_NS) $(LIBPATHS) $(LIBS) -o $(OBJDIR)/bl33.elf
#	$(Q) $(CC) $(CFLAGS) -nostartfiles -Wl,--defsym,__stack_size=0x300 -Wl,--defsym,__heap_size=0x100 -Tmetal.freertos.lds $(OBJS_NS) $(LIBPATHS) $(LIBS) -o $(OBJDIR)/bl33.elf
#	$(Q) $(CC) $(CFLAGS) -nostartfiles -Wl,--defsym,__stack_size=0x300 -Wl,--defsym,__heap_size=0x100 -Tbl808_flash_m0.ld $(OBJS_NS) $(LIBPATHS) $(LIBS) -o $(OBJDIR)/bl33.elf
#	$(Q) $(CC) $(CFLAGS) -nostartfiles -fms-extensions -ffunction-sections -fdata-sections --specs=nano.specs -Tbl808_flash_m0.ld $(OBJS_NS) $(LIBPATHS) $(LIBS) -o $(OBJDIR)/bl33.elf
# -Wl,-Map=$(TOPDIR)/../bl_mcu_sdk/examples/helloworld/build/build_out/helloworld_bl808_m0.map
#	$(Q) $(CC) -Tbl808_flash_m0.ld  -Wl,--cref -Wl,--gc-sections -nostartfiles -fms-extensions -ffunction-sections -fdata-sections --specs=nano.specs -ufw_header -march=rv32imafcpzpsfoperand_xtheade -mabi=ilp32f -mtune=e907 $(OBJS_NS) $(LIBPATHS) $(LIBS) -o $(OBJDIR)/bl33.elf
# $(Q) $(CC) $(CFLAGS) -Tbl808_flash_m0.ld  -Wl,--cref -Wl,--gc-sections -nostartfiles -fms-extensions -ffunction-sections -fdata-sections --specs=nano.specs -ufw_header -march=rv32imafcpzpsfoperand_xtheade -mabi=ilp32f -mtune=e907 $(OBJS_NS) $(LIBPATHS) $(LIBS) -o $(OBJDIR)/bl33.elf
# -Wl,-Map=$(TOPDIR)/../bl_mcu_sdk/examples/helloworld/build/build_out/helloworld_bl808_m0.map	
	$(Q) $(CC) -Tbl808_flash_m0.ld -Wl,--gc-sections -nostartfiles -fms-extensions -ffunction-sections -fdata-sections --specs=nano.specs -ufw_header -march=rv32imafcpzpsfoperand_xtheade -mabi=ilp32f -mtune=e907 $(OBJS_NS) -o $(OBJDIR)/bl33.elf $(LIBPATHS) $(LIBS) 
	$(Q) $(OBJCOPY) -S -O binary $(OBJDIR)/bl33.elf $(OBJDIR)/bl33.bin
	$(Q) $(OBJCOPY) -S -O ihex $(OBJDIR)/bl33.elf $(OBJDIR)/bl33.hex

clean:
	$(Q) rm -f ../../StdDriver/src/*$(OBJEXT) 

distclean: clean

