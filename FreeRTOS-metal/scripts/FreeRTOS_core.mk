#
# Copyright 2019 SiFive, Inc #
# SPDX-License-Identifier: Apache-2.0 #
#


# ----------------------------------------------------------------------
# CORE PART
# ----------------------------------------------------------------------
override SOURCE_DIRS := $(SOURCE_DIR)

# ----------------------------------------------------------------------
# Add Platform port - Here file for RISC-V port
# ----------------------------------------------------------------------
override SOURCE_DIRS += $(SOURCE_DIR)/portable/GCC/RISC-V

# ----------------------------------------------------------------------
# Add common directory for all Platform port 
# ----------------------------------------------------------------------
ifeq ($(PMP),ENABLE)
	override SOURCE_DIRS += $(SOURCE_DIR)/portable/Common
	override MAKE_CONFIG += freeRTOS.define.portUSING_MPU_WRAPPERS = 1
endif

# ---------------------------------------------------------------------
override C_SOURCES := $(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.c))

override ASM_SOURCES := $(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.[Ss])) \
						$(foreach dir,$(SOURCE_DIRS),$(wildcard $(dir)/*.asm))

# ----------------------------------------------------------------------
# Define HEAP managment model
# ----------------------------------------------------------------------
ifeq ($(HEAP),1)
	override C_SOURCES += $(SOURCE_DIR)/portable/MemMang/heap_1.c
else ifeq ($(HEAP),2)
	override C_SOURCES += $(SOURCE_DIR)/portable/MemMang/heap_2.c
else ifeq ($(HEAP),3)
	override C_SOURCES += $(SOURCE_DIR)/portable/MemMang/heap_3.c
else ifeq ($(HEAP),4)
	override C_SOURCES += $(SOURCE_DIR)/portable/MemMang/heap_4.c
else ifeq ($(HEAP),5)
	override C_SOURCES += $(SOURCE_DIR)/portable/MemMang/heap_5.c
else
	ERR = $(error No heap management selected)
endif

FREERTOS_DIR = $(CURRENT_DIR)
include $(CURRENT_DIR)/scripts/FreeRTOS.mk

override INCLUDE_DIRS := $(FREERTOS_INCLUDES)

override INCLUDES := $(foreach dir,$(INCLUDE_DIRS),$(wildcard $(dir)/*.h))

override C_OBJS := $(subst $(SOURCE_DIR),$(BUILD_DIR),$(C_SOURCES:.c=.o))
override ASM_OBJS := 	$(subst $(SOURCE_DIR),$(BUILD_DIR), $(patsubst %.S,%.o,$(filter %.S,$(ASM_SOURCES))))\
						$(subst $(SOURCE_DIR),$(BUILD_DIR), $(patsubst %.s,%.o,$(filter %.s,$(ASM_SOURCES))))\
						$(subst $(SOURCE_DIR),$(BUILD_DIR), $(patsubst %.asm,%.o,$(filter %.asm,$(ASM_SOURCES))))

override OBJS := $(C_OBJS) $(ASM_OBJS)