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

#include "val_crypto.h"

typedef struct {
    char                    test_desc[75];
    psa_key_type_t          key_type;
    psa_key_usage_t         usage;
    psa_algorithm_t         key_alg;
    const uint8_t           key_data[64];
    size_t                  key_length;
    psa_algorithm_t         mac_alg;
    const uint8_t           data[16];
    size_t                  data_size;
    size_t                  mac_size;
    uint8_t                 expected_data[32];
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_HMAC
#ifdef ARCH_TEST_SHA224
{
    .test_desc          = "Test psa_mac_sign_multipart HMAC SHA 224\n",
    .key_type           = PSA_KEY_TYPE_HMAC,
    .usage              = PSA_KEY_USAGE_SIGN_HASH,
    .key_alg            = PSA_ALG_HMAC(PSA_ALG_SHA_224),
    .key_data           = {0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
                           0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b},
    .key_length         = 20,
    .mac_alg            = PSA_ALG_HMAC(PSA_ALG_SHA_224),
    .data               = {0x48, 0x69, 0x20, 0x54, 0x68, 0x65, 0x72, 0x65},
    .data_size          = 8,
    .mac_size           = 64,
    .expected_data      = {0x89, 0x6f, 0xb1, 0x12, 0x8a, 0xbb, 0xdf, 0x19, 0x68, 0x32,
                           0x10, 0x7c, 0xd4, 0x9d, 0xf3, 0x3f, 0x47, 0xb4, 0xb1, 0x16,
                           0x99, 0x12, 0xba, 0x4f, 0x53, 0x68, 0x4b, 0x22},
    .expected_status    = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_SHA256
{
    .test_desc          = "Test psa_mac_sign_multipart HMAC SHA 256\n",
    .key_type           = PSA_KEY_TYPE_HMAC,
    .usage              = PSA_KEY_USAGE_SIGN_HASH,
    .key_alg            = PSA_ALG_HMAC(PSA_ALG_SHA_256),
    .key_data           = {0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
                           0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b},
    .key_length         = 20,
    .mac_alg            = PSA_ALG_HMAC(PSA_ALG_SHA_256),
    .data               = {0x48, 0x69, 0x20, 0x54, 0x68, 0x65, 0x72, 0x65},
    .data_size          = 8,
    .mac_size           = 64,
    .expected_data      = {0xb0, 0x34, 0x4c, 0x61, 0xd8, 0xdb, 0x38, 0x53, 0x5c, 0xa8,
                           0xaf, 0xce, 0xaf, 0x0b, 0xf1, 0x2b, 0x88, 0x1d, 0xc2, 0x00,
                           0xc9, 0x83, 0x3d, 0xa7, 0x26, 0xe9, 0x37, 0x6c, 0x2e, 0x32,
                           0xcf, 0xf7},
    .expected_status    = PSA_SUCCESS
}
#endif
#endif
};
