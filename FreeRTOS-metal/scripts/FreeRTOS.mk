#
# Copyright 2019 SiFive, Inc #
# SPDX-License-Identifier: Apache-2.0 #
#

ifeq ($(FREERTOS_DIR),)
	ERR = $(error Please specify FreeRTOS directory by using FREERTOS_DIR variable )
endif

# ----------------------------------------------------------------------
# Includes Location
# ----------------------------------------------------------------------
override FREERTOS_INCLUDES := $(FREERTOS_DIR)/FreeRTOS-Kernel/include
override FREERTOS_INCLUDES += $(FREERTOS_DIR)/FreeRTOS-Kernel/portable/GCC/RISC-V
override FREERTOS_INCLUDES += $(FREERTOS_DIR)/FreeRTOS-Kernel/portable/GCC/RISC-V/chip_specific_extensions/RV32I_CLINT_no_extensions

