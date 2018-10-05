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

# Add C source files for compilation
SRC_C= pal_baremetal_ns_intf.c pal_crypto_intf.c

# Add ASM source files for compilation
SRC_ASM=

INCLUDE= -I$(SOURCE)/platform/targets/$(TARGET)/ \
         -I$(BUILD)/platform/$(TARGET)/ \
         -I$(SOURCE)/platform/nspe/drivers/ \
         -I$(SOURCE)/platform/nspe/

VPATH=$(SOURCE)/platform/targets/$(TARGET)/: \
      $(SOURCE)/platform/nspe/drivers/: \
      $(SOURCE)/platform/nspe/: \

all: build

build: mkdir build_c build_asm pal_nspe.a

mkdir:
	@mkdir -p $(BUILD)/platform/nspe/

build_c: $(SRC_C:%.c=$(BUILD)/platform/nspe/%.o)
build_asm: $(SRC_ASM:%.s=$(BUILD)/platform/nspe/%.o)

$(BUILD)/platform/nspe/%.o : %.c
	$(CC) $(INCLUDE) -o $@ -c $<

$(BUILD)/platform/nspe/%.o : %.s
	$(AS) $(INCLUDE) -o $@ $<

pal_nspe.a:
	$(AR) $(AR_OPTIONS) $(BUILD)/platform/pal_nspe.a $(BUILD)/platform/nspe/*.o

clean:
	@rm -rf $(BUILD)/platform/nspe/*.o $(BUILD)/platform/nspe/*.a
