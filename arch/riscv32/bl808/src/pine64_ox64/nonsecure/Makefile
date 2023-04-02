############################################################################
# arch/cortex-m23/m2351/src/numaker_pfm_m2351/nonsecure/Makefile
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
-include Make.defs

CFLAGS += -I../ -I./	
CFLAGS += -I../../
CFLAGS += -I$(TOPDIR)/arch/riscv32/bl808/src/drivers/lhal/include
CFLAGS += -I$(TOPDIR)/arch/riscv32/bl808/src/drivers/lhal/include/arch
CFLAGS += -I$(TOPDIR)/arch/riscv32/bl808/src/drivers/lhal/include/arch/risc-v/t-head/Core/Include
CFLAGS += -I$(TOPDIR)/arch/riscv32/bl808/src/drivers/lhal/include/arch/risc-v/t-head
CFLAGS += -I$(TOPDIR)/arch/riscv32/bl808/src/drivers/lhal/src/flash
CFLAGS += -I$(TOPDIR)/arch/riscv32/bl808/src/drivers/lhal/config/bl808
CFLAGS += -I$(TOPDIR)/arch/riscv32/bl808/src/drivers/soc/bl808/std/include
CFLAGS += -I$(TOPDIR)/arch/riscv32/bl808/src/drivers/soc/bl808/std/include/hardware

CFLAGS += -I$(TOPDIR)/arch/riscv32/bl808/src/components/mm
CFLAGS += -I$(TOPDIR)/arch/riscv32/bl808/src/components/utils/log

#CFLAGS += -I$(TOPDIR)/include/mtower
# CFLAGS += -I$(TOPDIR)/FreeRTOS-metal/FreeRTOS-Kernel/include
# CFLAGS += -I$(TOPDIR)/FreeRTOS-metal/FreeRTOS-Kernel/portable/GCC/RISC-V
# CFLAGS += -I$(TOPDIR)/FreeRTOS-metal/FreeRTOS-Kernel/portable/GCC/RISC-V/chip_specific_extensions/RV32I_CLINT_no_extensions
#CFLAGS += -I$(TOPDIR)/tee_client/public
#CFLAGS += -I$(TOPDIR)/tee_client/libteec/include/freertos

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

LIBPATHS = -L. -L"$(TOPDIR)/lib" -L"$(TOPDIR)/arch/riscv32/bl808/src/drivers/lhal/src/pka"

# LIBS =  -lm -lc -lnosys -lm2351_StdDriver_ns -lnsc -lFreeRTOS_ns
# LIBS =  -lm -lc -lnosys -lm2351_StdDriver_ns -lnsc -lFreeRTOS_ns
LIBS =  -llibc -lmm -lutils -llhal -lstd -lpka -lfreertos -Wl,--no-whole-archive -lc -lm
# -lFreeRTOS_ns -lgcc
# -lfe310_StdDriver_ns

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
#	$(Q) $(MAKE) -C $(TOPDIR)/FreeRTOS-metal/FreeRTOS-Kernel/ TOPDIR="$(TOPDIR)" libFreeRTOS_ns.a
#	$(Q) $(MAKE) -C ../../freedom-metal/src/ TOPDIR="$(TOPDIR)" libfe310_StdDriver_ns.a
	$(Q) $(MAKE) -C ../../ TOPDIR="$(TOPDIR)" liblibc.a
	$(Q) $(MAKE) -C ../../ TOPDIR="$(TOPDIR)" libmm.a
	$(Q) $(MAKE) -C ../../ TOPDIR="$(TOPDIR)" libutils.a
	$(Q) $(MAKE) -C ../../ TOPDIR="$(TOPDIR)" liblhal.a
	$(Q) $(MAKE) -C ../../ TOPDIR="$(TOPDIR)" libstd.a
	$(Q) $(MAKE) -C ../../ TOPDIR="$(TOPDIR)" libfreertos.a
# $(Q) $(MAKE) -C ../../freedom-metal/src/ TOPDIR="$(TOPDIR)" libfe310_StdDriver_ns.a
	@echo "LD: mTower_ns$(EXEEXT)"
# $(Q) $(CC) $(CFLAGS) -Wl,--section-start=.text=$(CONFIG_START_ADDRESS_BL33) -Tmetal.freertos.lds $(OBJS_NS) $(LIBPATHS) $(LIBS) -o $(OBJDIR)/bl33.elf
#	$(Q) $(CC) $(CFLAGS) -nostartfiles -Wl,--defsym,__stack_size=0x300 -Wl,--defsym,__heap_size=0x100 -Tmetal.freertos.lds $(OBJS_NS) $(LIBPATHS) $(LIBS) -o $(OBJDIR)/bl33.elf
#	$(Q) $(CC) $(CFLAGS) -nostartfiles -Wl,--defsym,__stack_size=0x300 -Wl,--defsym,__heap_size=0x100 -Tbl808_flash_m0.ld $(OBJS_NS) $(LIBPATHS) $(LIBS) -o $(OBJDIR)/bl33.elf
#	$(Q) $(CC) $(CFLAGS) -nostartfiles -fms-extensions -ffunction-sections -fdata-sections --specs=nano.specs -Tbl808_flash_m0.ld $(OBJS_NS) $(LIBPATHS) $(LIBS) -o $(OBJDIR)/bl33.elf
# -Wl,-Map=/home/virtual-box/projects/bl_mcu_sdk/examples/helloworld/build/build_out/helloworld_bl808_m0.map
#	$(Q) $(CC) -Tbl808_flash_m0.ld  -Wl,--cref -Wl,--gc-sections -nostartfiles -fms-extensions -ffunction-sections -fdata-sections --specs=nano.specs -ufw_header -march=rv32imafcpzpsfoperand_xtheade -mabi=ilp32f -mtune=e907 $(OBJS_NS) $(LIBPATHS) $(LIBS) -o $(OBJDIR)/bl33.elf
# $(Q) $(CC) $(CFLAGS) -Tbl808_flash_m0.ld  -Wl,--cref -Wl,--gc-sections -nostartfiles -fms-extensions -ffunction-sections -fdata-sections --specs=nano.specs -ufw_header -march=rv32imafcpzpsfoperand_xtheade -mabi=ilp32f -mtune=e907 $(OBJS_NS) $(LIBPATHS) $(LIBS) -o $(OBJDIR)/bl33.elf
# -Wl,-Map=/home/virtual-box/projects/bl_mcu_sdk/examples/helloworld/build/build_out/helloworld_bl808_m0.map	
	$(Q) $(CC) -Tbl808_flash_m0.ld -Wl,--gc-sections -nostartfiles -fms-extensions -ffunction-sections -fdata-sections --specs=nano.specs -ufw_header -march=rv32imafcpzpsfoperand_xtheade -mabi=ilp32f -mtune=e907 $(OBJS_NS) -o $(OBJDIR)/bl33.elf $(LIBPATHS) $(LIBS) 
	$(Q) $(OBJCOPY) -S -O binary $(OBJDIR)/bl33.elf $(OBJDIR)/bl33.bin
	$(Q) $(OBJCOPY) -S -O ihex $(OBJDIR)/bl33.elf $(OBJDIR)/bl33.hex

clean:
	$(Q) rm -f ../../StdDriver/src/*$(OBJEXT) 

distclean: clean

