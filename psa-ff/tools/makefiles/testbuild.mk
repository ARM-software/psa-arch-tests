# * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
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
         -I$(SOURCE)/platform/nspe/ \
         -I$(SUITE_IN)/$(TEST)/\
         -I$(SUITE_IN)/include/

VPATH=$(SOURCE)/val/common/:\
      $(SOURCE)/val/nspe/:\
      $(SUITE_IN)/$(TEST)/:\
      $(SUITE_IN)/include/



all:  mkdir compile_c compile_asm test.elf

mkdir:
	mkdir -p $(SUITE_OUT)/$(TEST)/

compile_c: $(CC_SOURCE:%.c=$(SUITE_OUT)/$(TEST)/%.o)
compile_asm: $(AS_SOURCE:%.s=$(SUITE_OUT)/$(TEST)/%.o)

$(SUITE_OUT)/$(TEST)/%.o : %.c
	$(CC) -D NONSECURE_TEST_BUILD -o $@ -c $<

$(SUITE_OUT)/$(TEST)/%.o : %.s
	$(AS) -o $@ $<

test.elf:
	$(LD) -Xlinker -Map=$(SUITE_OUT)/$(TEST)/test.map -o $(SUITE_OUT)/$(TEST)/test.elf -T$(SUITE_OUT)/.test.linker $(SUITE_OUT)/$(TEST)/*.o
	$(DS)  $(SUITE_OUT)/$(TEST)/test.elf >  $(SUITE_OUT)/$(TEST)/test.disass

