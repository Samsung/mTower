############################################################################
# arch/cortex-m23/m2351/src/m2351_badge/Makefile
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

MTOWER_S = $(TOPDIR)/mtower_s$(EXEEXT)
MTOWER_NS = $(TOPDIR)/mtower_ns$(EXEEXT)

mtower$(EXEEXT):
#ifeq ($(CONFIG_BOOTLOADER2),y)
#	$(Q) $(MAKE) -C ../NuBL2 TOPDIR="$(TOPDIR)" NuBL2$(EXEEXT)
#endif
#ifeq ($(CONFIG_BOOTLOADER32),y)
	$(Q) $(MAKE) -C secure TOPDIR="$(TOPDIR)" mtower_s$(EXEEXT)
#endif
ifeq ($(CONFIG_BOOTLOADER33),y)
	$(Q) $(MAKE) -C nonsecure TOPDIR="$(TOPDIR)" mtower_ns$(EXEEXT)
endif
#ifeq ($(CONFIG_BOOTLOADER32),y)
	$(Q) cat $(TOPDIR)/mtower_s.txt
#endif
ifeq ($(CONFIG_BOOTLOADER33),y)
	$(Q) cat $(TOPDIR)/mtower_ns.txt
endif

clean:
	$(Q) $(MAKE) -C secure TOPDIR="$(TOPDIR)" clean
	$(Q) $(MAKE) -C nonsecure TOPDIR="$(TOPDIR)" clean
	$(Q) rm -rf $(TOPDIR)/build
	$(Q) rm -f $(TOPDIR)/mtower_s.bin
	$(Q) rm -f $(TOPDIR)/mtower_s.hex
	$(Q) rf -f $(TOPDIR)/mtower_s.txt
	$(Q) rm -f $(TOPDIR)/mtower_ns.bin
	$(Q) rm -f $(TOPDIR)/mtower_ns.hex
	$(Q) rf -f $(TOPDIR)/mtower_ns.txt

distclean: clean

flash:
	$(Q)$(ECHO) -e "$(BLUE)Not supported yet!$(NORMAL)"
