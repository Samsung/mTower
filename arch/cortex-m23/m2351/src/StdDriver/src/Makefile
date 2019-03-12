############################################################################
# arch/cortex-m23/m2351/src/StdDriver/Makefile
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

# Check for valid float argument
ifneq ($(FLOAT_TYPE), hard)
ifneq ($(FLOAT_TYPE), soft)
#override FLOAT_TYPE = hard
override FLOAT_TYPE = soft
endif
endif

CFLAGS += -I../../CMSIS/Include
CFLAGS += -I../../StdDriver/inc
CFLAGS += -I../../Device/Nuvoton/M2351/Include

ifeq ($(WORLD), secure)
CFLAGS += -mcmse
endif

SRCS = acmp.c bpwm.c can.c crc.c crypto.c dac.c eadc.c ebi.c ecap.c epwm.c \
       fmc.c gpio.c i2c.c i2s.c pdma.c qei.c qspi.c qspi.c retarget.c rtc.c \
       sc.c scuart.c sdh.c spi.c sys.c timer.c timer_pwm.c uart.c usbd.c \
       usci_i2c.c usci_spi.c usci_uart.c wdt.c wwdt.c

ifeq ($(WORLD), secure)
SRCS += clk.c
endif

ifneq ($(WORLD), secure)
CFLAGS += -DDEBUG_PORT=UART1_NS
endif

OBJS = $(SRCS:.c=.o)

.PHONY: libm2351_StdDriver.a

all: libm2351_StdDriver.a

%.o : %.c
	@echo "CC: $<"
	$(Q) $(CC) $(CFLAGS) -c -o $@ $^

libm2351_StdDriver.a: $(OBJS)
	$(Q) $(AR) $@ $(OBJS)

clean:
	rm -f $(OBJS) libm2351_StdDriver.a

