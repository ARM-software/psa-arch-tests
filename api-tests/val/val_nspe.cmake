#/** @file
# * Copyright (c) 2019, Arm Limited or its affiliates. All rights reserved.
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

# Listing all the sources from val
list(APPEND VAL_SRC_C_NSPE
	${PSA_ROOT_DIR}/val/nspe/val_entry.c
	${PSA_ROOT_DIR}/val/nspe/val_dispatcher.c
	${PSA_ROOT_DIR}/val/nspe/val_framework.c
	${PSA_ROOT_DIR}/val/nspe/val_crypto.c
	${PSA_ROOT_DIR}/val/nspe/val_interfaces.c
	${PSA_ROOT_DIR}/val/nspe/val_peripherals.c
	${PSA_ROOT_DIR}/val/common/val_target.c
	${PSA_ROOT_DIR}/val/nspe/val_protected_storage.c
	${PSA_ROOT_DIR}/val/nspe/val_internal_trusted_storage.c
	${PSA_ROOT_DIR}/val/nspe/val_attestation.c
)

# Create VAL NSPE library
add_library(${PSA_TARGET_VAL_NSPE_LIB} STATIC ${VAL_SRC_C_NSPE})


# PSA Include directories
foreach(psa_inc_path ${PSA_INCLUDE_PATHS})
        target_include_directories(${PSA_TARGET_VAL_NSPE_LIB} PRIVATE ${psa_inc_path})
endforeach()

if(${SUITE} STREQUAL "INITIAL_ATTESTATION")
target_include_directories(${PSA_TARGET_VAL_NSPE_LIB} PRIVATE
    ${PSA_QCBOR_INCLUDE_PATH}
)
endif()

target_include_directories(${PSA_TARGET_VAL_NSPE_LIB} PRIVATE
	${CMAKE_CURRENT_BINARY_DIR}
	${PSA_ROOT_DIR}/val/common
	${PSA_ROOT_DIR}/val/nspe
	${PSA_ROOT_DIR}/val/spe
	${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe
	${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe/common
	${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe/crypto
)

if(${WATCHDOG_AVAILABLE} EQUAL 1)
	target_compile_definitions(${PSA_TARGET_VAL_NSPE_LIB} PRIVATE WATCHDOG_AVAILABLE)
endif()
if(${TEST_COMBINE_ARCHIVE} EQUAL 1)
	target_compile_definitions(${PSA_TARGET_VAL_NSPE_LIB} PRIVATE TEST_COMBINE_ARCHIVE)
endif()
target_compile_definitions(${PSA_TARGET_VAL_NSPE_LIB} PRIVATE VAL_NSPE_BUILD)
target_compile_definitions(${PSA_TARGET_VAL_NSPE_LIB} PRIVATE TEST_COMBINE_ARCHIVE)
