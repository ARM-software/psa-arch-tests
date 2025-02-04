/** @file
 * Copyright (c) 2019-2025, Arm Limited or its affiliates. All rights reserved.
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
    char                        test_desc[85];
    psa_key_type_t              type;
    const uint8_t              *data;
    size_t                      data_length;
    psa_key_usage_t             usage_flags;
    psa_algorithm_t             alg;
    psa_key_derivation_step_t   step;
    const uint8_t              *peer_key;
    size_t                      peer_key_length;
    psa_status_t                expected_status;
} test_data;


static const test_data check1[] = {
#ifdef ARCH_TEST_ECDH
#ifdef ARCH_TEST_ECC_CURVE_SECP256R1
{
    .test_desc       = "Test psa_key_derivation_key_agreement - ECDH SECP256R1[ECDH]\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data            = ecdh_secp_256_r1_prv_key,
    .data_length     = ECDH_SECP_256_R1_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .peer_key        = ecdh_secp_256_r1_pub_key,
    .peer_key_length = ECDH_SECP_256_R1_PUB_KEY_LEN,
    .expected_status = PSA_SUCCESS
},
{
    .test_desc       = "Test psa_key_derivation_key_agreement - TLS12_PSK_TO_MS_SECP256R1[ECDH]\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data            = ecdh_secp_256_r1_prv_key,
    .data_length     = ECDH_SECP_256_R1_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg            = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_OTHER_SECRET,
    .peer_key        = ecdh_secp_256_r1_pub_key,
    .peer_key_length = ECDH_SECP_256_R1_PUB_KEY_LEN,
    .expected_status = PSA_SUCCESS
},
{
    .test_desc       = "Test psa_key_derivation_key_agreement - Invalid step[ECDH]\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data            = ecdh_secp_256_r1_prv_key,
    .data_length     = ECDH_SECP_256_R1_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_LABEL,
    .peer_key        = ecdh_secp_256_r1_pub_key,
    .peer_key_length = ECDH_SECP_256_R1_PUB_KEY_LEN,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},

#endif

#ifdef ARCH_TEST_ECC_CURVE_SECP384R1
{
    .test_desc       = "Test psa_key_derivation_key_agreement - ECDH SECP384R1[ECDH]\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data            = ecdh_secp_384_r1_prv_key,
    .data_length     = ECDH_SECP_384_R1_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .peer_key        = ecdh_secp_384_r1_pub_key,
    .peer_key_length = ECDH_SECP_384_R1_PUB_KEY_LEN,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_ECC_CURVE_SECP256R1
{
    .test_desc       = "Test psa_key_derivation_key_agreement - Invalid usage[ECDH]\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data            = ecdh_secp_256_r1_prv_key,
    .data_length     = ECDH_SECP_256_R1_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_ENCRYPT,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .peer_key        = ecdh_secp_256_r1_pub_key,
    .peer_key_length = ECDH_SECP_256_R1_PUB_KEY_LEN,
    .expected_status = PSA_ERROR_NOT_PERMITTED
},
#endif
#endif

#ifdef ARCH_TEST_ECC_CURVE_SECP256R1
#ifdef ARCH_TEST_SHA256
#ifdef ARCH_TEST_HKDF
{
    .test_desc       = "Test psa_key_derivation_key_agreement - KDF not a key agreement alg[ECDH]\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data            = ecdh_secp_256_r1_prv_key,
    .data_length     = ECDH_SECP_256_R1_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .peer_key        = ecdh_secp_256_r1_pub_key,
    .peer_key_length = ECDH_SECP_256_R1_PUB_KEY_LEN,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
#endif
#endif

#ifdef ARCH_TEST_ECDH
{
    .test_desc       = "Test psa_key_derivation_key_agreement - Public key of different curve[ECDH]\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data            = ecdh_secp_256_r1_prv_key,
    .data_length     = ECDH_SECP_256_R1_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .peer_key        = ecdh_secp_384_r1_pub_key,
    .peer_key_length = ECDH_SECP_384_R1_PUB_KEY_LEN,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
{
    .test_desc       = "Test psa_key_derivation_key_agreement - invalid input for the step\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data            = ecdh_secp_256_r1_prv_key,
    .data_length     = ECDH_SECP_256_R1_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg            = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_INFO,
    .peer_key        = ecdh_secp_256_r1_pub_key,
    .peer_key_length = ECDH_SECP_256_R1_PUB_KEY_LEN,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},

{
    .test_desc       = "Test psa_key_derivation_key_agreement - Pub key instead of Prv key[ECDH]\n",
    .type            = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1),
    .data            = ecdh_secp_256_r1_pub_key,
    .data_length     = ECDH_SECP_256_R1_PUB_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .peer_key        = ecdh_secp_256_r1_pub_key,
    .peer_key_length = ECDH_SECP_256_R1_PUB_KEY_LEN,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
}
#ifdef ARCH_TEST_FFDH
,
#endif

#endif
#endif


#ifdef ARCH_TEST_FFDH
#ifdef ARCH_TEST_SHA256
#ifdef ARCH_TEST_HKDF

#ifdef ARCH_TEST_FFDHE2048
{
    .test_desc       = "Test psa_key_derivation_key_agreement - FFDH FFDHE2048[FFDH]\n",
    .type            = PSA_KEY_TYPE_DH_KEY_PAIR(PSA_DH_FAMILY_RFC7919),
    .data            = ffdh_key_data_2048,
    .data_length     = FFDH2048_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_FFDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .peer_key        = ffdh_peer_key_data_2048,
    .peer_key_length = FFDH2048_PUB_KEY_LEN,
    .expected_status = PSA_SUCCESS
},

{
    .test_desc       = "Test psa_key_derivation_key_agreement - Invalid step[FFDH]\n",
    .type            = PSA_KEY_TYPE_DH_KEY_PAIR(PSA_DH_FAMILY_RFC7919),
    .data            = ffdh_key_data_2048,
    .data_length     = FFDH2048_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_FFDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_LABEL,
    .peer_key        = ffdh_peer_key_data_2048,
    .peer_key_length = FFDH2048_PUB_KEY_LEN,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},

{
    .test_desc       = "Test psa_key_derivation_key_agreement - Invalid usage[FFDH]\n",
    .type            = PSA_KEY_TYPE_DH_KEY_PAIR(PSA_DH_FAMILY_RFC7919),
    .data            = ffdh_key_data_2048,
    .data_length     = FFDH2048_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_ENCRYPT,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_FFDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .peer_key        = ffdh_peer_key_data_2048,
    .peer_key_length = FFDH2048_PUB_KEY_LEN,
    .expected_status = PSA_ERROR_NOT_PERMITTED
},

{
    .test_desc       = "Test psa_key_derivation_key_agreement - KDF not a key agreement alg[FFDH]\n",
    .type            = PSA_KEY_TYPE_DH_KEY_PAIR(PSA_DH_FAMILY_RFC7919),
    .data            = ffdh_key_data_2048,
    .data_length     = FFDH2048_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_HKDF(PSA_ALG_SHA_256),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .peer_key        = ffdh_peer_key_data_2048,
    .peer_key_length = FFDH2048_PUB_KEY_LEN,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
{
    .test_desc       = "Test psa_key_derivation_key_agreement - Public key of different curve[FFDH]\n",
    .type            = PSA_KEY_TYPE_DH_KEY_PAIR(PSA_DH_FAMILY_RFC7919),
    .data            = ffdh_key_data_2048,
    .data_length     = FFDH2048_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_FFDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .peer_key        = ffdh_peer_key_data_4096,
    .peer_key_length = FFDH4096_PUB_KEY_LEN,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
{
    .test_desc       = "Test psa_key_derivation_key_agreement-Public key different curve[FFDH]\n",
    .type            = PSA_KEY_TYPE_DH_KEY_PAIR(PSA_DH_FAMILY_RFC7919),
    .data            = ffdh_key_data_2048,
    .data_length     = FFDH2048_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg            = PSA_ALG_KEY_AGREEMENT(PSA_ALG_FFDH, PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .peer_key        = ffdh_peer_key_data_2048,
    .peer_key_length = FFDH4096_PUB_KEY_LEN,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},

{
    .test_desc       = "Test psa_key_derivation_key_agreement - Pub key instead of Prv key[FFDH]\n",
    .type            = PSA_KEY_TYPE_DH_PUBLIC_KEY(PSA_DH_FAMILY_RFC7919),
    .data            = ffdh_peer_key_data_2048,
    .data_length     = FFDH2048_PUB_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_FFDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .peer_key        = ffdh_key_data_2048,
    .peer_key_length = FFDH2048_PRV_KEY_LEN,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
#endif

#ifdef ARCH_TEST_FFDHE3072
{
    .test_desc       = "Test psa_key_derivation_key_agreement - FFDH3072[FFDH]\n",
    .type            = PSA_KEY_TYPE_DH_KEY_PAIR(PSA_DH_FAMILY_RFC7919),
    .data            = ffdh_key_data_3072,
    .data_length     = FFDH3072_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_FFDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .peer_key        = ffdh_peer_key_data_3072,
    .peer_key_length = FFDH3072_PUB_KEY_LEN,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_FFDHE4096
{
    .test_desc       = "Test psa_key_derivation_key_agreement - FFDH4096[FFDH]\n",
    .type            = PSA_KEY_TYPE_DH_KEY_PAIR(PSA_DH_FAMILY_RFC7919),
    .data            = ffdh_key_data_4096,
    .data_length     = FFDH4096_PRV_KEY_LEN,
    .usage_flags     = PSA_KEY_USAGE_DERIVE,
    .alg             = PSA_ALG_KEY_AGREEMENT(PSA_ALG_FFDH, PSA_ALG_HKDF(PSA_ALG_SHA_256)),
    .step            = PSA_KEY_DERIVATION_INPUT_SECRET,
    .peer_key        = ffdh_peer_key_data_4096,
    .peer_key_length = FFDH4096_PUB_KEY_LEN,
    .expected_status = PSA_SUCCESS
}
#endif

#endif
#endif
#endif


};
