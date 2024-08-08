############################################################################
# arch/riscv32/bl808/src/Makefile
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

# ifeq ($(WORLD), secure)
# CFLAGS += -mcmse
# endif

SRCS_LIBC = components/libc/newlib/syscalls_nosys.c
SRCS_LIBC += components/libc/vsnprintf.c
SRCS_LIBC += components/libc/snprintf.c 
SRCS_LIBC += components/libc/sprintf.c 
SRCS_LIBC += components/libc/vsprintf.c 
SRCS_LIBC += components/libc/printf.c

SRCS_MM  = components/mm/tlsf/bflb_tlsf.c
SRCS_MM += components/mm/tlsf/tlsf.c
SRCS_MM += components/mm/mem.c

SRCS_LHAL = drivers/lhal/config/bl808/device_table.c
SRCS_LHAL += drivers/lhal/src/bflb_common.c
# SRCS_LHAL += drivers/lhal/src/bflb_adc.c
# SRCS_LHAL += drivers/lhal/src/bflb_cks.c
# SRCS_LHAL += drivers/lhal/src/bflb_ef_ctrl.c
SRCS_LHAL += drivers/lhal/src/bflb_gpio.c
# SRCS_LHAL += drivers/lhal/src/bflb_i2c.c
# SRCS_LHAL += drivers/lhal/src/bflb_dma.c
# SRCS_LHAL += drivers/lhal/src/bflb_rtc.c
# SRCS_LHAL += drivers/lhal/src/bflb_sec_aes.c
# SRCS_LHAL += drivers/lhal/src/bflb_sec_sha.c
# SRCS_LHAL += drivers/lhal/src/bflb_sec_trng.c
# SRCS_LHAL += drivers/lhal/src/bflb_spi.c
# SRCS_LHAL += drivers/lhal/src/bflb_timer.c
SRCS_LHAL += drivers/lhal/src/bflb_uart.c
# SRCS_LHAL += drivers/lhal/src/bflb_wdg.c
SRCS_LHAL += drivers/lhal/src/bflb_flash.c
# SRCS_LHAL += drivers/lhal/src/bflb_dac.c
# SRCS_LHAL += drivers/lhal/src/bflb_emac.c
# SRCS_LHAL += drivers/lhal/src/bflb_ir.c
# SRCS_LHAL += drivers/lhal/src/bflb_mjpeg.c
# SRCS_LHAL += drivers/lhal/src/bflb_pwm_v2.c
# SRCS_LHAL += drivers/lhal/src/bflb_cam.c
# SRCS_LHAL += drivers/lhal/src/bflb_iso11898.c
# SRCS_LHAL += drivers/lhal/src/bflb_csi.c
# SRCS_LHAL += drivers/lhal/src/bflb_i2s.c
SRCS_LHAL += drivers/lhal/src/bflb_irq.c
# SRCS_LHAL += drivers/lhal/src/bflb_l1c.c
SRCS_LHAL += drivers/lhal/src/bflb_mtimer.c
SRCS_LHAL += drivers/lhal/include/arch/risc-v/t-head/rv_hart.c
SRCS_LHAL += drivers/lhal/include/arch/risc-v/t-head/rv_pmp.c
SRCS_LHAL += drivers/lhal/src/flash/bflb_sf_ctrl.c
SRCS_LHAL += drivers/lhal/src/flash/bflb_sflash.c
SRCS_LHAL += drivers/lhal/src/flash/bflb_sf_cfg.c
SRCS_LHAL += drivers/lhal/src/flash/bflb_xip_sflash.c
# SRCS_LHAL += drivers/lhal/src/bflb_kys.c
# SRCS_LHAL += drivers/lhal/src/bflb_pwm_v1.c
# SRCS_LHAL += drivers/lhal/src/bflb_auadc.c
# SRCS_LHAL += drivers/lhal/src/bflb_clock.c
# SRCS_LHAL += drivers/lhal/src/bflb_dbi.c
# SRCS_LHAL += drivers/lhal/src/bflb_sec_irq.c
# SRCS_LHAL += drivers/lhal/src/bflb_audac.c
# SRCS_LHAL += drivers/lhal/src/bflb_sdio2.c

SRCS_STD = drivers/soc/bl808/std/src/bl808_aon.c
SRCS_STD += drivers/soc/bl808/std/src/bl808_clock.c
SRCS_STD += drivers/soc/bl808/std/port/bl808_clock.c
SRCS_STD += drivers/soc/bl808/std/src/bl808_common.c
SRCS_STD += drivers/soc/bl808/std/src/bl808_ef_cfg.c
SRCS_STD += drivers/soc/bl808/std/src/bl808_glb.c
SRCS_STD += drivers/soc/bl808/std/src/bl808_glb_gpio.c
SRCS_STD += drivers/soc/bl808/std/src/bl808_hbn.c
SRCS_STD += drivers/soc/bl808/std/src/bl808_l1c.c
SRCS_STD += drivers/soc/bl808/std/src/bl808_pds.c
# SRCS_STD += drivers/soc/bl808/std/src/bl808_psram_uhs.c
# SRCS_STD += drivers/soc/bl808/std/src/bl808_sdh.c
# SRCS_STD += drivers/soc/bl808/std/src/bl808_tzc_sec.c
# SRCS_STD += drivers/soc/bl808/std/src/bl808_uhs_phy.c
SRCS_STD += drivers/soc/bl808/std/startup/m0/interrupt.c
SRCS_STD += drivers/soc/bl808/std/startup/m0/start_load.c
SRCS_STD += drivers/soc/bl808/std/startup/m0/system_bl808.c

