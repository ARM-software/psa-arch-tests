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

# Local PAL Make variable
## PSA_IPC_IMPLEMENTED must be true for IPC SUITE
PSA_IPC_IMPLEMENTED:=1

## PSA_CRYPTO_IMPLEMENTED must be true for CRYPTO SUITE
PSA_CRYPTO_IMPLEMENTED:=1

# PAL C source files part of NSPE library
SRC_C_NSPE= pal_driver_ns_intf.c

ifeq (${PSA_IPC_IMPLEMENTED},1)
SRC_C_NSPE += pal_client_api_intf.c
SRC_C_NSPE += pal_driver_ipc_intf.c
else
SRC_C_NSPE += pal_client_api_empty_intf.c
SRC_C_NSPE += pal_driver_ns_intf.c pal_nvmem.c pal_uart.c pal_wd_cmsdk.c
endif

ifeq (${PSA_CRYPTO_IMPLEMENTED},1)
SRC_C_NSPE += pal_crypto_intf.c
else
SRC_C_NSPE += pal_crypto_empty_intf.c
endif

# PAL ASM source files part of NSPE library
SRC_ASM_NSPE=

# PAL C source files part of SPE library - driver partition
SRC_C_DRIVER_SP= pal_driver_intf.c pal_nvmem.c pal_uart.c pal_wd_cmsdk.c

# PAL ASM source files part of SPE library - driver partition
SRC_ASM_DRIVER_SP=


INCLUDE= -I$(SOURCE)/platform/targets/$(TARGET)/nspe \
         -I$(SOURCE)/platform/targets/$(TARGET)/spe \
         -I$(BUILD)/platform/$(TARGET)/ \
         -I$(SOURCE)/platform/drivers/uart/cmsdk \
         -I$(SOURCE)/platform/drivers/nvmem/ \
         -I$(SOURCE)/platform/drivers/watchdog/cmsdk

VPATH=$(SOURCE)/platform/targets/$(TARGET)/: \
      $(SOURCE)/platform/targets/$(TARGET)/spe: \
      $(SOURCE)/platform/targets/$(TARGET)/nspe: \
      $(SOURCE)/platform/drivers/uart/cmsdk: \
      $(SOURCE)/platform/drivers/nvmem: \
      $(SOURCE)/platform/drivers/watchdog/cmsdk: \

all: build

ifeq (${PSA_IPC_IMPLEMENTED},1)
build: mkdir build_nspe_pal build_spe_pal
else
build: mkdir build_nspe_pal
endif

mkdir:
	@mkdir -p $(BUILD)/platform/nspe/
	@mkdir -p $(BUILD)/platform/spe/

# BUILD NSPE PAL
build_nspe_pal: build_c_nspe build_asm_nspe pal_nspe.a

build_c_nspe: $(SRC_C_NSPE:%.c=$(BUILD)/platform/nspe/%.o)
build_asm_nspe: $(SRC_ASM_NSPE:%.s=$(BUILD)/platform/nspe/%.o)

$(BUILD)/platform/nspe/%.o : %.c
	$(CC) $(PAL_CDEFS) $(INCLUDE) -o $@ -c $<

$(BUILD)/platform/nspe/%.o : %.s
	$(AS) $(INCLUDE) -o $@ $<

pal_nspe.a:
	$(AR) $(AR_OPTIONS) $(BUILD)/platform/pal_nspe.a $(BUILD)/platform/nspe/*.o

# BUILD SPE PAL
build_spe_pal: build_driver_sp

build_driver_sp: build_c_driver_sp build_asm_driver_sp

build_c_driver_sp: $(SRC_C_DRIVER_SP:%.c=$(BUILD)/platform/spe/%_driver_sp.o)
build_asm_driver_sp: $(SRC_ASM_DRIVER_SP:%.s=$(BUILD)/platform/spe/%_driver_sp.o)

# Generated %_driver_sp.o(s) are used in spbuild.mk to create final driver_partition.a
$(BUILD)/platform/spe/%_driver_sp.o : %.c
	$(CC) $(INCLUDE) -o $@ -c $<

$(BUILD)/platform/spe/%_driver_sp.o : %.s
	$(AS) $(INCLUDE) -o $@ $<

clean:
	@rm -rf $(BUILD)/platform/nspe/* $(BUILD)/platform/spe/*.a
