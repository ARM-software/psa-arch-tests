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

TOOLCHAIN=GNUARM
PREFIX=

#### GNUARM OPTIONS - START ####
ifneq (,$(findstring $(TOOLCHAIN),GNUARM-GCC))
    ifeq (${TOOLCHAIN}, GNUARM)
    PREFIX=arm-none-eabi-
    endif

    ifeq (${CPU_ARCH}, armv7m)
    TARGET_SWITCH= -march=armv7-m
    else
        ifeq (${CPU_ARCH}, armv8m_ml)
        TARGET_SWITCH= -march=armv8-m.main -mcmse
        else
        TARGET_SWITCH= -march=armv8-m.base -mcmse
        endif
    endif

COMPILER= $(PREFIX)gcc
ASSEMBLER= $(PREFIX)as
AR= $(PREFIX)ar
LINKER= $(PREFIX)gcc
OBJDUMP=$(PREFIX)objdump

COMPILER_OPTIONS= $(TARGET_SWITCH) -Wall -Werror -fdata-sections -ffunction-sections -mno-unaligned-access

ASSEMBLER_OPTIONS= $(TARGET_SWITCH) -mthumb
AR_OPTIONS= -rcs
LINKER_OPTIONS= $(TARGET_SWITCH) -mthumb -Wall -Werror -O0 -fdata-sections \
				-ffunction-sections -Xlinker --fatal-warnings -Xlinker --gc-sections \
				-z max-page-size=0x400 -lgcc -lc -lnosys
OBJDUMP_OPTIONS= -d
endif #GNUARM-GCC
#### GNUARM OPTIONS - END ####

#### ARMCLANG OPTIONS - START ####
ifeq (${TOOLCHAIN}, ARMCLANG)

COMPILER= armclang
ASSEMBLER= armclang
AR= armar
LINKER= armlink
OBJDUMP=fromelf

    ifeq (${CPU_ARCH}, armv7m)
    TARGET_SWITCH= -march=armv7-m
    TARGET_SWITCH_LD= --cpu=7-M
    else
        ifeq (${CPU_ARCH}, armv8m_ml)
        TARGET_SWITCH= -march=armv8-m.main -mcmse
        TARGET_SWITCH_LD= --cpu=8-M.Main
        else
        TARGET_SWITCH= -march=armv8-m.base -mcmse
        TARGET_SWITCH_LD= --cpu=8-M.Base
        endif
    endif

COMPILER_OPTIONS= --target=arm-arm-none-eabi $(TARGET_SWITCH) -Wall -Werror -fshort-enums -fshort-wchar -funsigned-char -fdata-sections -ffunction-sections -mno-unaligned-access -mfpu=none
AR_OPTIONS= --create -cr
LINKER_OPTIONS= --strict --map --symbols --xref  --info=summarysizes,sizes,totals,unused,veneers --diag_warning=L6204
OBJDUMP_OPTIONS= -c -d --datasymbols
endif
#### ARMCLANG OPTIONS - END ####

COMPILER_OPTIONS += -DVERBOSE=$(VERBOSE)

ifeq (${TEST_COMBINE_ARCHIVE}, 1)
COMPILER_OPTIONS += -DTEST_COMBINE_ARCHIVE=1
endif

ifeq (${WATCHDOG_AVAILABLE}, 1)
COMPILER_OPTIONS += -DWATCHDOG_AVAILABLE=1
endif

ifeq (${SP_HEAP_MEM_SUPP}, 1)
COMPILER_OPTIONS += -DSP_HEAP_MEM_SUPP=1
endif

ifeq (${PSA_IPC_IMPLEMENTED}, 1)
COMPILER_OPTIONS += -DPSA_IPC_IMPLEMENTED=1
endif

ifeq (${PSA_CRYPTO_IMPLEMENTED}, 1)
COMPILER_OPTIONS += -DPSA_CRYPTO_IMPLEMENTED=1
endif

ifeq (${PSA_PROTECTED_STORAGE_IMPLEMENTED}, 1)
COMPILER_OPTIONS += -DPSA_PROTECTED_STORAGE_IMPLEMENTED=1
endif

ifeq (${PSA_INTERNAL_TRUSTED_STORAGE_IMPLEMENTED}, 1)
COMPILER_OPTIONS += -DPSA_INTERNAL_TRUSTED_STORAGE_IMPLEMENTED=1
endif

ifeq (${PSA_INITIAL_ATTESTATION_IMPLEMENTED}, 1)
COMPILER_OPTIONS += -DPSA_INITIAL_ATTESTATION_IMPLEMENTED=1
endif

CC= $(COMPILER) $(COMPILER_OPTIONS) $(CC_OPTIONS) $(USER_INCLUDE) $(INCLUDE)
AS= $(ASSEMBLER) $(ASSEMBLER_OPTIONS) $(AS_OPTIONS)
LD= $(LINKER) $(LINKER_OPTIONS)
DS= $(OBJDUMP) $(OBJDUMP_OPTIONS)
