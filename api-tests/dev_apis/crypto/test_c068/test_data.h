/** @file
 * Copyright (c) 2024, Arm Limited or its affiliates. All rights reserved.
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
    psa_algorithm_t             key_alg;
    size_t                      capacity;
    const uint8_t              *data;
    size_t                      data_length;
    psa_key_derivation_step_t   step;
    const uint8_t              *salt_data;
    size_t                      salt_length;
    psa_key_type_t              key_type;
    size_t                      key_bits;
    psa_key_usage_t             usage;
    const uint8_t               expected_hash[32];
    size_t                      expected_hash_len;
    psa_status_t                expected_status;
} test_data;

static const test_data check1[] = {

#ifdef ARCH_TEST_PBKDF2
#ifdef ARCH_TEST_HMAC
#ifdef ARCH_TEST_SHA256
{
    .test_desc          = "Test psa_key_derivation_verify_key - PBKDF2_HMAC algo. with Password\n",
    .key_alg            = PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_256),
    .capacity           = 32,
    .data               = input_password,
    .data_length        = INPUT_PASSWORD_LEN,
    .step               = PSA_KEY_DERIVATION_INPUT_PASSWORD,
    .salt_data          = input_salt,
    .salt_length        = INPUT_SALT_LEN,
    .key_type           = PSA_KEY_TYPE_PASSWORD_HASH,
    .key_bits           = BYTES_TO_BITS(AES_32B_KEY_SIZE),
    .usage              = PSA_KEY_USAGE_VERIFY_DERIVATION,
    .expected_hash      = {0xD0, 0x6A, 0xE2, 0xF2, 0xC9, 0x7C, 0x9D, 0x56, 0x99, 0x5D, 0xEA,
                           0x77, 0xC5, 0x38, 0x72, 0x08, 0xF5, 0x14, 0xF1, 0x4F, 0x39, 0x63,
                           0x69, 0xEB, 0x0C, 0x69, 0x66, 0xC8, 0x33, 0x8E, 0x28, 0xE5},
    .expected_hash_len  = 32,
    .expected_status    = PSA_SUCCESS
},
{
    .test_desc          = "Test psa_key_derivation_verify_key - Incorrect usage flag\n",
    .key_alg            = PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_256),
    .capacity           = 32,
    .data               = input_password,
    .data_length        = INPUT_PASSWORD_LEN,
    .step               = PSA_KEY_DERIVATION_INPUT_SALT,
    .salt_data          = input_salt,
    .salt_length        = INPUT_SALT_LEN,
    .key_type           = PSA_KEY_TYPE_PASSWORD_HASH,
    .key_bits           = BYTES_TO_BITS(AES_32B_KEY_SIZE),
    .usage              = PSA_KEY_USAGE_EXPORT,
    .expected_hash      = {0xB4, 0xF7, 0x9C, 0x4A, 0xE5, 0x47, 0xF4, 0x44, 0x9A, 0xED, 0x76,
                           0x7C, 0xFC, 0xA7, 0xA3, 0x15, 0xF4, 0x3C, 0x57, 0x95, 0x50, 0x82,
                           0x57, 0xB4, 0x7B, 0x98, 0xE6, 0x67, 0x6B, 0x6B, 0xC7, 0x00},
    .expected_hash_len  = 32,
    .expected_status    = PSA_ERROR_NOT_PERMITTED
},
{
    .test_desc          = "Test psa_key_derivation_verify_key - Mismatched expected value\n",
    .key_alg            = PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_256),
    .capacity           = 32,
    .data               = input_password,
    .data_length        = INPUT_PASSWORD_LEN,
    .step               = PSA_KEY_DERIVATION_INPUT_PASSWORD,
    .salt_data          = input_salt,
    .salt_length        = INPUT_SALT_LEN,
    .key_type           = PSA_KEY_TYPE_PASSWORD_HASH,
    .key_bits           = BYTES_TO_BITS(AES_32B_KEY_SIZE),
    .usage              = PSA_KEY_USAGE_VERIFY_DERIVATION,
    .expected_hash      = {0xB4, 0xF7, 0x9C, 0x4A, 0xE5, 0x47, 0xF4, 0x44, 0x9A, 0xED, 0x76,
                           0x7C, 0xFC, 0xA7, 0xA3, 0x15, 0xF4, 0x3C, 0x57, 0x95, 0x50, 0x82,
                           0x57, 0xB4, 0x7B, 0x98, 0xE6, 0x67, 0x6B, 0x6B, 0xC7, 0x00},
    .expected_hash_len  = 32,
    .expected_status    = PSA_ERROR_INVALID_SIGNATURE
},
{
    .test_desc          = "Test psa_key_derivation_verify_key - Invalid operation's capacity\n",
    .key_alg            = PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_256),
    .capacity           = 16,
    .data               = input_password,
    .data_length        = INPUT_PASSWORD_LEN,
    .step               = PSA_KEY_DERIVATION_INPUT_PASSWORD,
    .salt_data          = input_salt,
    .salt_length        = INPUT_SALT_LEN,
    .key_type           = PSA_KEY_TYPE_PASSWORD_HASH,
    .key_bits           = BYTES_TO_BITS(AES_32B_KEY_SIZE),
    .usage              = PSA_KEY_USAGE_VERIFY_DERIVATION,
    .expected_hash      = {0xD0, 0x6A, 0xE2, 0xF2, 0xC9, 0x7C, 0x9D, 0x56, 0x99, 0x5D, 0xEA,
                           0x77, 0xC5, 0x38, 0x72, 0x08, 0xF5, 0x14, 0xF1, 0x4F, 0x39, 0x63,
                           0x69, 0xEB, 0x0C, 0x69, 0x66, 0xC8, 0x33, 0x8E, 0x28, 0xE5},
    .expected_hash_len  = 32,
    .expected_status    = PSA_ERROR_INSUFFICIENT_DATA
},
#endif
#endif

#ifdef ARCH_TEST_CMAC
{
    .test_desc          = "Test psa_key_derivation_verify_key - PBKDF2_CMAC algo. with Password\n",
    .key_alg            = PSA_ALG_PBKDF2_AES_CMAC_PRF_128,
    .capacity           = 32,
    .data               = input_password,
    .data_length        = INPUT_PASSWORD_LEN,
    .step               = PSA_KEY_DERIVATION_INPUT_PASSWORD,
    .salt_data          = input_salt,
    .salt_length        = INPUT_SALT_LEN,
    .key_type           = PSA_KEY_TYPE_PASSWORD_HASH,
    .key_bits           = BYTES_TO_BITS(AES_32B_KEY_SIZE),
    .usage              = PSA_KEY_USAGE_VERIFY_DERIVATION,
    .expected_hash      = {0x5E, 0x31, 0x62, 0x72, 0x59, 0xDD, 0x53, 0x98, 0x30, 0x38, 0xC8,
                           0xE5, 0x9B, 0x79, 0xF9, 0xE8, 0x05, 0xAA, 0xA5, 0x9F, 0x9C, 0x7D,
                           0x57, 0x70, 0xB4, 0x16, 0x73, 0x9E, 0x7D, 0x01, 0xFF, 0x6E},
    .expected_hash_len  = 32,
    .expected_status    = PSA_SUCCESS
},
{
    .test_desc          = "Test psa_key_derivation_verify_key - wrong key type\n",
    .key_alg            = PSA_ALG_PBKDF2_AES_CMAC_PRF_128,
    .capacity           = 32,
    .data               = input_password,
    .data_length        = INPUT_PASSWORD_LEN,
    .step               = PSA_KEY_DERIVATION_INPUT_PASSWORD,
    .salt_data          = input_salt,
    .salt_length        = INPUT_SALT_LEN,
    .key_type           = PSA_KEY_TYPE_PASSWORD,
    .key_bits           = BYTES_TO_BITS(AES_32B_KEY_SIZE),
    .usage              = PSA_KEY_USAGE_VERIFY_DERIVATION,
    .expected_hash      = {0x5E, 0x31, 0x62, 0x72, 0x59, 0xDD, 0x53, 0x98, 0x30, 0x38, 0xC8,
                           0xE5, 0x9B, 0x79, 0xF9, 0xE8, 0x05, 0xAA, 0xA5, 0x9F, 0x9C, 0x7D,
                           0x57, 0x70, 0xB4, 0x16, 0x73, 0x9E, 0x7D, 0x01, 0xFF, 0x6E},
    .expected_hash_len  = 32,
    .expected_status    = PSA_ERROR_INVALID_ARGUMENT
}
#endif
#endif
};
