# * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
# * SPDX-License-Identifier : Apache-2.0
# *
# * Licensed under the Apache License, Version 2.0 (the "License");
# * you may not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# *  http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
#**/

include $(SOURCE)/tools/makefiles/toolchain.mk
include $(SUITE_IN)/$(TEST)/source.mk

INCLUDE= -I$(SOURCE)/val/common/ \
         -I$(SOURCE)/val/nspe/ \
         -I$(SOURCE)/val/spe/ \
         -I$(SOURCE)/ \
         -I$(SOURCE)/platform/targets/$(TARGET)/nspe/ \
         -I$(SOURCE)/platform/targets/$(TARGET)/nspe/common \
         -I$(SOURCE)/platform/targets/$(TARGET)/nspe/crypto \
         -I$(BUILD)/platform/$(TARGET)/ \
         -I$(SUITE_IN)/$(TEST)/\
         -I$(SUITE_IN)/include/ \
         -I$(BUILD)/val/

ifeq (${SUITE}, protected_storage)
INCLUDE += -I$(SUITE_IN)/../internal_trusted_storage/$(TEST)
endif

VPATH=$(SOURCE)/val/common/:\
      $(SOURCE)/val/nspe/:\
      $(SOURCE)/val/spe/:\
      $(SUITE_IN)/$(TEST)/:\
      $(SUITE_IN)/include/:

ifeq (${SUITE}, protected_storage)
VPATH += $(SUITE_IN)/../internal_trusted_storage/$(TEST)/
endif


all:  mkdir compile_c_nspe compile_asm_nspe test.elf compile_c_spe compile_asm_spe

mkdir:
	mkdir -p $(SUITE_OUT)/$(TEST)/

compile_c_nspe: $(CC_SOURCE:%.c=$(SUITE_OUT)/$(TEST)/%_nspe.o)
compile_asm_nspe: $(AS_SOURCE:%.s=$(SUITE_OUT)/$(TEST)/%_nspe.o)

$(SUITE_OUT)/$(TEST)/%_nspe.o : %.c
	$(CC) -D NONSECURE_TEST_BUILD -DVAL_NSPE_BUILD -o $@ -c $<

$(SUITE_OUT)/$(TEST)/%_nspe.o : %.s
	$(AS) -o $@ $<

# Generated %_spe.o(s) are used in spbuild.mk to create final client_partition.a and server_partition.a
compile_c_spe: $(CC_SOURCE_SPE:%.c=$(SUITE_OUT)/$(TEST)/%_spe.o)
compile_asm_spe: $(AS_SOURCE_SPE:%.s=$(SUITE_OUT)/$(TEST)/%_spe.o)

$(SUITE_OUT)/$(TEST)/%_spe.o : %.c
	$(CC) -DSPE_BUILD -o $@ -c $<

$(SUITE_OUT)/$(TEST)/%_spe.o : %.s
	$(AS) -o $@ $<

test.elf:
ifeq (${TOOLCHAIN}, GNUARM)
	$(LD) -Xlinker -Map=$(SUITE_OUT)/$(TEST)/test.map -o $(SUITE_OUT)/$(TEST)/test.elf -T$(SUITE_OUT)/.test.linker $(SUITE_OUT)/$(TEST)/*_nspe.o
else
	$(LD)  --scatter=$(SUITE_OUT)/.test.sct  --list=$(SUITE_OUT)/$(TEST)/test.map -o $(SUITE_OUT)/$(TEST)/test.elf  $(SUITE_OUT)/$(TEST)/*_nspe.o
endif
	$(DS)  $(SUITE_OUT)/$(TEST)/test.elf >  $(SUITE_OUT)/$(TEST)/test.disass

