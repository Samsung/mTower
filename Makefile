############################################################################
# Makefile
#
# Copyright (c) 2018 Samsung Electronics Co., Ltd. All Rights Reserved.
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

TOPDIR := ${shell pwd | sed -e 's/ /\\ /g'}

-include $(TOPDIR)/.config
-include $(TOPDIR)/Make.defs

# Control build verbosity
#
#  V=1,2: Enable echo of commands
#  V=2:   Enable bug/verbose options in tools and scripts

ifeq ($(V),1)
export Q :=
else
ifeq ($(V),2)
export Q :=
else
export Q := @
endif
endif

# Process architecture and board-specific directories

ARCH_DIR = arch/$(CONFIG_ARCH)/$(CONFIG_ARCH_FAMILY)
ARCH_SRC = $(ARCH_DIR)/src/$(CONFIG_PLATFORM)
ARCH_INC = $(ARCH_DIR)/include

# Lists of build directories.

# CONTEXTDIRS include directories that have special, one-time pre-build
#   requirements.  Normally this includes things like auto-generation of
#   configuration specific files.

CONTEXTDIRS = configs $(APPDIR)

# MTOWERLIBS is the list of mTower libraries that is passed to the
#   processor-specific Makefile to build the final mTower target.

#MTOWERLIBS =

#ifeq ($(CONFIG_MTOWER_CRYPTO),y)
#MTOWERLIBS += crypto
#endif

#ifeq ($(CONFIG_MTOWER_LIBC),y)
#MTOWERLIBS += libc
#endif

# OTHERDIRS - These are directories that are not built but probably should
#   be cleaned to prevent garbage from collecting in them when changing
#   configurations.

OTHERDIRS = lib

# CLEANDIRS are the directories that will clean in.  These are
#   all directories that we know about.

CLEANDIRS = $(MTOWERLIBS) $(OTHERDIRS) $(ARCH_SRC)

# This is the name of the final target (relative to the top level directorty)

MTOWERNAME = mtower

BIN = $(MTOWERNAME)$(EXEEXT)

PHONY = context clean_context check_context subdir_clean clean subdir_distclean \
distclean savedefconfig docs_gen docs_show toolchain

.PHONY: $(PHONY)

all: $(BIN)

# Targets used to build include/mtower/config.h.  Creation of config.h is
# part of the overall mTower configuration sequence. Notice that the
# tools/mkconfig tool is built and used to create include/mTower/config.h

tools/mkconfig$(HOSTEXEEXT):
	$(Q) $(MAKE) -C tools -f Makefile.host TOPDIR="$(TOPDIR)" mkconfig$(HOSTEXEEXT)

include/mtower/config.h: $(TOPDIR)/.config tools/mkconfig$(HOSTEXEEXT)
	$(Q) tools/mkconfig $(TOPDIR) > include/mtower/config.h

.config:
	@cp configs/$(PLATFORM)/defconfig .config

Make.defs:
	@cp configs/$(PLATFORM)/Make.defs Make.defs

create_context: Make.defs .config

# toolchain
#
# The toolchain target must be called to assembly mTower with a specific cross
# cross compiler (loading and unpacking). 

$(TOPDIR)/toolchain/$(GCC_SOURCE):
	$(Q) if [ ! -e $(TOPDIR)/toolchain/$(GCC_SOURCE) ]; then \
		mkdir -p $(TOPDIR)/toolchain ; \
		wget -P $(TOPDIR)/toolchain $(GCC_SITE)/$(GCC_SOURCE) ; \
	fi

gcc-unpacked: $(TOPDIR)/toolchain/$(GCC_SOURCE)
	$(Q) if [ ! -d $(TOOLCHAINPATH) ]; then \
		tar xjf $(TOPDIR)/toolchain/$(GCC_SOURCE) -C $(TOPDIR)/toolchain ; \
	fi

toolchain: check_context Make.defs .config $(TOPDIR)/toolchain/$(GCC_SOURCE) gcc-unpacked

# context
#
# The context target is invoked on each target build to assure that mTower is
# properly configured. The basic configuration steps include creation of the
# the config.h header file in the include/mtower directory.

