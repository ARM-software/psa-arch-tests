#/** @file
# * Copyright (c) 2021 Arm Limited or its affiliates. All rights reserved.
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

if(NOT DEFINED PSA_ADAC_ROOT)
    get_filename_component(PSA_ADAC_ROOT ${CMAKE_SOURCE_DIR}/psa-adac ABSOLUTE)
endif()

# Listing all the sources from val
list(APPEND VAL_SRC_C
	${CMAKE_SOURCE_DIR}/val/src/val_entry.c
	${CMAKE_SOURCE_DIR}/val/src/val_adac.c
	${CMAKE_SOURCE_DIR}/val/src/val_dispatcher.c
	${CMAKE_SOURCE_DIR}/val/src/val_framework.c
	${CMAKE_SOURCE_DIR}/val/src/val_interfaces.c
	${CMAKE_SOURCE_DIR}/val/src/val_peripherals.c
)

# Create VAL library
add_library(${ADAC_HOST_VAL_LIB} STATIC ${VAL_SRC_C})

target_include_directories(${ADAC_HOST_VAL_LIB} PRIVATE
    ${CMAKE_SOURCE_DIR}/val/include
    ${CMAKE_BINARY_DIR}
    ${PSA_ADAC_ROOT}/psa-adac/core/include
    ${PSA_ADAC_ROOT}/psa-adac/sdm/include
    )

target_link_libraries(${ADAC_HOST_VAL_LIB} mbedcrypto)
