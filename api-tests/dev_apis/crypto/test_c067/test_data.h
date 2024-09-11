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

#define DERIVATION_INPUT_CNT    3

typedef struct {
    psa_key_derivation_step_t  step;
    const uint8_t             *data;
    size_t                     data_length;
} key_derivation_input_t;

typedef struct {
    char                        test_desc[75];
    psa_key_type_t              type;
    psa_key_usage_t             usage_flag;
    psa_algorithm_t             alg;
    size_t                      capacity;
    const uint8_t              *data;
    size_t                      data_length;
    key_derivation_input_t      derv_inputs[DERIVATION_INPUT_CNT];
    const uint8_t               expected_output[32];
    size_t                      expected_length;
    psa_status_t                expected_status;
} test_data;

static const test_data check1[] = {

#ifdef ARCH_TEST_SHA256
#ifdef ARCH_TEST_PBKDF2
#ifdef ARCH_TEST_HMAC
{
    .test_desc          = "Test psa_key_derivation_verify_bytes - PBKDF2_HMAC alg. with Password\n",
    .type               = PSA_KEY_TYPE_PASSWORD,
    .usage_flag         = PSA_KEY_USAGE_VERIFY_DERIVATION,
    .alg                = PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_256),
    .capacity           = 32,
    .data               = input_bytes_data,
    .data_length        = INPUT_BYTES_DATA_LEN,
    .derv_inputs        = {
                            {
                            .step        = PSA_KEY_DERIVATION_INPUT_COST,
                            .data        = NULL,
                            .data_length = 1
                            },
                            {
                            .step        = PSA_KEY_DERIVATION_INPUT_SALT,
                            .data        = input_salt,
                            .data_length = INPUT_SALT_LEN
                            },
                            {
                            .step        = PSA_KEY_DERIVATION_INPUT_PASSWORD,
                            .data        = input_password,
                            .data_length = INPUT_PASSWORD_LEN
                            }
                          },
    .expected_output    = {0xD0, 0x6A, 0xE2, 0xF2, 0xC9, 0x7C, 0x9D, 0x56, 0x99, 0x5D, 0xEA,
                           0x77, 0xC5, 0x38, 0x72, 0x08, 0xF5, 0x14, 0xF1, 0x4F, 0x39, 0x63,
                           0x69, 0xEB, 0x0C, 0x69, 0x66, 0xC8, 0x33, 0x8E, 0x28, 0xE5},
    .expected_length    = 32,
    .expected_status    = PSA_SUCCESS
},
{
    .test_desc          = "Test psa_key_derivation_verify_bytes - Mismatched expected value\n",
    .type               = PSA_KEY_TYPE_PASSWORD,
    .usage_flag         = PSA_KEY_USAGE_VERIFY_DERIVATION,
    .alg                = PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_256),
    .capacity           = 32,
    .data               = input_bytes_data,
    .data_length        = INPUT_BYTES_DATA_LEN,
    .derv_inputs        = {
                            {
                            .step        = PSA_KEY_DERIVATION_INPUT_COST,
                            .data        = NULL,
                            .data_length = 1
                            },
                            {
                            .step        = PSA_KEY_DERIVATION_INPUT_SALT,
                            .data        = input_salt,
                            .data_length = INPUT_SALT_LEN
                            },
                            {
                            .step        = PSA_KEY_DERIVATION_INPUT_PASSWORD,
                            .data        = input_password,
                            .data_length = INPUT_PASSWORD_LEN
                            }
                          },
    .expected_output    = {0xB4, 0xF7, 0x9C, 0x4A, 0xE5, 0x47, 0xF4, 0x44, 0x9A, 0xED, 0x76,
                           0x7C, 0xFC, 0xA7, 0xA3, 0x15, 0xF4, 0x3C, 0x57, 0x95, 0x50, 0x82,
                           0x57, 0xB4, 0x7B, 0x98, 0xE6, 0x67, 0x6B, 0x6B, 0xC7, 0x00},
    .expected_length    = 32,
    .expected_status    = PSA_ERROR_INVALID_SIGNATURE
},
{
    .test_desc          = "Test psa_key_derivation_verify_bytes - Invalid operation's capacity\n",
    .type               = PSA_KEY_TYPE_PASSWORD,
    .usage_flag         = PSA_KEY_USAGE_VERIFY_DERIVATION,
    .alg                = PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_256),
    .capacity           = 16,
    .data               = input_bytes_data,
    .data_length        = INPUT_BYTES_DATA_LEN,
    .derv_inputs        = {
                            {
                            .step        = PSA_KEY_DERIVATION_INPUT_COST,
                            .data        = NULL,
                            .data_length = 1
                            },
                            {
                            .step        = PSA_KEY_DERIVATION_INPUT_SALT,
                            .data        = input_salt,
                            .data_length = INPUT_SALT_LEN
                            },
                            {
                            .step        = PSA_KEY_DERIVATION_INPUT_PASSWORD,
                            .data        = input_password,
                            .data_length = INPUT_PASSWORD_LEN
                            }
                          },
    .expected_output    = {0xD0, 0x6A, 0xE2, 0xF2, 0xC9, 0x7C, 0x9D, 0x56, 0x99, 0x5D, 0xEA,
                           0x77, 0xC5, 0x38, 0x72, 0x08, 0xF5, 0x14, 0xF1, 0x4F, 0x39, 0x63,
                           0x69, 0xEB, 0x0C, 0x69, 0x66, 0xC8, 0x33, 0x8E, 0x28, 0xE5},
    .expected_length    = 32,
    .expected_status    = PSA_ERROR_INSUFFICIENT_DATA
},
#endif
#endif

