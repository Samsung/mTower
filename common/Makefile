############################################################################
# common/Makefile
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

CFLAGS += -I.
CFLAGS += -I$(TOPDIR)/include/mtower

ifeq ($(WORLD), secure)
CFLAGS += -mcmse
endif

SRCS  = heap_4.c 

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

liballoc_s.a: $(OBJS)
	$(Q) $(AR) $(OBJDIR)/$@ $(OBJS)
	$(Q) cp $(OBJDIR)/$@ $(TOPDIR)/lib/$@

clean:
	rm -f $(OBJS) liballoc_s.a
