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
    psa_algorithm_t             alg;
    uint8_t                     data[32];
    size_t                      data_length;
    psa_key_derivation_step_t   step;
    psa_status_t                expected_status;
} test_data;

static test_data check1[] = {
{"Test psa_key_derivation_input_bytes - Step as Info\n",
 PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 "abcdefghijklmnop", 16,
 PSA_KEY_DERIVATION_INPUT_INFO,
 PSA_SUCCESS,
},

{"Test psa_key_derivation_input_bytes - Step as secret\n",
 PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 "abcdefghijklmnop", 16,
 PSA_KEY_DERIVATION_INPUT_SECRET,
 PSA_ERROR_INVALID_ARGUMENT,
},

{"Test psa_key_derivation_input_bytes - Step as salt\n",
 PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 "abcdefghijklmnop", 16,
 PSA_KEY_DERIVATION_INPUT_SALT,
 PSA_SUCCESS,
},

{"Test psa_key_derivation_input_bytes - Step as label\n",
 PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 "abcdefghijklmnop", 16,
 PSA_KEY_DERIVATION_INPUT_LABEL,
 PSA_ERROR_INVALID_ARGUMENT,
},

{"Test psa_key_derivation_input_bytes - Step as seed\n",
 PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 "abcdefghijklmnop", 16,
 PSA_KEY_DERIVATION_INPUT_SEED,
 PSA_ERROR_INVALID_ARGUMENT,
},

{"Test psa_key_derivation_input_bytes - Invalid step\n",
 PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 "abcdefghijklmnop", 16,
 PSA_KEY_DERIVATION_STEP_INVALID,
 PSA_ERROR_INVALID_ARGUMENT,
},
};
