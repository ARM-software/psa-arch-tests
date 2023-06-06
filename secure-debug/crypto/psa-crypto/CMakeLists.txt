# @file
# Copyright (c) 2021-2023, Arm Limited or its affiliates. All rights reserved.
# SPDX-License-Identifier : Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

if(NOT DEFINED SECURE_DEBUG_ROOT)
    get_filename_component(SECURE_DEBUG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../.. ABSOLUTE)
endif()

if(NOT DEFINED PSA_ADAC_ROOT)
    get_filename_component(PSA_ADAC_ROOT ${SECURE_DEBUG_ROOT}/psa-adac ABSOLUTE)
endif()

include_directories(${PSA_ADAC_ROOT}/psa-adac/core/include)
include(${SECURE_DEBUG_ROOT}/crypto/mbedcrypto.cmake)

add_library(psa_adac_psa_crypto STATIC)

set(PSA_CRYPTO_SRC
        ${SECURE_DEBUG_ROOT}/crypto/psa-crypto/adac_crypto_psa.c
        ${SECURE_DEBUG_ROOT}/crypto/psa-crypto/adac_crypto_psa_hash.c
        ${SECURE_DEBUG_ROOT}/crypto/psa-crypto/adac_crypto_psa_mac.c
        ${SECURE_DEBUG_ROOT}/crypto/psa-crypto/adac_crypto_psa_pk.c)

target_sources(psa_adac_psa_crypto PUBLIC ${PSA_CRYPTO_SRC})
