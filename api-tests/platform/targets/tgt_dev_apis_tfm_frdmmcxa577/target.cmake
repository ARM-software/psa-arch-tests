#/** @file
# * Copyright (c) 2026, Arm Limited or its affiliates. All rights reserved.
# * SPDX-License-Identifier : Apache-2.0
# *
# * Copyright 2026 NXP
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

# No watchdog is configured for this platform.
set(WATCHDOG_AVAILABLE 0)

# PAL C source files part of NSPE library
list(APPEND PAL_SRC_C_NSPE )

# PAL ASM source files part of NSPE library
list(APPEND PAL_SRC_ASM_NSPE )

# PAL C source files part of SPE library - driver partition
list(APPEND PAL_SRC_C_DRIVER_SP )

# PAL ASM source files part of SPE library - driver partition
list(APPEND PAL_SRC_ASM_DRIVER_SP )

# Now this is a major hack, but there is no way to get a 
# platform specific header file (this platform overrides 
# tfm_builtin_key_ids.h) included here. Even if the header 
# file is part of TF-M's install interface, this is still 
# not an option as the test suite typically is part of the 
# TF-M build itself which means the install has not yet 
# happened and the interface files are only at their source 
# location but not at the install location.
# So we need that platform specific include path here which 
# depends on the file structure in TF-M (which probalby is 
# reasonably stable at this point)...
list(APPEND PSA_INCLUDE_PATHS ${CMAKE_SOURCE_DIR}/platform/ext/target/nxp/frdmmcxa577/Device/Include/)

# Listing all the sources required for given target
if(${SUITE} STREQUAL "IPC")
	list(APPEND PAL_SRC_C_NSPE
		# driver functionalities are implemented as RoT-services
		# and secure and non-secure clients will call to these RoT-services to get appropriate driver services.
		${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe/common/pal_client_api_intf.c
		${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe/common/pal_driver_ipc_intf.c
	)
	list(APPEND PAL_SRC_C_DRIVER_SP
		# Driver files will be compiled as part of driver partition
		${PSA_ROOT_DIR}/platform/targets/${TARGET}/spe/pal_driver_intf.c
		${PSA_ROOT_DIR}/platform/drivers/nvmem/pal_nvmem.c
		${PSA_ROOT_DIR}/platform/drivers/uart/pl011/pal_uart.c
		${PSA_ROOT_DIR}/platform/drivers/watchdog/cmsdk/pal_wd_cmsdk.c
	)
else()
	list(APPEND PAL_SRC_C_NSPE
		# driver files will be compiled as part of NSPE
		${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe/pal_driver_intf.c
		${PSA_ROOT_DIR}/platform/drivers/nvmem/pal_nvmem.c
		${PSA_ROOT_DIR}/platform/drivers/uart/pl011/pal_uart.c
		${PSA_ROOT_DIR}/platform/drivers/watchdog/cmsdk/pal_wd_cmsdk.c
	)
endif()

if(${SUITE} STREQUAL "CRYPTO")
	list(APPEND PAL_SRC_C_NSPE
		${PSA_ROOT_DIR}/platform/targets/common/nspe/crypto/pal_crypto_intf.c
	)
endif()
if((${SUITE} STREQUAL "PROTECTED_STORAGE") OR (${SUITE} STREQUAL "STORAGE"))
	list(APPEND PAL_SRC_C_NSPE
		${PSA_ROOT_DIR}/platform/targets/common/nspe/protected_storage/pal_protected_storage_intf.c
	)
endif()
if((${SUITE} STREQUAL "INTERNAL_TRUSTED_STORAGE") OR (${SUITE} STREQUAL "STORAGE"))
	list(APPEND PAL_SRC_C_NSPE
		${PSA_ROOT_DIR}/platform/targets/common/nspe/internal_trusted_storage/pal_internal_trusted_storage_intf.c
	)
endif()
if(${SUITE} STREQUAL "INITIAL_ATTESTATION")
	list(APPEND PAL_SRC_C_NSPE
		${PSA_ROOT_DIR}/platform/targets/common/nspe/initial_attestation/pal_attestation_intf.c
		${PSA_ROOT_DIR}/platform/targets/common/nspe/initial_attestation/pal_attestation_crypto.c
                ${PSA_TARGET_QCBOR}/src/UsefulBuf.c
                ${PSA_TARGET_QCBOR}/src/ieee754.c
                ${PSA_TARGET_QCBOR}/src/qcbor_decode.c
                ${PSA_TARGET_QCBOR}/src/qcbor_encode.c
	)
endif()

# Create NSPE library
add_library(${PSA_TARGET_PAL_NSPE_LIB} STATIC ${PAL_SRC_C_NSPE} ${PAL_SRC_ASM_NSPE})

# PSA Include directories
foreach(psa_inc_path ${PSA_INCLUDE_PATHS})
	target_include_directories(${PSA_TARGET_PAL_NSPE_LIB} PRIVATE ${psa_inc_path})
endforeach()

list(APPEND PAL_DRIVER_INCLUDE_PATHS
	${PSA_ROOT_DIR}/platform/drivers/nvmem
	${PSA_ROOT_DIR}/platform/drivers/uart/pl011
	${PSA_ROOT_DIR}/platform/drivers/watchdog/cmsdk
)

if(${SUITE} STREQUAL "IPC")
    # driver files will be compiled as part of NSPE
    target_include_directories(${PSA_TARGET_DRIVER_PARTITION_LIB} PRIVATE
        ${PAL_DRIVER_INCLUDE_PATHS}
        ${PSA_ROOT_DIR}/platform/targets/common/nspe
        ${PSA_ROOT_DIR}/platform/targets/common/nspe/crypto
        ${PSA_ROOT_DIR}/platform/targets/common/nspe/protected_storage
        ${PSA_ROOT_DIR}/platform/targets/common/nspe/internal_trusted_storage
        ${PSA_ROOT_DIR}/platform/targets/common/nspe/initial_attestation
    )
else()
    # driver files will be compiled as part of NSPE
    target_include_directories(${PSA_TARGET_PAL_NSPE_LIB} PRIVATE
        ${PAL_DRIVER_INCLUDE_PATHS}
        ${PSA_ROOT_DIR}/platform/targets/common/nspe
        ${PSA_ROOT_DIR}/platform/targets/common/nspe/crypto
        ${PSA_ROOT_DIR}/platform/targets/common/nspe/protected_storage
        ${PSA_ROOT_DIR}/platform/targets/common/nspe/internal_trusted_storage
        ${PSA_ROOT_DIR}/platform/targets/common/nspe/initial_attestation
        ${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe
    )
endif()

if(${SUITE} STREQUAL "INITIAL_ATTESTATION")
target_include_directories(${PSA_TARGET_PAL_NSPE_LIB} PRIVATE
	${PSA_QCBOR_INCLUDE_PATH}
)
endif()
