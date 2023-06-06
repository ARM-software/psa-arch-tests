#/** @file
# * Copyright (c) 2023, Arm Limited or its affiliates. All rights reserved.
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

if(NOT DEFINED SECURE_DEBUG_ROOT)
    get_filename_component(SECURE_DEBUG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/.. ABSOLUTE)
endif()

include_directories(${SECURE_DEBUG_ROOT}/external/mbedtls/include)

set(MBEDTLS_FATAL_WARNINGS OFF CACHE BOOL "Disable fatal warnings" FORCE)
set(ENABLE_PROGRAMS OFF CACHE BOOL "Disable PROGRAMS" FORCE)
set(ENABLE_TESTING OFF CACHE BOOL "Disable TESTING" FORCE)
add_subdirectory(${SECURE_DEBUG_ROOT}/external/mbedtls mbedcrypto)

set_target_properties(mbedtls mbedx509 PROPERTIES
        EXCLUDE_FROM_ALL 1
        EXCLUDE_FROM_DEFAULT_BUILD 1)
