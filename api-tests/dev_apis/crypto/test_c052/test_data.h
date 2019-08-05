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
    psa_key_handle_t        key_handle;
    psa_key_type_t          key_type;
    uint8_t                 key_data[32];
    uint32_t                key_length;
    psa_key_usage_t         usage;
    psa_algorithm_t         key_alg;
    psa_algorithm_t         alg;
    psa_status_t            expected_status;
} test_data;

static test_data check1[] = {
#ifdef ARCH_TEST_CCM
#ifdef ARCH_TEST_AES_128
{"Test psa_aead_encrypt_setup - AES-CCM\n", 1, PSA_KEY_TYPE_AES,
{0xD7, 0x82, 0x8D, 0x13, 0xB2, 0xB0, 0xBD, 0xC3, 0x25, 0xA7, 0x62, 0x36, 0xDF,
 0x93, 0xCC, 0x6B},
 AES_16B_KEY_SIZE, PSA_KEY_USAGE_ENCRYPT, PSA_ALG_CCM, PSA_ALG_CCM,
 PSA_SUCCESS
},

{"Test psa_aead_encrypt_setup - AES-CCM 24 bytes Tag length = 4\n", 2, PSA_KEY_TYPE_AES,
{0x41, 0x89, 0x35, 0x1B, 0x5C, 0xAE, 0xA3, 0x75, 0xA0, 0x29, 0x9E, 0x81, 0xC6,
 0x21, 0xBF, 0x43}, AES_16B_KEY_SIZE, PSA_KEY_USAGE_ENCRYPT, PSA_ALG_CCM,
 PSA_ALG_AEAD_WITH_TAG_LENGTH(PSA_ALG_CCM, 4),
 PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_GCM
#ifdef ARCH_TEST_AES_128
{"Test psa_aead_encrypt_setup - GCM - 16B AES - 12B Nounce & 12B addi data\n", 3, PSA_KEY_TYPE_AES,
{0x3d, 0xe0, 0x98, 0x74, 0xb3, 0x88, 0xe6, 0x49, 0x19, 0x88, 0xd0, 0xc3, 0x60,
 0x7e, 0xae, 0x1f}, AES_16B_KEY_SIZE, PSA_KEY_USAGE_ENCRYPT, PSA_ALG_GCM, PSA_ALG_GCM,
 PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_CCM
#ifdef ARCH_TEST_DES_1KEY
{"Test psa_aead_encrypt_setup - DES Key\n", 4, PSA_KEY_TYPE_DES,
{0x70, 0x24, 0x55, 0x0C, 0x14, 0x9D, 0xED, 0x29}, DES_8B_KEY_SIZE,
 PSA_KEY_USAGE_ENCRYPT, PSA_ALG_CCM, PSA_ALG_CCM,
 PSA_ERROR_NOT_SUPPORTED
},
#endif
#endif

#ifdef ARCH_TEST_AES_128
#ifdef ARCH_TEST_CIPER_MODE_CFB
{"Test psa_aead_encrypt_setup - Unsupported Algorithm\n", 5, PSA_KEY_TYPE_AES,
{0x49, 0x8E, 0xC7, 0x7D, 0x01, 0x95, 0x0D, 0x94, 0x2C, 0x16, 0xA5, 0x3E, 0x99,
 0x5F, 0xC9}, AES_16B_KEY_SIZE, PSA_KEY_USAGE_ENCRYPT, PSA_ALG_CFB, PSA_ALG_CFB,
 PSA_ERROR_NOT_SUPPORTED
},
#endif

#ifdef ARCH_TEST_GCM
{"Test psa_aead_encrypt_setup - Invalid key usage\n", 6, PSA_KEY_TYPE_AES,
{0x49, 0x8E, 0xC7, 0x7D, 0x01, 0x95, 0x0D, 0x94, 0x2C, 0x16, 0xA5, 0x3E, 0x99,
 0x5F, 0xC9}, AES_16B_KEY_SIZE, PSA_KEY_USAGE_DECRYPT, PSA_ALG_GCM, PSA_ALG_GCM,
 PSA_ERROR_NOT_PERMITTED
},
#endif
#endif
};
