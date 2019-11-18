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

# PAL C source files part of NSPE library
list(APPEND PAL_SRC_C_NSPE )

# PAL ASM source files part of NSPE library
list(APPEND PAL_SRC_ASM_NSPE )

# PAL C source files part of SPE library - driver partition
list(APPEND PAL_SRC_C_DRIVER_SP )

# PAL ASM source files part of SPE library - driver partition
list(APPEND PAL_SRC_ASM_DRIVER_SP )


# Listing all the sources required for given target
if(${SUITE} STREQUAL "IPC")
	message(FATAL_ERROR "IPC not supported")
else()
	list(APPEND PAL_SRC_C_NSPE
		# driver files will be compiled as part of NSPE
		${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe/common/pal_client_api_empty_intf.c
		${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe/common/pal_driver_ns_intf.c
	)
endif()
if(${SUITE} STREQUAL "CRYPTO")
	list(APPEND PAL_SRC_C_NSPE
		${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe/crypto/pal_crypto_intf.c
	)
endif()
if(${SUITE} STREQUAL "PROTECTED_STORAGE")
	message(FATAL_ERROR "Protected Storage not supported")
endif()
if(${SUITE} STREQUAL "INTERNAL_TRUSTED_STORAGE")
	message(FATAL_ERROR "Internal Trusted Storage not support")
endif()
if(${SUITE} STREQUAL "INITIAL_ATTESTATION")
	message(FATAL_ERROR "Initial attestation not supported")
endif()

# Create NSPE library
add_library(${PSA_TARGET_PAL_NSPE_LIB} STATIC ${PAL_SRC_C_NSPE} ${PAL_SRC_ASM_NSPE})

# PSA Include directories
foreach(psa_inc_path ${PSA_INCLUDE_PATHS})
	target_include_directories(${PSA_TARGET_PAL_NSPE_LIB} PRIVATE ${psa_inc_path})
endforeach()

target_include_directories(${PSA_TARGET_PAL_NSPE_LIB} PRIVATE
	${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe/common
	${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe/crypto
	${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe/protected_storage
	${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe/internal_trusted_storage
	${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe/initial_attestation
)

if(${SUITE} STREQUAL "INITIAL_ATTESTATION")
target_include_directories(${PSA_TARGET_PAL_NSPE_LIB} PRIVATE
	${PSA_QCBOR_INCLUDE_PATH}
)
endif()
