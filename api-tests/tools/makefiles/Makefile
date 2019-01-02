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

ifeq (${SUITE}, ipc)
SUITE_DIR = ff/$(SUITE)
else
SUITE_DIR = dev_apis/$(SUITE)
endif

export SUITE_IN= $(SOURCE)/$(SUITE_DIR)
export SUITE_OUT= $(BUILD)/$(SUITE_DIR)

include $(SOURCE)/tools/makefiles/toolchain.mk


all: clean target_cfg gen_linker process_testsuite.db build

#Generate target files from User provided data base
target_cfg:
	mkdir -p $(BUILD)/platform/${TARGET}/
	@if [ ! -f "$(SOURCE)/platform/targets/$(TARGET)/target.cfg" ]; then { echo "Error: Target Not Found!!!"; exit 1; } fi
	perl $(SOURCE)/tools/scripts/targetConfigGen.pl $(SOURCE) $(BUILD) ${TARGET}

#Read target.cfg and update the addresses in linker script
gen_linker:
	mkdir -p $(SUITE_OUT)/ $(BUILD)/val/ $(BUILD)/partition/ ;
	perl $(SOURCE)/tools/scripts/process_test_linker_file.pl $(SOURCE) $(SUITE_OUT) ${TARGET} $(TOOLCHAIN)

process_testsuite.db:
	$(eval TEST_LIST := $(shell grep "^test" $(SUITE_IN)/testsuite.db > $(SUITE_OUT)/.testlist.txt ; cat $(SUITE_OUT)/.testlist.txt))
	perl $(SOURCE)/tools/scripts/gen_tests_list.pl $(BUILD) $(SUITE_OUT)/.testlist.txt $(SUITE)


#Build framework archives and test_combine.elf
build:  build_pal val_nspe.a test_combine.elf partition_build output_list

build_pal:
	@echo  ""
	@echo  "----------pal build start-------------"
	make -f $(SOURCE)/platform/targets/$(TARGET)/Makefile
	@echo  "----------pal build complete-------------"

val_nspe.a:
	@echo  ""
	@echo  "----------val build start-------------"
	make -f $(SOURCE)/tools/makefiles/valbuild.mk
	@echo  "----------val build complete-------------"

test_combine.elf: test.elf
	perl $(SOURCE)/tools/scripts/test_elf_combine.pl $(SUITE_OUT)/.testlist.txt
	hexdump -v -e ' 1/4 "%08X" "\n"' $(SUITE_OUT)/test_elf_combine.bin > $(SUITE_OUT)/test_elf_combine.hex
	$(AR) $(AR_OPTIONS) $(SUITE_OUT)/test_combine.a $(SUITE_OUT)/test*/test_*_nspe.o

test.elf:
	@echo  ""
	@echo  "----------test build start-------------"
	@$(foreach TEST,$(TEST_LIST), make -f $(SOURCE)/tools/makefiles/testbuild.mk  TEST=$(TEST) ;)
	@echo  "----------test build complete-------------"

partition_build:
ifeq (${PSA_IPC_IMPLEMENTED}, 1)
	@echo  ""
	@echo  "----------test partition build start-------------"
	make -f $(SOURCE)/tools/makefiles/spbuild.mk
	@echo  "----------test partition build complete-------------"
endif

output_list:
	@echo  ""
	@echo  "Below are the list of output binaries/libraries. Integrate these bins/libs"
	@echo  "to your software stack to execute test suite."
	@echo  ""
	@echo  "a) NSPE files:"
	@echo  " $(BUILD)/val/val_nspe.a"
	@echo  " $(BUILD)/platform/pal_nspe.a"
ifeq (${TEST_COMBINE_ARCHIVE}, 1)
	@echo  " $(SUITE_OUT)/test_combine.a"
else
	@echo  " $(SUITE_OUT)/test_elf_combine.bin"
endif
	@echo  ""
ifeq (${SUITE}, ipc)
	@echo  "b) SPE files"
	@echo  " $(BUILD)/partition/driver_partition.a"
	@echo  " $(BUILD)/partition/client_partition.a"
	@echo  " $(BUILD)/partition/server_partition.a"
	@echo  ""
endif

clean:
	@echo  ">>>> Cleaning the build directory..."
	rm -rf $(BUILD)/*
