############################################################################
# arch/cortex-m23/m2351/src/numaker_pfm_m2351/Makefile
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

MTOWER_S = $(TOPDIR)/mtower_s.bin
MTOWER_NS = $(TOPDIR)/mtower_ns.bin

ifeq ($(CONFIG_BOOTLOADER2),y)
BL2 := $(TOPDIR)/build/secure$(subst $(TOPDIR),,$(shell pwd))/../NuBL2/bl2.bin
ECDSA_KEYS := ../NuBL2/KeyInfo/ecdsa_keys.bin
else
BL2 := null
ECDSA_KEYS := null
endif

ifeq ($(CONFIG_BOOTLOADER32),y)
BL32 := $(TOPDIR)/build/secure$(subst $(TOPDIR),,$(shell pwd))/secure/bl32.bin
else
BL32 := null
endif
ifeq ($(CONFIG_BOOTLOADER33),y)
BL33 := $(TOPDIR)/build/nonsecure$(subst $(TOPDIR),,$(shell pwd))/nonsecure/bl33.bin
else
BL33 := null
endif

mtower$(EXEEXT):
ifeq ($(CONFIG_BOOTLOADER2),y)
	$(Q) $(MAKE) -C ../NuBL2 TOPDIR="$(TOPDIR)" NuBL2$(EXEEXT)
endif
ifeq ($(CONFIG_BOOTLOADER32),y)
	$(Q) $(MAKE) -C secure TOPDIR="$(TOPDIR)" mtower_s$(EXEEXT)
endif
ifeq ($(CONFIG_BOOTLOADER33),y)
	$(Q) $(MAKE) -C nonsecure TOPDIR="$(TOPDIR)" mtower_ns$(EXEEXT)
endif
	$(TOPDIR)/tools/fwinfogen $(BL2) $(BL32) $(MTOWER_S) $(ECDSA_KEYS) $(BL33) $(MTOWER_NS)
	$(Q) cp -f $(TOPDIR)/mtower_ns.bin /media/sf_Shared/
	$(Q) cp -f $(TOPDIR)/mtower_s.bin /media/sf_Shared/

	$(Q) printf "$(GREEN)+--------------------------------------------------------------------+$(NORMAL)\n"
	$(Q) printf "$(GREEN)| mtower_s.bin                                                       |$(NORMAL)\n"
	$(Q) printf "$(GREEN)+--------------------------------------------------------------------+$(NORMAL)\n"
	$(Q) printf "$(GREEN)| Name            | Start addr | End addr   | Size      | File name  |$(NORMAL)\n"
	$(Q) printf "$(GREEN)+--------------------------------------------------------------------+$(NORMAL)\n"
ifeq ($(CONFIG_BOOTLOADER2),y)
	$(eval bl2_sz = `stat -c%s $(TOPDIR)/build/secure$(subst $(TOPDIR),,$(shell pwd))/../NuBL2/bl2.bin`)
	$(eval bl2_start = 0x00000000)

	$(Q) printf "$(GREEN)| Boot loader 2   | 0x%08x | 0x%08x | %u\t| bl2.bin    |$(NORMAL)\n" $(bl2_start) $$(( $(bl2_start) + $(bl2_sz) )) $(bl2_sz)
endif

ifeq ($(CONFIG_BOOTLOADER32),y)
	$(eval bl32_sz = `stat -c%s $(TOPDIR)/build/secure$(subst $(TOPDIR),,$(shell pwd))/secure/bl32.bin`)
	$(eval bl32_start = $(CONFIG_START_ADDRESS_BL32))

	$(Q) printf "$(GREEN)| Secure handler  | 0x%08x | 0x%08x | %u\t| bl32.bin   |$(NORMAL)\n" $(bl32_start) $$(( $(bl32_start) + $(bl32_sz) )) $(bl32_sz)
endif
	$(Q) printf "$(GREEN)+--------------------------------------------------------------------+$(NORMAL)\n"

ifeq ($(CONFIG_BOOTLOADER33),y)
	$(eval bl33_sz = `stat -c%s $(TOPDIR)/build/nonsecure$(subst $(TOPDIR),,$(shell pwd))/nonsecure/bl33.bin`)
	$(eval bl33_start = 0x10040000)

	$(Q) printf "$(GREEN)+--------------------------------------------------------------------+$(NORMAL)\n"
	$(Q) printf "$(GREEN)| mtower_ns.bin                                                      |$(NORMAL)\n"
	$(Q) printf "$(GREEN)+--------------------------------------------------------------------+$(NORMAL)\n"
	$(Q) printf "$(GREEN)| Name            | Start addr | End addr   | Size      | File name  |$(NORMAL)\n"
	$(Q) printf "$(GREEN)+--------------------------------------------------------------------+$(NORMAL)\n"
	$(Q) printf "$(GREEN)| FreeRTOS        | 0x%08x | 0x%08x | %u\t| bl33.bin   |$(NORMAL)\n" $(bl33_start) $$(( $(bl33_start) + $(bl33_sz) )) $(bl33_sz)
	$(Q) printf "$(GREEN)+--------------------------------------------------------------------+$(NORMAL)\n"
endif

clean:
	$(Q) $(MAKE) -C secure TOPDIR="$(TOPDIR)" clean
	$(Q) $(MAKE) -C nonsecure TOPDIR="$(TOPDIR)" clean
	$(Q) rm -rf $(TOPDIR)/build
	$(Q) rm -f $(TOPDIR)/mtower_s.bin
	$(Q) rm -f $(TOPDIR)/mtower_ns.bin

distclean: clean

flash:
	$(Q)$(ECHO) -e "$(BLUE)Not supported yet!$(NORMAL)"
