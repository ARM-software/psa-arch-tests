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

# Root folder
ROOT_DIR:=.

DIRECTORY := $(dir $(wildcard $(ROOT_DIR)/*/))
DIRECTORY := $(filter-out ./, $(DIRECTORY))
DIRECTORY := $(sort $(filter-out ./include/, $(DIRECTORY)))
CLEAN_DIRECTORY :=$(DIRECTORY)

comma := ,

all: check build
	./tbsa_elf_combine.pl $(ROOT_DIR)/
	mv ./tbsa_test_combined.bin ../out/$(TARGET)/.
	hexdump -v -e ' 1/4 "%08X" "\n"' ../out/$(TARGET)/tbsa_test_combined.bin > ../out/$(TARGET)/tbsa_test_combined.hex

check:
ifneq ($(SUITE), )
DIRECTORY := $(subst $(comma), , $(SUITE))
endif

build:
	$(foreach d, $(DIRECTORY), cd $d && $(MAKE) TARGET=$(TARGET) && cd ..;)

clean:
	$(foreach d, $(CLEAN_DIRECTORY), cd $d && $(MAKE) clean && cd ..;)
