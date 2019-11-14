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

###############################################################################
# FUNCTION: _create_psa_storage_exe()
#  Function for generating PSA Storage test binaries. This function requires
#  PSA_STORAGE_LIB_FILENAME to be specificed on the cmake command line,
#  where the symbol is defined as the full path to the external PSA storage
#  library to test. e.g.
#    cmake ... -DPSA_STORAGE_LIB_FILENAME=/wdir/usr/lib/libpsastorage.so
# ARGUMENTS:
#   _exe_name     Name of the test binary to generate
#   _api_dir      PSA storage API directory name e.g. internal_trusted_storage
#                 or protected_storage
###############################################################################
function(_create_psa_storage_exe _exe_name _api_dir)

	if(NOT DEFINED PSA_STORAGE_LIB_FILENAME)
		message(FATAL_ERROR "ERROR: PSA_STORAGE_LIB_FILENAME undefined.")
	endif()

	# Create the PSA PS Storage test binary.
	set(EXE_NAME ${_exe_name})

	# Define PSA_LIB_NAME to be the name of the PSA Storage library to be tested.
	get_filename_component(PSA_STORAGE_LIB_NAME ${PSA_STORAGE_LIB_FILENAME} NAME [CACHE])
	set(PSA_LIB_NAME ${PSA_STORAGE_LIB_NAME})

	# The path to the PSA Storage libpsastorage.so (external to this project)
	# is specified on the cmake command line with the PSA_STORAGE_LD_LIBRARY_PATH
	# symbol, and used as the the link directory.
	get_filename_component(PSA_STORAGE_LIB_DIR ${PSA_STORAGE_LIB_FILENAME} DIRECTORY [CACHE])
	link_directories(${PSA_STORAGE_LIB_DIR})

	# Create list of test binary source files.
	list(APPEND EXE_SRC ${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe/common/main.c)

	# Create list of libraries to link to test binary
	list(APPEND EXE_LIBS
		${PROJECT_BINARY_DIR}/dev_apis/${_api_dir}/test_combine.a
		${PROJECT_BINARY_DIR}/val/val_nspe.a
		${PROJECT_BINARY_DIR}/platform/pal_nspe.a
		${PROJECT_BINARY_DIR}/dev_apis/${_api_dir}/test_combine.a
		${PSA_LIB_NAME}
	)

	add_executable(${EXE_NAME} ${EXE_SRC})
	target_link_libraries(${EXE_NAME} ${EXE_LIBS})
endfunction(_create_psa_storage_exe)

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
	list(APPEND PAL_SRC_C_NSPE
		${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe/protected_storage/pal_protected_storage_intf.c
	)
	_create_psa_storage_exe(psa-arch-tests-ps protected_storage)
endif()
if(${SUITE} STREQUAL "INTERNAL_TRUSTED_STORAGE")
	list(APPEND PAL_SRC_C_NSPE
		${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe/internal_trusted_storage/pal_internal_trusted_storage_intf.c
	)
	_create_psa_storage_exe(psa-arch-tests-its internal_trusted_storage)
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
