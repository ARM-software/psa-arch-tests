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
    char                    test_desc[75];
    psa_algorithm_t         alg;
    psa_status_t            expected_status;
} test_data;

static test_data check1[] = {
{"Test psa_key_derivation_setup - ECDH + HKDF-SHA-256\n",
 PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
 PSA_SUCCESS
},

{"Test psa_key_derivation_setup - ECDH, unknown KDF\n",
 PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(0)),
 PSA_ERROR_NOT_SUPPORTED
},

{"Test psa_key_derivation_setup - bad key derivation algorithm\n",
 PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_HASH_ALG_INVALID),
 PSA_ERROR_INVALID_ARGUMENT
},

{"Test psa_key_derivation_setup - Invalid Algorithm\n",
 PSA_ALG_INVALID,
 PSA_ERROR_INVALID_ARGUMENT,
},
};
