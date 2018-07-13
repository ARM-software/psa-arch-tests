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

.PHONY: all clean

# Test specific macros
OUT_DIR:=./out

# Root folder
ROOT_DIR:=.

DIRECTORY := $(dir $(wildcard $(ROOT_DIR)/*/))
DIRECTORY := $(filter-out ./, $(DIRECTORY))
DIRECTORY := $(filter-out $(OUT_DIR)/, $(DIRECTORY))
DIRECTORY := $(sort $(filter-out ./include/, $(DIRECTORY)))
CLEAN_DIRECTORY :=$(DIRECTORY)

comma := ,

all: check build output
	./tbsa_elf_combine.pl $(ROOT_DIR)/
	mv ./tbsa_test_combined.bin $(OUT_DIR)/.
	hexdump -v -e ' 1/4 "%08X" "\n"' $(OUT_DIR)/tbsa_test_combined.bin > $(OUT_DIR)/tbsa_test_combined.hex

check:
ifneq ($(SUITE), )
DIRECTORY := $(subst $(comma), , $(SUITE))
endif

output:
	mkdir -p $(OUT_DIR)

build:
	$(foreach d, $(DIRECTORY), cd $d && $(MAKE) && cd ..;)

clean:
	$(foreach d, $(CLEAN_DIRECTORY), cd $d && $(MAKE) clean && cd ..;)
	rm -rf $(OUT_DIR)
