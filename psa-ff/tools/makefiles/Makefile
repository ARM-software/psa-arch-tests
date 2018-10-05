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

export SUITE_IN= $(SOURCE)/test_suites/$(SUITE)
export SUITE_OUT= $(BUILD)/$(SUITE)

include $(SOURCE)/tools/makefiles/toolchain.mk


all: clean target_cfg gen_linker build simulate

#Generate target files from User provided data base
target_cfg:
	mkdir -p $(BUILD)/platform/${TARGET}/
	@if [ ! -f "$(SOURCE)/platform/targets/$(TARGET)/target.cfg" ]; then { echo "Error: Target Not Found!!!"; exit 1; } fi
	perl $(SOURCE)/tools/scripts/targetConfigGen.pl $(SOURCE) $(BUILD) ${TARGET}

#Read target.cfg and update the addresses in linker script
gen_linker:
	mkdir -p $(SUITE_OUT)/;
	perl $(SOURCE)/tools/scripts/process_test_linker_file.pl $(SOURCE) $(SUITE_OUT) ${TARGET} $(TOOLCHAIN)


#Build framework archives and test_combine.elf
build:  pal_nspe.a val_nspe.a test_combine.elf

pal_nspe.a:
	@echo  "----------pal build start-------------"
	make -f $(SOURCE)/platform/nspe/Makefile
	@echo  "----------pal build complete-------------"

val_nspe.a:
	@echo  "----------val build start-------------"
	make -f $(SOURCE)/tools/makefiles/valbuild.mk
	@echo  "----------val build complete-------------"

test_combine.elf: test.elf
	perl $(SOURCE)/tools/scripts/test_elf_combine.pl $(SUITE) $(SUITE_OUT)/.testlist.txt
	hexdump -v -e ' 1/4 "%08X" "\n"' $(SUITE_OUT)/test_elf_combine.bin > $(SUITE_OUT)/test_elf_combine.hex

test.elf:
	@echo  "----------test build start-------------"
	@mkdir -p $(SUITE_OUT)/
	@$(eval TEST_LIST := $(shell grep "^test" $(SUITE_IN)/testsuite.db > $(SUITE_OUT)/.testlist.txt ; cat $(SUITE_OUT)/.testlist.txt))
	@$(foreach TEST,$(TEST_LIST), make -f $(SOURCE)/tools/makefiles/testbuild.mk  TEST=$(TEST) ;)
	@echo  "----------test build complete-------------"

simulate:

clean:
	@echo  ">>>> Cleaning the build directory..."
	rm -rf $(BUILD)/*
