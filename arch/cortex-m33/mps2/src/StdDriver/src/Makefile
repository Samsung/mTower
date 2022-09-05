############################################################################
# arch/cortex-m33/mps2/src/mps2_an505_qemu/Makefile
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

CFLAGS += -I../../CMSIS/Include
CFLAGS += -I../../StdDriver/inc
CFLAGS += -I$(TOPDIR)/include/mtower
CFLAGS += -I$(TOPDIR)/arch/$(CONFIG_ARCH)/$(CONFIG_ARCH_FAMILY)/src/$(CONFIG_PLATFORM)

ifeq ($(WORLD), secure)
CFLAGS += -mcmse
endif

CFLAGS += -Wno-unused-parameter -Wno-undef -Wno-maybe-uninitialized
CFLAGS += -Wno-unused-variable -Wno-pointer-compare

#SRCS += acmp.c
#SRCS += bpwm.c
#SRCS += can.c
#SRCS += crc.c
#SRCS += crypto.c
#SRCS += dac.c
#SRCS += eadc.c
#SRCS += ebi.c
#SRCS += ecap.c
#SRCS += epwm.c
SRCS += fmc.c
SRCS += gpio.c
#SRCS += i2c.c
#SRCS += i2s.c
#SRCS += pdma.c
#SRCS += qei.c
#SRCS += qspi.c
SRCS += retarget.c
#SRCS += rtc.c
#SRCS += sc.c
#SRCS += scuart.c
#SRCS += sdh.c
#SRCS += spi.c
SRCS += sys.c
#SRCS += timer.c
#SRCS += timer_pwm.c
SRCS += uart.c
#SRCS += usbd.c
#SRCS += usci_i2c.c
#SRCS += usci_spi.c
#SRCS += usci_uart.c
#SRCS += wdt.c
#SRCS += wwdt.c

ifeq ($(WORLD), secure)
SRCS += clk.c
endif

ifneq ($(WORLD), secure)
CFLAGS += -DDEBUG_PORT=$(subst ",,$(CONFIG_NONSECURE_DEBUG_UART))
CFLAGS += -DDEBUG_OUT_PREFIX=$(CONFIG_NONSECURE_FONT_COLOR)
OBJDIR = $(TOPDIR)/build/nonsecure$(subst $(TOPDIR),,$(shell pwd))
endif

ifeq ($(WORLD), secure)
CFLAGS += -DDEBUG_PORT=$(subst ",,$(CONFIG_SECURE_DEBUG_UART))
CFLAGS += -DDEBUG_OUT_PREFIX=$(CONFIG_SECURE_FONT_COLOR)
OBJDIR = $(TOPDIR)/build/secure$(subst $(TOPDIR),,$(shell pwd))
endif

OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.c=.o))

current_dir = $(subst $(TOPDIR),,$(shell pwd))

$(OBJDIR)/%.o : %.c
	$(Q) mkdir -p $(OBJDIR)/$(dir $<)
	@echo "CC: $<"
	$(Q) $(CC) $(CFLAGS) -c -o $@ $^

libStdDriver_s.a: $(OBJS)
	$(Q) $(AR) $(OBJDIR)/$@ $(OBJS)
	$(Q) cp $(OBJDIR)/$@ $(TOPDIR)/lib/$@

libStdDriver_ns.a: $(OBJS)
	$(Q) $(AR) $(OBJDIR)/$@ $(OBJS)
	$(Q) cp $(OBJDIR)/$@ $(TOPDIR)/lib/$@

clean:
	rm -f libStdDriver.a

