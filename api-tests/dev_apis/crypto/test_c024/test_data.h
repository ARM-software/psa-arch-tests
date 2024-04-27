/** @file
 * Copyright (c) 2019-2024, Arm Limited or its affiliates. All rights reserved.
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
    char                    test_desc[75];
    psa_key_type_t          type;
    const uint8_t          *data;
    size_t                  data_length;
    psa_key_usage_t         usage_flags;
    psa_algorithm_t         key_alg;
    psa_algorithm_t         aead_alg;
    const uint8_t          *nonce;
    size_t                  nonce_length;
    const uint8_t          *additional_data;
    size_t                  additional_data_length;
    const uint8_t          *plaintext;
    size_t                  plaintext_length;
    uint8_t                *ciphertext;
    const uint8_t          *expected_ciphertext;
    size_t                  ciphertext_size;
    size_t                  expected_ciphertext_length;
    psa_status_t            expected_status[2];
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_CCM
#ifdef ARCH_TEST_AES_128
{
    .test_desc                  = "Test psa_aead_encrypt - CCM - AES - 13B nonce & 8B add data\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_ENCRYPT,
    .key_alg                    = PSA_ALG_CCM,
    .aead_alg                   = PSA_ALG_CCM,
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 8,
    .plaintext                  = plaintext,
    .plaintext_length           = 23,
    .ciphertext                 = expected_output,
    .expected_ciphertext        = aead_ciphertext_1,
    .ciphertext_size            = BUFFER_SIZE,
    .expected_ciphertext_length = AEAD_CIPHERTEXT_LEN_1,
    .expected_status            = {PSA_SUCCESS, PSA_SUCCESS}
},

{
    .test_desc                  = "Test psa_aead_encrypt - CCM - AES - 13B nonce & 32B add data\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_ENCRYPT,
    .key_alg                    = PSA_ALG_CCM,
    .aead_alg                   = PSA_ALG_CCM,
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 32,
    .plaintext                  = plaintext,
    .plaintext_length           = 24,
    .ciphertext                 = expected_output,
    .expected_ciphertext        = aead_ciphertext_2,
    .ciphertext_size            = BUFFER_SIZE,
    .expected_ciphertext_length = AEAD_CIPHERTEXT_LEN_2,
    .expected_status            = {PSA_SUCCESS, PSA_SUCCESS}
},

#ifndef ARCH_TEST_CCM_MIN_TAG_LEN_8
{
    .test_desc                  = "Test psa_aead_encrypt - CCM - AES - 24 bytes Tag length = 4\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_ENCRYPT,
    .key_alg                    = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 4),
    .aead_alg                   = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 4),
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 32,
    .plaintext                  = plaintext,
    .plaintext_length           = 24,
    .ciphertext                 = expected_output,
    .expected_ciphertext        = aead_ciphertext_3,
    .ciphertext_size            = BUFFER_SIZE,
    .expected_ciphertext_length = AEAD_CIPHERTEXT_LEN_3,
    .expected_status            = {PSA_SUCCESS, PSA_SUCCESS}
},

{
    .test_desc                  = "Test psa_aead_encrypt - CCM - AES (atleast Tag length=4)\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_ENCRYPT,
    .key_alg                    = PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(PSA_ALG_CCM, 4),
    .aead_alg                   = PSA_ALG_CCM,
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 8,
    .plaintext                  = plaintext,
    .plaintext_length           = 23,
    .ciphertext                 = expected_output,
    .expected_ciphertext        = aead_ciphertext_1,
    .ciphertext_size            = BUFFER_SIZE,
    .expected_ciphertext_length = AEAD_CIPHERTEXT_LEN_1,
    .expected_status            = {PSA_SUCCESS, PSA_SUCCESS}
},

{
    .test_desc                  = "Test psa_aead_encrypt - (atleast length=4 on shortened Tag)\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_ENCRYPT,
    .key_alg                    = PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(PSA_ALG_CCM, 4),
    .aead_alg                   = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 4),
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 32,
    .plaintext                  = plaintext,
    .plaintext_length           = 24,
    .ciphertext                 = expected_output,
    .expected_ciphertext        = aead_ciphertext_3,
    .ciphertext_size            = BUFFER_SIZE,
    .expected_ciphertext_length = AEAD_CIPHERTEXT_LEN_3,
    .expected_status            = {PSA_SUCCESS, PSA_SUCCESS}
},

{
    .test_desc                  = "Test psa_aead_encrypt - CCM - AES (Tag length < min. length)\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_ENCRYPT,
    .key_alg                    = PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(PSA_ALG_CCM, 8),
    .aead_alg                   = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 4),
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 32,
    .plaintext                  = plaintext,
    .plaintext_length           = 24,
    .ciphertext                 = expected_output,
    .expected_ciphertext        = aead_ciphertext_3,
    .ciphertext_size            = BUFFER_SIZE,
    .expected_ciphertext_length = AEAD_CIPHERTEXT_LEN_3,
    .expected_status            = {PSA_ERROR_NOT_PERMITTED, PSA_ERROR_NOT_PERMITTED}
},
#else
{
    .test_desc                  = "Test psa_aead_encrypt - CCM - AES - 24 bytes Tag length = 8\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_ENCRYPT,
    .key_alg                    = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 8),
    .aead_alg                   = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 8),
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 32,
    .plaintext                  = plaintext,
    .plaintext_length           = 24,
    .ciphertext                 = expected_output,
    .expected_ciphertext        = aead_ciphertext_7,
    .ciphertext_size            = BUFFER_SIZE,
    .expected_ciphertext_length = AEAD_CIPHERTEXT_LEN_7,
    .expected_status            = {PSA_SUCCESS, PSA_SUCCESS}
},

{
    .test_desc                  = "Test psa_aead_encrypt - CCM - AES (atleast Tag length=8)\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_ENCRYPT,
    .key_alg                    = PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(PSA_ALG_CCM, 8),
    .aead_alg                   = PSA_ALG_CCM,
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 8,
    .plaintext                  = plaintext,
    .plaintext_length           = 23,
    .ciphertext                 = expected_output,
    .expected_ciphertext        = aead_ciphertext_1,
    .ciphertext_size            = BUFFER_SIZE,
    .expected_ciphertext_length = AEAD_CIPHERTEXT_LEN_1,
    .expected_status            = {PSA_SUCCESS, PSA_SUCCESS}
},

{
    .test_desc                  = "Test psa_aead_encrypt - (atleast length=8 on shortened Tag)\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_ENCRYPT,
    .key_alg                    = PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(PSA_ALG_CCM, 8),
    .aead_alg                   = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 8),
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 32,
    .plaintext                  = plaintext,
    .plaintext_length           = 24,
    .ciphertext                 = expected_output,
    .expected_ciphertext        = aead_ciphertext_7,
    .ciphertext_size            = BUFFER_SIZE,
    .expected_ciphertext_length = AEAD_CIPHERTEXT_LEN_7,
    .expected_status            = {PSA_SUCCESS, PSA_SUCCESS}
},

{
    .test_desc                  = "Test psa_aead_encrypt - CCM - AES (Tag length < min. length)\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_ENCRYPT,
    .key_alg                    = PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(PSA_ALG_CCM, 10),
    .aead_alg                   = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 8),
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 32,
    .plaintext                  = plaintext,
    .plaintext_length           = 24,
    .ciphertext                 = expected_output,
    .expected_ciphertext        = aead_ciphertext_7,
    .ciphertext_size            = BUFFER_SIZE,
    .expected_ciphertext_length = AEAD_CIPHERTEXT_LEN_7,
    .expected_status            = {PSA_ERROR_NOT_PERMITTED, PSA_ERROR_NOT_PERMITTED}
},
#endif

#ifdef ARCH_TEST_ARIA
{
    .test_desc                  = "Test psa_aead_encrypt - CCM - ARIA - 24 bytes Tag length = 4\n",
    .type                       = PSA_KEY_TYPE_ARIA,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_ENCRYPT,
    .key_alg                    = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 4),
    .aead_alg                   = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 4),
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 32,
    .plaintext                  = plaintext,
    .plaintext_length           = 24,
    .ciphertext                 = expected_output,
    .expected_ciphertext        = aead_aria_cipher_test,
    .ciphertext_size            = BUFFER_SIZE,
    .expected_ciphertext_length = AEAD_CIPHERTEXT_LEN_3,
    .expected_status            = {PSA_SUCCESS, PSA_SUCCESS}
},
#endif

{
    .test_desc                  = "Test psa_aead_encrypt - CCM - AES - Zero additional data\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_ENCRYPT,
    .key_alg                    = PSA_ALG_CCM,
    .aead_alg                   = PSA_ALG_CCM,
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = NULL,
    .additional_data_length     = 0,
    .plaintext                  = plaintext,
    .plaintext_length           = 24,
    .ciphertext                 = expected_output,
    .expected_ciphertext        = aead_ciphertext_4,
    .ciphertext_size            = BUFFER_SIZE,
    .expected_ciphertext_length = AEAD_CIPHERTEXT_LEN_4,
    .expected_status            = {PSA_SUCCESS, PSA_SUCCESS}
},

{
    .test_desc                  = "Test psa_aead_encrypt - CCM - AES - Zero plaintext\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_ENCRYPT,
    .key_alg                    = PSA_ALG_CCM,
    .aead_alg                   = PSA_ALG_CCM,
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 32,
    .plaintext                  = NULL,
    .plaintext_length           = 0,
    .ciphertext                 = expected_output,
    .expected_ciphertext        = aead_ciphertext_5,
    .ciphertext_size            = BUFFER_SIZE,
    .expected_ciphertext_length = AEAD_CIPHERTEXT_LEN_5,
    .expected_status            = {PSA_SUCCESS, PSA_SUCCESS}
},
#endif
#endif

#ifdef ARCH_TEST_GCM
#ifdef ARCH_TEST_AES_128
{
    .test_desc                  = "Test psa_aead_encrypt - GCM - AES - 12B nonce & 12B add data\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_ENCRYPT,
    .key_alg                    = PSA_ALG_GCM,
    .aead_alg                   = PSA_ALG_GCM,
    .nonce                      = nonce,
    .nonce_length               = 12,
    .additional_data            = additional_data,
    .additional_data_length     = 12,
    .plaintext                  = plaintext,
    .plaintext_length           = 24,
    .ciphertext                 = expected_output,
    .expected_ciphertext        = aead_ciphertext_6,
    .ciphertext_size            = BUFFER_SIZE,
    .expected_ciphertext_length = AEAD_CIPHERTEXT_LEN_6,
    .expected_status            = {PSA_SUCCESS, PSA_SUCCESS}
},
#endif
#endif

#ifdef ARCH_TEST_CCM
#ifdef ARCH_TEST_AES_128
{
    .test_desc                  = "Test psa_aead_encrypt - Unsupported algorithm\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_ENCRYPT,
    .key_alg                    = PSA_ALG_CFB,
    .aead_alg                   = PSA_ALG_CFB,
    .nonce                      = NULL,
    .nonce_length               = 0,
    .additional_data            = NULL,
    .additional_data_length     = 0,
    .plaintext                  = NULL,
    .plaintext_length           = 0,
    .ciphertext                 = NULL,
    .expected_ciphertext        = NULL,
    .ciphertext_size            = 0,
    .expected_ciphertext_length = 0,
    .expected_status            = {PSA_ERROR_NOT_SUPPORTED, PSA_ERROR_INVALID_ARGUMENT}
},

{
    .test_desc                  = "Test psa_aead_encrypt - Invalid usage flag\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_DECRYPT,
    .key_alg                    = PSA_ALG_CCM,
    .aead_alg                   = PSA_ALG_CCM,
    .nonce                      = NULL,
    .nonce_length               = 0,
    .additional_data            = NULL,
    .additional_data_length     = 0,
    .plaintext                  = NULL,
    .plaintext_length           = 0,
    .ciphertext                 = NULL,
    .expected_ciphertext        = NULL,
    .ciphertext_size            = 0,
    .expected_ciphertext_length = 0,
    .expected_status            = {PSA_ERROR_NOT_PERMITTED, PSA_ERROR_NOT_PERMITTED}
},

{
    .test_desc                  = "Test psa_aead_encrypt - Invalid ciphertext_size\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_ENCRYPT,
    .key_alg                    = PSA_ALG_CCM,
    .aead_alg                   = PSA_ALG_CCM,
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 32,
    .plaintext                  = plaintext,
    .plaintext_length           = 24,
    .ciphertext                 = expected_output,
    .expected_ciphertext        = aead_ciphertext_2,
    .ciphertext_size            = AEAD_CIPHERTEXT_LEN_2-1,
    .expected_ciphertext_length = AEAD_CIPHERTEXT_LEN_2,
    .expected_status            = {PSA_ERROR_BUFFER_TOO_SMALL, PSA_ERROR_BUFFER_TOO_SMALL}
},

{
    .test_desc                  = "Test psa_aead_encrypt - Invalid nonce\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_ENCRYPT,
    .key_alg                    = PSA_ALG_CCM,
    .aead_alg                   = PSA_ALG_CCM,
    .nonce                      = NULL,
    .nonce_length               = 0,
    .additional_data            = additional_data,
    .additional_data_length     = 32,
    .plaintext                  = plaintext,
    .plaintext_length           = 24,
    .ciphertext                 = expected_output,
    .expected_ciphertext        = aead_ciphertext_2,
    .ciphertext_size            = BUFFER_SIZE,
    .expected_ciphertext_length = AEAD_CIPHERTEXT_LEN_2,
    .expected_status            = {PSA_ERROR_INVALID_ARGUMENT, PSA_ERROR_INVALID_ARGUMENT}
},

{
    .test_desc                  = "Test psa_aead_encrypt - Invalid tag length 0\n",
    .type                       = PSA_KEY_TYPE_AES,
    .data                       = key_data,
    .data_length                = AES_16B_KEY_SIZE,
    .usage_flags                = PSA_KEY_USAGE_ENCRYPT,
    .key_alg                    = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0),
    .aead_alg                   = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 0),
    .nonce                      = nonce,
    .nonce_length               = 13,
    .additional_data            = additional_data,
    .additional_data_length     = 32,
    .plaintext                  = plaintext,
    .plaintext_length           = 24,
    .ciphertext                 = expected_output,
    .expected_ciphertext        = aead_ciphertext_2,
    .ciphertext_size            = BUFFER_SIZE,
    .expected_ciphertext_length = AEAD_CIPHERTEXT_LEN_2,
    .expected_status            = {PSA_ERROR_INVALID_ARGUMENT, PSA_ERROR_INVALID_ARGUMENT}
},
#endif
#endif
};
