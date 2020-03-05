/** @file
 * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
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
    psa_key_type_t          key_type;
    size_t                  attr_bits;
    psa_key_usage_t         usage;
    psa_algorithm_t         key_alg;
    psa_key_id_t            key_id;
    psa_key_lifetime_t      key_lifetime;
    psa_key_lifetime_t      lifetime;
} test_data;

static test_data check1[] = {
{"Test set/get key attributes\n", PSA_KEY_TYPE_AES, BYTES_TO_BITS(AES_16B_KEY_SIZE),
 PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT, PSA_ALG_CCM, 0x1234,
 PSA_KEY_LIFETIME_PERSISTENT
},
};