SRCS_ASM_STD += drivers/soc/bl808/std/startup/m0/riscv_fpu.S
SRCS_ASM_STD += drivers/soc/bl808/std/startup/m0/start.S
SRCS_ASM_STD += drivers/soc/bl808/std/startup/m0/vector.S

SRCS_ASM_FREERTOS  = $(TOPDIR)/freertos/portable/GCC/RISC-V/portASM.S

SRCS_FREERTOS += $(TOPDIR)/freertos/croutine.c
SRCS_FREERTOS += $(TOPDIR)/freertos/event_groups.c
SRCS_FREERTOS += $(TOPDIR)/freertos/list.c
SRCS_FREERTOS += $(TOPDIR)/freertos/queue.c
SRCS_FREERTOS += $(TOPDIR)/freertos/tasks.c
SRCS_FREERTOS += $(TOPDIR)/freertos/timers.c
SRCS_FREERTOS += $(TOPDIR)/freertos/portable/MemMang/heap_4.c
SRCS_FREERTOS += $(TOPDIR)/freertos/portable/GCC/RISC-V/pmp.c
SRCS_FREERTOS += $(TOPDIR)/freertos/portable/GCC/RISC-V/bl808_port.c
SRCS_FREERTOS += $(TOPDIR)/freertos/portable/Common/mpu_wrappers.c
SRCS_FREERTOS += $(TOPDIR)/freertos/stream_buffer.c

# ifeq ($(WORLD), secure)
# SRCS += clk.c
# endif

# ifneq ($(WORLD), secure)
# CFLAGS += -DDEBUG_PORT=$(subst ",,$(CONFIG_NONSECURE_DEBUG_UART))
# CFLAGS += -DDEBUG_OUT_PREFIX=$(CONFIG_NONSECURE_FONT_COLOR)
OBJDIR = $(TOPDIR)/build/nonsecure$(subst $(TOPDIR),,$(shell pwd))
# endif

# ifeq ($(WORLD), secure)
# CFLAGS += -DDEBUG_PORT=$(subst ",,$(CONFIG_SECURE_DEBUG_UART))
# CFLAGS += -DDEBUG_OUT_PREFIX=$(CONFIG_SECURE_FONT_COLOR)
# OBJDIR = $(TOPDIR)/build/secure$(subst $(TOPDIR),,$(shell pwd))
# endif

# OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.c=.o))

OBJS_LIBC = $(addprefix $(OBJDIR)/, $(SRCS_LIBC:.c=.o))
OBJS_MM = $(addprefix $(OBJDIR)/, $(SRCS_MM:.c=.o))
OBJS_LHAL = $(addprefix $(OBJDIR)/, $(SRCS_LHAL:.c=.o))
OBJS_STD = $(addprefix $(OBJDIR)/, $(SRCS_STD:.c=.o))
AOBJS_STD += $(addprefix $(OBJDIR)/, $(SRCS_ASM_STD:.S=.o))
OBJS_FREERTOS = $(addprefix $(OBJDIR)/, $(SRCS_FREERTOS:.c=.o))
AOBJS_FREERTOS = $(addprefix $(OBJDIR)/, $(SRCS_ASM_FREERTOS:.S=.o))

current_dir = $(subst $(TOPDIR),,$(shell pwd))

$(OBJDIR)/%.o : %.c
	$(Q) mkdir -p $(OBJDIR)/$(dir $<)
	@echo "CC: $<"
	$(Q) $(CC) $(CFLAGS) -c -o $@ $^

$(OBJDIR)/%.o : %.S
	$(Q) mkdir -p $(OBJDIR)/$(dir $<)
	@echo "CC: $<"
	$(Q) $(CC) $(CFLAGS) -c -o $@ $^

#$(AOBJS_NS): $(OBJDIR)/%$(OBJEXT): %.S
#	$(Q) mkdir -p $(OBJDIR)/$(dir $<)
#	@echo "AS: $<"
#	$(Q) $(CC) -c $(CFLAGS) $< -o $@

liblibc.a: $(OBJS_LIBC)
	$(Q) $(AR) $(OBJDIR)/$@ $(OBJS_LIBC)
	$(Q) cp $(OBJDIR)/$@ $(TOPDIR)/lib/$@

libmm.a: $(OBJS_MM)
	$(Q) $(AR) $(OBJDIR)/$@ $(OBJS_MM)
	$(Q) cp $(OBJDIR)/$@ $(TOPDIR)/lib/$@

liblhal.a: $(OBJS_LHAL)
	$(Q) $(AR) $(OBJDIR)/$@ $(OBJS_LHAL)
	$(Q) cp $(OBJDIR)/$@ $(TOPDIR)/lib/$@

libstd.a: $(OBJS_STD) $(AOBJS_STD)
	$(Q) $(AR) $(OBJDIR)/$@ $(OBJS_STD) $(AOBJS_STD)
	$(Q) cp $(OBJDIR)/$@ $(TOPDIR)/lib/$@

libfreertos.a: $(OBJS_FREERTOS) $(AOBJS_FREERTOS)
	$(Q) $(AR) $(OBJDIR)/$@ $(OBJS_FREERTOS) $(AOBJS_FREERTOS)
	$(Q) cp $(OBJDIR)/$@ $(TOPDIR)/lib/$@

clean:
	rm -f liblibc.a libmm.a liblhal.a libstd.a libfreertos.a

