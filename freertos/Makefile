############################################################################
# freertos/Makefile
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

CFLAGS += -I./include
CFLAGS += -I$(TOPDIR)/include/mtower
CFLAGS += -I$(TOPDIR)/arch/$(CONFIG_ARCH)/$(CONFIG_ARCH_FAMILY)/src/$(CONFIG_PLATFORM)
CFLAGS += -I./portable/GCC/$(FREE_RTOS_ARCH)
CFLAGS += -I../arch/$(subst ",,$(CONFIG_ARCH))/$(subst ",,$(CONFIG_ARCH_FAMILY))/src/$(subst ",,$(CONFIG_PLATFORM))/nonsecure
CFLAGS += -I$(TOPDIR)/arch/$(CONFIG_ARCH)/$(CONFIG_ARCH_FAMILY)/src/CMSIS/Include
ifeq ($(WORLD), secure)
CFLAGS += -mcmse
endif

SRCS  = croutine.c list.c queue.c stream_buffer.c tasks.c timers.c event_groups.c
SRCS += portable/GCC/$(FREE_RTOS_ARCH)/port.c portable/MemMang/heap_4.c

ifeq ($(FREE_RTOS_ARCH), RISC-V)
SRCS += portable/GCC/$(FREE_RTOS_ARCH)/pmp.c
SRCS += portable/Common/mpu_wrappers.c
CFLAGS += -DportUSING_MPU_WRAPPERS=1
endif

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

libFreeRTOS_s.a: $(OBJS)
	$(Q) $(AR) $(OBJDIR)/$@ $(OBJS)
	$(Q) cp $(OBJDIR)/$@ $(TOPDIR)/lib/$@

libFreeRTOS_ns.a: $(OBJS)
	$(Q) $(AR) $(OBJDIR)/$@ $(OBJS)
	$(Q) cp $(OBJDIR)/$@ $(TOPDIR)/lib/$@

clean:
	rm -f $(OBJS) libm2351_StdDriver.a