context: check_context include/mtower/config.h Make.defs
	$(Q) for dir in $(CONTEXTDIRS) ; do \
		$(MAKE) -C $$dir TOPDIR="$(TOPDIR)" context; \
	done

# clean_context
#
# This is part of the distclean target. It removes all of the header files
# created by the context target.

clean_context:
	$(Q) $(MAKE) -C configs TOPDIR="$(TOPDIR)" clean_context
	$(Q) rm -f include/mtower/config.h

# check_context
#
# This target checks if mTower has been configured. mTower is configured using
# the script tools/configure.sh.  That script will install certain files in
# the top-level mTower build directory.  This target verifies that those
# configuration files have been installed and that mTower is ready to be built.

check_context:
	$(Q) if [ ! -e ${TOPDIR}/.config -o ! -e ${TOPDIR}/Make.defs ]; then \
		echo "" ; echo "mTower has not been configured:" ; \
		echo "  make PLATFORM=<platform> create_context" ; echo "" ; \
		exit 1 ; \
	fi

# The target build sequence will first create a series of  libraries, one per 
# configured source file directory. The final mTower execution will then be built
# from those libraries.  The following targets build those libraries.

mtower_libs:
	$(Q) for dir in $(MTOWERLIBS) ; do \
		$(MAKE) -C $$dir TOPDIR="$(TOPDIR)" lib$$dir$(LIBEXT) ; \
	done

# $(BIN)
#
# Create the final mTower executable by build process.

$(BIN): context mtower_libs 
	$(Q) $(MAKE) -C $(ARCH_SRC) TOPDIR="$(TOPDIR)" $(BIN)
	$(Q) if [ -e $(TOPDIR)/mtower ] || [ -e $(TOPDIR)/mtower.bin ]; then \
		$(ECHO) -e "$(GREEN)***********************************************$(NORMAL)" ; \
		$(ECHO) -e "$(GREEN)**** mTower has been successfully compiled ****$(NORMAL)" ; \
		$(ECHO) -e "$(GREEN)***********************************************$(NORMAL)" ; \
	fi


# Flash MCU.
#
# flash

flash:
	$(Q) $(MAKE) -C $(ARCH_SRC) TOPDIR="$(TOPDIR)" flash

# Configuration targets
#
# These targets depend on the kconfig-frontends packages.  To use these, you
# must first download and install the kconfig-frontends package from this
# location: http://ymorin.is-a-geek.org/projects/kconfig-frontends.  See
# README.md file in the mTower/tools GIT repository for additional information.

do_menuconfig:
	$(Q) kconfig-mconf Kconfig

menuconfig: do_menuconfig 

do_savedefconfig:
	@cp -f .config configs/$(CONFIG_PLATFORM)/defconfig

savedefconfig: do_savedefconfig

savedefmakedefs:
	@cp -f Make.defs configs/$(CONFIG_PLATFORM)/

# clean:     Removes derived object files, archives, executables, and
#            temporary files, but retains the configuration and context
#            files and directories.
# distclean: Does 'clean' then also removes all configuration and context
#            files. This essentially restores the directory structure
#            to its original, unconfigured stated.

subdir_clean:
	$(Q) for dir in $(CLEANDIRS) ; do \
		if [ -e $$dir/Makefile ]; then \
			$(MAKE) -C $$dir TOPDIR="$(TOPDIR)" clean ; \
		fi \
	done
	$(Q) $(MAKE) -C tools -f Makefile.host TOPDIR="$(TOPDIR)" clean

clean: subdir_clean
	$(Q) rm -f $(MTOWERNAME)*
	$(Q) rm -rf docs/html/

subdir_distclean:
	$(Q) for dir in $(CLEANDIRS) ; do \
		if [ -e $$dir/Makefile ]; then \
			$(MAKE) -C $$dir TOPDIR="$(TOPDIR)" distclean ; \
		fi \
	done

distclean: clean subdir_distclean clean_context
	$(Q) rm -f Make.defs
	$(Q) rm -f .config
	$(Q) rm -f .config.old

docs_gen:
	$(Q) doxygen

docs_show:
	$(Q) firefox docs/html/index.html
