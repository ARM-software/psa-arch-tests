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

INCLUDE= -I$(SOURCE)/val/common/ \
         -I$(SOURCE)/val/nspe/ \
         -I$(SOURCE)/val/spe/ \
         -I$(SOURCE)/ \
         -I$(SOURCE)/ff/partition/common/ \
         -I$(SOURCE)/ff/partition/ipc/ \
         -I$(BUILD)/partition/ \
         -I$(BUILD)/platform/$(TARGET)/

VPATH=$(SOURCE)/val/common/:\
      $(SOURCE)/val/nspe/:\
      $(SOURCE)/val/spe/:\
      $(SOURCE)/ff/partition/common/: \
      $(SOURCE)/ff/partition/ipc/


CC_SOURCE += driver_partition.c val_driver_service_apis.c

ifeq (${SUITE}, ipc)
CC_SOURCE += client_partition.c server_partition.c
all:  mkdir compile_c compile_asm driver_partition.a client_partition.a server_partition.a
else
all:  mkdir compile_c compile_asm driver_partition.a
endif


mkdir:
	@mkdir -p $(BUILD)/partition/

compile_c: $(CC_SOURCE:%.c=$(BUILD)/partition/%.o)
compile_asm: $(AS_SOURCE:%.s=$(BUILD)/partition/%.o)

$(BUILD)/partition/%.o : %.c
	$(CC) -o $@ -c $<

$(BUILD)/partition/%.o : %.s
	$(AS) -o $@ $<

client_partition.a:
	$(AR) $(AR_OPTIONS) $(BUILD)/partition/client_partition.a $(BUILD)/partition/client_partition.o $(SUITE_OUT)/test*/test_i*_spe.o

server_partition.a:
	$(AR) $(AR_OPTIONS) $(BUILD)/partition/server_partition.a $(BUILD)/partition/server_partition.o $(SUITE_OUT)/test*/test_supp_*_spe.o

driver_partition.a:
	$(AR) $(AR_OPTIONS) $(BUILD)/partition/driver_partition.a $(BUILD)/platform/spe/*_driver_sp.o $(BUILD)/partition/driver_partition.o $(BUILD)/partition/val_driver_service_apis.o
