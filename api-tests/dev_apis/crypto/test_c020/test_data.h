/** @file
 * Copyright (c) 2019, Arm Limited or its affiliates. All rights reserved.
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

#include "val_crypto.h"

typedef struct {
    char                        test_desc[75];
    psa_key_handle_t            key_handle;
    psa_key_type_t              key_type;
    uint8_t                     key_data[34];
    uint32_t                    key_length;
    size_t                      output_size;
    psa_key_usage_t             usage;
    psa_algorithm_t             key_alg;
    psa_key_derivation_step_t   step;
    size_t                      capacity;
    uint8_t                     data[16];
    size_t                      data_length;
    psa_status_t                expected_status;
} test_data;

static test_data check1[] = {
{"Test psa_key_derivation_output_bytes - Key\n", 1, PSA_KEY_TYPE_DERIVE,
{0x49, 0x8E, 0xC7, 0x7D, 0x01, 0x95, 0x0D, 0x94, 0x2C, 0x16, 0xA5, 0x3E, 0x99,
 0x5F, 0xC9, 0x77},
 AES_16B_KEY_SIZE, 42, PSA_KEY_USAGE_DERIVE,
 PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 PSA_KEY_DERIVATION_INPUT_SECRET, 42, {0}, 0,
 PSA_SUCCESS
},

{"Test psa_key_derivation_output_bytes - Info\n", 2, PSA_KEY_TYPE_DERIVE,
{0}, 0, 42, 0, PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 PSA_KEY_DERIVATION_INPUT_INFO, 42, "This is the info", 16,
 PSA_SUCCESS
},

{"Test psa_key_derivation_output_bytes - Salt\n", 3, PSA_KEY_TYPE_DERIVE,
{0}, 0, 42, 0, PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 PSA_KEY_DERIVATION_INPUT_SALT, 42, "This is the info", 16,
 PSA_ERROR_BAD_STATE
},

{"Test psa_key_derivation_output_bytes - Label\n", 4, PSA_KEY_TYPE_DERIVE,
{0}, 0, 42, 0, PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 PSA_KEY_DERIVATION_INPUT_LABEL, 42, "This is the info", 16,
 PSA_ERROR_BAD_STATE
},

{"Test psa_key_derivation_output_bytes - Seed\n", 5, PSA_KEY_TYPE_DERIVE,
{0}, 0, 42, 0, PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 PSA_KEY_DERIVATION_INPUT_SEED, 42, "This is the info", 16,
 PSA_ERROR_BAD_STATE
},

{"Test psa_key_derivation_output_bytes - Greater Capacity than available\n", 6, PSA_KEY_TYPE_DERIVE,
{0x49, 0x8E, 0xC7, 0x7D, 0x01, 0x95, 0x0D, 0x94, 0x2C, 0x16, 0xA5, 0x3E, 0x99,
 0x5F, 0xC9, 0x77},
 AES_16B_KEY_SIZE, BUFFER_SIZE, PSA_KEY_USAGE_DERIVE,
 PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 PSA_KEY_DERIVATION_INPUT_SECRET, 42, {0}, 0,
 PSA_ERROR_INSUFFICIENT_DATA,
},
};
