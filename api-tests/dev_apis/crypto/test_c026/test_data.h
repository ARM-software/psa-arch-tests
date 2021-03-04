/** @file
 * Copyright (c) 2019-2021, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#include "test_crypto_common.h"

typedef struct {
    char                    test_desc[75];
    psa_key_type_t          type;
    const uint8_t          *data;
    size_t                  data_length;
    psa_key_usage_t         usage_flags;
    psa_algorithm_t         alg;
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_HMAC
#ifdef ARCH_TEST_SHA256
{
    .test_desc       = "Test psa_mac_sign_setup - HMAC - SHA256\n",
    .type            = PSA_KEY_TYPE_HMAC,
    .data            = key_data,
    .data_length     = 64,
    .usage_flags     = PSA_KEY_USAGE_SIGN_MESSAGE,
    .alg             = PSA_ALG_HMAC(PSA_ALG_SHA_256),
    .expected_status = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_AES_128
#ifdef ARCH_TEST_CMAC
{
    .test_desc       = "Test psa_mac_sign_setup - CMAC - AES\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_SIGN_MESSAGE,
    .alg             = PSA_ALG_CMAC,
    .expected_status = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_HMAC
#ifdef ARCH_TEST_CMAC
{
    .test_desc       = "Test psa_mac_sign_setup - Incompatible HMAC for CMAC\n",
    .type            = PSA_KEY_TYPE_HMAC,
    .data            = key_data,
    .data_length     = 64,
    .usage_flags     = PSA_KEY_USAGE_SIGN_MESSAGE,
    .alg             = PSA_ALG_CMAC,
    .expected_status = PSA_ERROR_NOT_SUPPORTED
},
#endif
#endif

#ifdef ARCH_TEST_HMAC
#ifdef ARCH_TEST_SHA256
{
    .test_desc       = "Test psa_mac_sign_setup - Invalid usage flag\n",
    .type            = PSA_KEY_TYPE_HMAC,
    .data            = key_data,
    .data_length     = 64,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .alg             = PSA_ALG_HMAC(PSA_ALG_SHA_256),
    .expected_status = PSA_ERROR_NOT_PERMITTED
},

{
    .test_desc       = "Test psa_mac_sign_setup - Invalid key type\n",
    .type            = PSA_KEY_TYPE_RAW_DATA,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_SIGN_MESSAGE,
    .alg             = PSA_ALG_HMAC(PSA_ALG_SHA_256),
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
#endif
#endif

#ifdef ARCH_TEST_HMAC
#ifdef ARCH_TEST_SHA256
#ifdef ARCH_TEST_TRUNCATED_MAC
{
    .test_desc       = "Test psa_mac_sign_setup - Truncated MAC too large\n",
    .type            = PSA_KEY_TYPE_HMAC,
    .data            = key_data,
    .data_length     = 64,
    .usage_flags     = PSA_KEY_USAGE_SIGN_MESSAGE,
    .alg             = PSA_ALG_TRUNCATED_MAC(PSA_ALG_HMAC(PSA_ALG_SHA_256), 33),
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},

{
    .test_desc       = "Test psa_mac_sign_setup - Truncated MAC too small\n",
    .type            = PSA_KEY_TYPE_HMAC,
    .data            = key_data,
    .data_length     = 64,
    .usage_flags     = PSA_KEY_USAGE_SIGN_MESSAGE,
    .alg             = PSA_ALG_TRUNCATED_MAC(PSA_ALG_HMAC(PSA_ALG_SHA_256), 1),
    .expected_status = PSA_ERROR_NOT_SUPPORTED
},
#endif
#endif

{
    .test_desc       = "Test psa_mac_sign_setup - Unknown MAC algorithm\n",
    .type            = PSA_KEY_TYPE_HMAC,
    .data            = key_data,
    .data_length     = 64,
    .usage_flags     = PSA_KEY_USAGE_SIGN_MESSAGE,
    .alg             = PSA_ALG_HMAC(0),
    .expected_status = PSA_ERROR_NOT_SUPPORTED
},
#endif

#ifdef ARCH_TEST_AES_128
#ifdef ARCH_TEST_CBC_NO_PADDING
{
    .test_desc       = "Test psa_mac_sign_setup - Bad algorithm (not a MAC algorithm)\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .usage_flags     = PSA_KEY_USAGE_SIGN_MESSAGE,
    .alg             = PSA_ALG_CBC_NO_PADDING,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
#endif
#endif
};
