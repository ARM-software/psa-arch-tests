/** @file
 * Copyright (c) 2023-2024, Arm Limited or its affiliates. All rights reserved.
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
    psa_algorithm_t             alg;
    psa_key_derivation_step_t   step;
    uint64_t                    int_data;
    psa_status_t                expected_status;
} test_data;

static const test_data check1[] = {

#ifdef ARCH_TEST_PBKDF2
#ifdef ARCH_TEST_HMAC
#ifdef ARCH_TEST_SHA256
{
    .test_desc       = "Test psa_key_derivation_input_integer - cost input\n",
    .alg             = PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_256),
    .step            = PSA_KEY_DERIVATION_INPUT_COST,
    .int_data        = INTEGER_INPUT,
    .expected_status = PSA_SUCCESS
},

{
    .test_desc       = "Test psa_key_derivation_input_integer - salt\n",
    .alg             = PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_256),
    .step            = PSA_KEY_DERIVATION_INPUT_SALT,
    .int_data        = INTEGER_INPUT,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},

{
    .test_desc       = "Test psa_key_derivation_input_integer - password\n",
    .alg             = PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_256),
    .step            = PSA_KEY_DERIVATION_INPUT_PASSWORD,
    .int_data        = INTEGER_INPUT,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},

{
    .test_desc       = "Test psa_key_derivation_input_integer - Input as a negative integer\n",
    .alg             = PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_256),
    .step            = PSA_KEY_DERIVATION_INPUT_COST,
    .int_data        = NEGATIVE_INTEGER_INPUT,
    .expected_status = PSA_ERROR_NOT_SUPPORTED
}
#endif
#endif
#endif

};
