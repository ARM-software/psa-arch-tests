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

INCLUDE= -I$(SOURCE)/val/common/ \
         -I$(SOURCE)/val/nspe/ \
         -I$(SOURCE)/val/spe/ \
         -I$(SOURCE)/platform/targets/$(TARGET)/nspe/ \
         -I$(BUILD)/platform/$(TARGET)/ \
         -I$(BUILD)/val/

VPATH=$(SOURCE)/val/common/:\
      $(SOURCE)/val/nspe/:\
      $(SOURCE)/val/spe/

SRC_COMMON=
SRC_NS= val_entry.c val_dispatcher.c val_framework.c val_crypto.c val_interfaces.c val_peripherals.c val_target.c val_protected_storage.c val_internal_trusted_storage.c val_attestation.c

all: build

build: mkdir build_common build_ns val_nspe.a

mkdir:
	@mkdir -p $(BUILD)/val/

build_common: $(SRC_COMMON:%.c=$(BUILD)/val/%.o)
build_ns: $(SRC_NS:%.c=$(BUILD)/val/%.o)

$(BUILD)/val/%.o : %.c
	$(CC)  $(INCLUDE) -DVAL_NSPE_BUILD  -o $@ -c $<

val_nspe.a:
	$(AR) $(AR_OPTIONS) $(BUILD)/val/val_nspe.a $(BUILD)/val/*.o

clean:
	@rm -rf $(BUILD)/val/*.o $(BUILD)/val/*.a