#ifdef ARCH_TEST_HKDF
{
    .test_desc          = "Test psa_key_derivation_verify_bytes - HKDF algo.\n",
    .type               = PSA_KEY_TYPE_DERIVE,
    .usage_flag         = PSA_KEY_USAGE_VERIFY_DERIVATION,
    .alg                = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .capacity           = 32,
    .data               = input_bytes_data,
    .data_length        = INPUT_BYTES_DATA_LEN,
    .derv_inputs        = {
                            {
                            .step        = PSA_KEY_DERIVATION_INPUT_SALT,
                            .data        = input_salt,
                            .data_length = INPUT_SALT_LEN
                            },
                            {
                            .step        = PSA_KEY_DERIVATION_INPUT_SECRET,
                            .data        = NULL,
                            .data_length = 0
                            },
                            {
                            .step        = PSA_KEY_DERIVATION_INPUT_INFO,
                            .data        = input_info,
                            .data_length = INPUT_INFO_LEN
                            }
                         },
    .expected_output    = {0x6B, 0x85, 0xC0, 0x4D, 0xEF, 0xCE, 0x20, 0x03, 0x0B, 0x6F, 0x0B,
                           0x1C, 0xDE, 0xED, 0x42, 0x97, 0xAF, 0xF1, 0x5B, 0x35, 0x39, 0x76,
                           0xF8, 0xB2, 0x7C, 0x2B, 0xAC, 0x51, 0xCB, 0x57, 0x3E, 0xD3},
    .expected_length    = 32,
    .expected_status    = PSA_SUCCESS
},

{
    .test_desc          = "Test psa_key_derivation_verify_bytes - optional salt\n",
    .type               = PSA_KEY_TYPE_DERIVE,
    .usage_flag         = PSA_KEY_USAGE_VERIFY_DERIVATION,
    .alg                = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .capacity           = 32,
    .data               = input_bytes_data,
    .data_length        = INPUT_BYTES_DATA_LEN,
    .derv_inputs        = {
                            {
                            .step        = PSA_KEY_DERIVATION_INPUT_SECRET,
                            .data        = NULL,
                            .data_length = 0
                            },
                            {
                            .step        = PSA_KEY_DERIVATION_INPUT_INFO,
                            .data        = input_info,
                            .data_length = INPUT_INFO_LEN
                            },
                            {
                            .step        = 0,
                            .data        = NULL,
                            .data_length = 0
                            }
                         },
    .expected_output    = {0xF5, 0x07, 0x1E, 0x9D, 0x8E, 0xE5, 0x87, 0xE1, 0xB5, 0xA6, 0x76,
                           0x9C, 0x6B, 0xEC, 0xC9, 0x76, 0x06, 0x59, 0x29, 0x29, 0x6B, 0x51,
                           0xAE, 0x27, 0xFC, 0x45, 0x03, 0x40, 0x82, 0xBE, 0x93, 0xEA},
    .expected_length    = 32,
    .expected_status    = PSA_SUCCESS
},

{
    .test_desc          = "Test psa_key_derivation_verify_bytes - missing salt/secret/info\n",
    .type               = PSA_KEY_TYPE_DERIVE,
    .usage_flag         = PSA_KEY_USAGE_VERIFY_DERIVATION,
    .alg                = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .capacity           = 32,
    .data               = input_bytes_data,
    .data_length        = INPUT_BYTES_DATA_LEN,
    .derv_inputs        = {
                            {
                            .step        = 0,
                            .data        = NULL,
                            .data_length = 0
                            },
                            {
                            .step        = 0,
                            .data        = NULL,
                            .data_length = 0
                            },
                            {
                            .step        = 0,
                            .data        = NULL,
                            .data_length = 0
                            }
                         },
    .expected_output    = {0xEB, 0x70, 0xF0, 0x1D, 0xED, 0xE9, 0xAF, 0xAF, 0xA4, 0x49, 0xEE,
                           0xE1, 0xB1, 0x28, 0x65, 0x04, 0xE1, 0xF6, 0x23, 0x88, 0xB3, 0xF7,
                           0xDD, 0x4F, 0x95, 0x66, 0x97, 0xB0, 0xE8, 0x28, 0xFE, 0x18},
    .expected_length    = 32,
    .expected_status    = PSA_ERROR_BAD_STATE
}
#endif
#endif
};
