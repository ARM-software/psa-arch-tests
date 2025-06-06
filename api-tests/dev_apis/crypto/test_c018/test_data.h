/** @file
 * Copyright (c) 2019-2025, Arm Limited or its affiliates. All rights reserved.
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
    char                        test_desc[75];
    psa_key_type_t              type;
    const uint8_t              *data;
    size_t                      data_length;
    size_t                      bits;
    psa_key_usage_t             usage_flags;
    psa_algorithm_t             alg;
    psa_key_derivation_step_t   step;
    psa_algorithm_t             setup_alg;
    psa_status_t                expected_status;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_SHA256
#ifdef ARCH_TEST_HKDF
#ifdef ARCH_TEST_AES_128
{
    .test_desc       = "Test psa_key_derivation_input_key 16 Byte Key\n",
    .type            = PSA_KEY_TYPE_DERIVE,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .bits            = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .setup_alg       = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .expected_status = PSA_SUCCESS
},

{
    .test_desc       = "Test psa_key_derivation_input_key with invalid usage\n",
    .type            = PSA_KEY_TYPE_DERIVE,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .bits            = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .alg             = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .setup_alg       = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .expected_status = PSA_ERROR_NOT_PERMITTED
},

{
    .test_desc       = "Test psa_key_derivation_input_key with step as label\n",
    .type            = PSA_KEY_TYPE_DERIVE,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .bits            = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .step            = PSA_KEY_DERIVATION_INPUT_LABEL,
    .setup_alg       = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},

{
    .test_desc       = "Test psa_key_derivation_input_key with step as info\n",
    .type            = PSA_KEY_TYPE_DERIVE,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .bits            = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .step            = PSA_KEY_DERIVATION_INPUT_INFO,
    .setup_alg       = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},

{
    .test_desc       = "Test psa_key_derivation_input_key with step as seed\n",
    .type            = PSA_KEY_TYPE_DERIVE,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .bits            = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .step            = PSA_KEY_DERIVATION_INPUT_SEED,
    .setup_alg       = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},

{
    .test_desc       = "Test psa_key_derivation_input_key with step as salt\n",
    .type            = PSA_KEY_TYPE_DERIVE,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .bits            = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .step            = PSA_KEY_DERIVATION_INPUT_SALT,
    .setup_alg       = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
#endif
#ifdef ARCH_TEST_AES_192
{
    .test_desc       = "Test psa_key_derivation_input_key with key type as AES(not derive)\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_24B_KEY_SIZE,
    .bits            = BYTES_TO_BITS(AES_24B_KEY_SIZE),
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .setup_alg       = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .expected_status = PSA_ERROR_INVALID_ARGUMENT,
},
#endif
#ifdef ARCH_TEST_AES_256
{
    .test_desc       = "Test psa_key_derivation_input_key incorrect key algorithm\n",
    .type            = PSA_KEY_TYPE_DERIVE,
    .data            = key_data,
    .data_length     = AES_32B_KEY_SIZE,
    .bits            = BYTES_TO_BITS(AES_32B_KEY_SIZE),
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_CBC_NO_PADDING,
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .setup_alg       = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .expected_status = PSA_ERROR_NOT_PERMITTED
},
#endif
#ifdef ARCH_TEST_RSA_PKCS1V15_SIGN_RAW
#ifdef ARCH_TEST_RSA_2048
{
    .test_desc       = "Test psa_key_derivation_input_key with key type as 2048 RSA public key\n",
    .type            = PSA_KEY_TYPE_RSA_PUBLIC_KEY,
    .data            = rsa_256_key_data,
    .data_length     = 270,
    .bits            = BYTES_TO_BITS(256),
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .setup_alg       = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .expected_status = PSA_ERROR_INVALID_ARGUMENT,
},

{
    .test_desc       = "Test psa_key_derivation_input_key with key type as RSA 2048 keypair\n",
    .type            = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .data            = rsa_256_key_pair,
    .data_length     = 1193,
    .bits            = BYTES_TO_BITS(256),
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .setup_alg       = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .expected_status = PSA_ERROR_INVALID_ARGUMENT,
},
#endif
#endif

{
    .test_desc       = "Test psa_key_derivation_input_key with zero as step\n",
    .type            = PSA_KEY_TYPE_DERIVE,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .bits            = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .step            = 0,
    .setup_alg       = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
#endif
#ifdef ARCH_TEST_HKDF_EXTRACT
{
    .test_desc       = "Test psa_key_derivation_input_key HKDF_EXTRACT with out of order step\n",
    .type            = PSA_KEY_TYPE_DERIVE,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .bits            = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_HKDF_EXTRACT(PSA_ALG_SHA_256),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .setup_alg       = PSA_ALG_HKDF_EXTRACT(PSA_ALG_SHA_256),
    .expected_status = PSA_ERROR_BAD_STATE
},
#endif
#endif
#ifdef ARCH_TEST_PBKDF2
#ifdef ARCH_TEST_HMAC
#ifdef ARCH_TEST_SHA256
{
    .test_desc        = "Test psa_key_derivation_input_key with cost input as step\n",
    .type             = PSA_KEY_TYPE_PASSWORD_HASH,
    .data             = key_data,
    .data_length      = AES_16B_KEY_SIZE,
    .bits             = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .usage_flags      = PSA_KEY_USAGE_DERIVE,
    .alg              = PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_256),
    .step             = PSA_KEY_DERIVATION_INPUT_COST,
    .setup_alg        = PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_256),
    .expected_status  = PSA_ERROR_INVALID_ARGUMENT
},

{
    .test_desc        = "Test psa_key_derivation_input_key with password as step\n",
    .type             = PSA_KEY_TYPE_PASSWORD,
    .data             = key_data,
    .data_length      = AES_16B_KEY_SIZE,
    .bits             = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .usage_flags      = PSA_KEY_USAGE_DERIVE,
    .alg              = PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_256),
    .step             = PSA_KEY_DERIVATION_INPUT_PASSWORD,
    .setup_alg        = PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_256),
    .expected_status  = PSA_SUCCESS
},
#endif
#endif
#endif
#ifdef ARCH_TEST_SHA256
#ifdef ARCH_TEST_TLS12_PSK_TO_MS
{
    .test_desc        = "Test psa_key_derivation_input_key with other secret as step\n",
    .type             = PSA_KEY_TYPE_DERIVE,
    .data             = key_data,
    .data_length      = 48,
    .bits             = 384,
    .usage_flags      = PSA_KEY_USAGE_DERIVE,
    .alg              = PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_SHA_256),
    .step             = PSA_KEY_DERIVATION_INPUT_OTHER_SECRET,
    .setup_alg        = PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_SHA_256),
    .expected_status  = PSA_SUCCESS
},
{
    .test_desc        = "Test psa_key_derivation_input_key with out of order step input\n",
    .type             = PSA_KEY_TYPE_DERIVE,
    .data             = key_data,
    .data_length      = 48,
    .bits             = 384,
    .usage_flags      = PSA_KEY_USAGE_DERIVE,
    .alg              = PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_SHA_256),
    .step             = PSA_KEY_DERIVATION_INPUT_SECRET,
    .setup_alg        = PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_SHA_256),
    .expected_status  = PSA_ERROR_BAD_STATE
},
#endif
#ifdef ARCH_TEST_TLS12_PRF
{
    .test_desc        = "Test psa_key_derivation_input_key with 16B key secret as step\n",
    .type             = PSA_KEY_TYPE_DERIVE,
    .data             = key_data,
    .data_length      = AES_16B_KEY_SIZE,
    .bits             = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .usage_flags      = PSA_KEY_USAGE_DERIVE,
    .alg              = PSA_ALG_TLS12_PRF(PSA_ALG_SHA_256),
    .step             = PSA_KEY_DERIVATION_INPUT_SECRET,
    .setup_alg        = PSA_ALG_TLS12_PRF(PSA_ALG_SHA_256),
    .expected_status  = PSA_SUCCESS
},
{
    .test_desc        = "Test psa_key_derivation_input_key with invalid input step\n",
    .type             = PSA_KEY_TYPE_DERIVE,
    .data             = key_data,
    .data_length      = AES_16B_KEY_SIZE,
    .bits             = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .usage_flags      = PSA_KEY_USAGE_DERIVE,
    .alg              = PSA_ALG_TLS12_PRF(PSA_ALG_SHA_256),
    .step             = PSA_KEY_DERIVATION_INPUT_OTHER_SECRET,
    .setup_alg        = PSA_ALG_TLS12_PRF(PSA_ALG_SHA_256),
    .expected_status  = PSA_ERROR_INVALID_ARGUMENT
},
#endif
#endif
#ifdef ARCH_TEST_SP800_108_COUNTER_HMAC
{
    .test_desc        = "Test psa_key_derivation_input_key - SP800_108_HMAC\n",
    .type             = PSA_KEY_TYPE_DERIVE,
    .data             = key_data,
    .data_length      = AES_16B_KEY_SIZE,
    .bits             = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .usage_flags      = PSA_KEY_USAGE_DERIVE,
    .alg              = PSA_ALG_SP800_108_COUNTER_HMAC(PSA_ALG_SHA_256),
    .step             = PSA_KEY_DERIVATION_INPUT_SECRET,
    .setup_alg        = PSA_ALG_SP800_108_COUNTER_HMAC(PSA_ALG_SHA_256),
    .expected_status  = PSA_SUCCESS
},
#endif
#ifdef ARCH_TEST_SP800_108_COUNTER_CMAC
{
    .test_desc        = "Test psa_key_derivation_input_key - SP800_108_CMAC\n",
    .type             = PSA_KEY_TYPE_AES,
    .data             = key_data,
    .data_length      = AES_16B_KEY_SIZE,
    .bits             = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .usage_flags      = PSA_KEY_USAGE_DERIVE,
    .alg              = PSA_ALG_SP800_108_COUNTER_CMAC,
    .step             = PSA_KEY_DERIVATION_INPUT_SECRET,
    .setup_alg        = PSA_ALG_SP800_108_COUNTER_CMAC,
    .expected_status  = PSA_SUCCESS
},
#endif

};
