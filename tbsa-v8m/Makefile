#/** @file
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

# Toolchain
PREFIX:=arm-none-eabi-
CC:=$(PREFIX)gcc
CXX:=$(PREFIX)g++
OBJCOPY:=$(PREFIX)objcopy
OBJDUMP:=$(PREFIX)objdump
AS:=$(PREFIX)as

# Root folder
ROOT_DIR:=.

# Test specific macros
OUT_DIR  :=./out/$(TARGET)
VLIB     :=tbsa_val.a
PLIB     :=tbsa_pal.a
CLEAN_DIR:= $(dir $(wildcard $(ROOT_DIR)/platform/board/*/))
CLEAN_DIR:= $(filter-out ./, $(CLEAN_DIR))

SUITE :=

.PHONY: all clean

all: check_target check_arch clean gen_database gen_linker setup_cmsis build

check_target:
ifeq ($(TARGET),)
	@echo "TARGET not specified"
	@exit 1
endif
	@if [ ! -d "./platform/board/$(TARGET)/" ]; then { echo "Target Not Found!!!"; exit 1; } fi

check_arch:
ifeq ($(ARCH),)
ARCH := main
endif

VALINCDIR     :=val/include/
CFLAGS        :=-march=armv8-m.$(ARCH) -mcmse -mthumb -Wall -Werror -O0 -fdata-sections -ffunction-sections -I$(VALINCDIR)
ASFLAGS       :=-march=armv8-m.$(ARCH) -mthumb
LDFLAGS       :=\
               -Xlinker --fatal-warnings \
               -Xlinker --gc-sections \
               -z max-page-size=0x400 \
               -Xlinker -Map=tbsa.map

SYSLIBS       :=$(OUT_DIR)/$(VLIB) $(OUT_DIR)/$(PLIB) -lgcc -lc -lnosys

gen_database:
	@echo ""
	@echo "Reading target configuration file ..."
	mkdir -p platform/board/$(TARGET)/include
	./tools/tgt_cfg_parser/targetConfigGen.pl ./platform/board/$(TARGET)/tbsa_tgt.cfg platform/board/$(TARGET)/include/pal_database
	gcc -m32 targetConfigGen.c -w -o targetConfigGen -I$(VALINCDIR)
	./targetConfigGen
	rm -f ./targetConfigGen
	rm -f ./targetConfigGen.c

gen_linker:
	@echo ""
	@echo "Generating target specific linker scripts ..."
	cd tools/gen_linker_scripts/ && python gen_linker_scripts.py $(TARGET)
	@echo ""

setup_cmsis:
	@if [ ! -d "./platform/CMSIS_5" ]; then\
		cd platform && git clone https://github.com/ARM-software/CMSIS_5.git && cd -;\
		cd platform/CMSIS_5 && git checkout 49ac527aa7406cecbba46d4d3bdbc7f60c6c6d42;\
	else	echo "CMSIS already cloned"; echo "";\
	fi

build: output pal_lib $(VLIB) tbsa.bin

tbsa_val_src  :=$(wildcard val/src/*.c)
tbsa_val_src_s :=$(wildcard val/src/asm/val_secure_$(ARCH).s)
tbsa_val_src_s +=$(wildcard val/src/asm/val_pe_support.s)

tbsa_val_obj  :=$(tbsa_val_src:.c=.o)
tbsa_val_obj  +=$(tbsa_val_src_s:.s=.o)

output:
	mkdir -p $(OUT_DIR)

pal_lib:
	cd platform/board/$(TARGET) && make ODIR=$(OUT_DIR) LIBNAME=$(PLIB) ENV=$(ENV) ARCH=$(ARCH)

$(VLIB): $(tbsa_val_obj)
	$(AR) -rc $(OUT_DIR)/$@ $^
	find $(ROOT_DIR) -iname '*.o' -delete

clean:
	rm -rf $(OUT_DIR)
	find $(ROOT_DIR) -iname '*.o' -delete
	cd test_pool && make clean
	echo $(CLEAN_DIR)
	$(foreach d, $(CLEAN_DIR), rm -rf $dinclude/;)
	$(foreach d, $(CLEAN_DIR), rm -rf $dlinker/;)

tbsa.bin:tbsa.elf
	$(OBJCOPY) $< -O binary $@
	mv tbsa.asm $(OUT_DIR)
	mv tbsa.bin $(OUT_DIR)
	mv tbsa.elf $(OUT_DIR)
	mv tbsa.hex $(OUT_DIR)
	mv tbsa.map $(OUT_DIR)
	cd test_pool && make VLIB=$(OUT_DIR)/$(VLIB) PLIB=$(OUT_DIR)/$(PLIB) ARCH=$(ARCH)

tbsa.elf:tbsa_app/tbsa_main.c
	$(CC) $(CFLAGS) -Tplatform/board/$(TARGET)/linker/tbsa.linker $(LDFLAGS) -o $@ tbsa_app/tbsa_main.c $(SYSLIBS)
	$(OBJDUMP) -d $@ > tbsa.asm
	hexdump -v -e ' 1/4 "%08X" "\n"' tbsa.elf > tbsa.hex
