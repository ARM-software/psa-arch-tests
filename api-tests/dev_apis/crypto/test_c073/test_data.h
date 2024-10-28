/** @file
 * Copyright (c) 2024, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
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
    psa_key_usage_t         usage_flag;
    psa_algorithm_t         alg;
    psa_pake_primitive_t    pake_primitive;
    psa_pake_role_t         role;
    const uint8_t          *user_id;
    const uint8_t          *peer_id;
    const uint8_t          *context;
    size_t                  context_len;
    size_t                  user_id_len;
    size_t                  peer_id_len;
    uint32_t                key_confirmation;
    psa_status_t            expected_status;
} test_data;

#if (defined(ARCH_TEST_ECC_CURVE_SECP256R1) && (defined(ARCH_TEST_SHA256)))
static const test_data check1[] = {
#ifdef ARCH_TEST_SPAKE2P
#ifdef ARCH_TEST_SPAKE2P_HMAC
{
  .test_desc        = "Test psa_pake_set_context - SPAKE2P HMAC - SHA256 \n",
  .type             = PSA_KEY_TYPE_SPAKE2P_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
  .data             = spake2p_key_pair,
  .data_length      = 64,
  .usage_flag       = PSA_KEY_USAGE_DERIVE,
  .alg              = PSA_ALG_SPAKE2P_HMAC(PSA_ALG_SHA_256),
  .pake_primitive   = PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC, PSA_ECC_FAMILY_SECP_R1, 256),
  .user_id          = userid,
  .user_id_len      = sizeof(userid),
  .peer_id          = peerid,
  .peer_id_len      = sizeof(peerid),
  .context          = context,
  .context_len      = CONTEXT_LEN,
  .role             = PSA_PAKE_ROLE_CLIENT,
  .key_confirmation = 0,
  .expected_status  = PSA_SUCCESS
},
{
  .test_desc        = "Test psa_pake_set_context - SPAKE2P HMAC - SHA256 \n",
  .type             = PSA_KEY_TYPE_SPAKE2P_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1),
  .data             = spake2p_pub_key,
  .data_length      = 97,
  .usage_flag       = PSA_KEY_USAGE_DERIVE,
  .alg              = PSA_ALG_SPAKE2P_HMAC(PSA_ALG_SHA_256),
  .pake_primitive   = PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC, PSA_ECC_FAMILY_SECP_R1, 256),
  .user_id          = userid,
  .user_id_len      = sizeof(userid),
  .peer_id          = peerid,
  .peer_id_len      = sizeof(peerid),
  .context          = context,
  .context_len      = 0,
  .role             = PSA_PAKE_ROLE_SERVER,
  .key_confirmation = 0,
  .expected_status  = PSA_SUCCESS
},
#endif
#ifdef ARCH_TEST_SPAKE2P_CMAC
{
  .test_desc        = "Test psa_pake_set_context - SPAKE2P CMAC - SHA256 \n",
  .type             = PSA_KEY_TYPE_SPAKE2P_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
  .data             = spake2p_key_pair,
  .data_length      = 64,
  .usage_flag       = PSA_KEY_USAGE_DERIVE,
  .alg              = PSA_ALG_SPAKE2P_CMAC(PSA_ALG_SHA_256),
  .pake_primitive   = PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC, PSA_ECC_FAMILY_SECP_R1, 256),
  .user_id          = userid,
  .user_id_len      = sizeof(userid),
  .peer_id          = peerid,
  .peer_id_len      = sizeof(peerid),
  .context          = context,
  .context_len      = CONTEXT_LEN,
  .role             = PSA_PAKE_ROLE_CLIENT,
  .key_confirmation = 0,
  .expected_status  = PSA_SUCCESS
},
#endif
#ifdef ARCH_TEST_SPAKE2P_MATTER
{
  .test_desc        = "Test psa_pake_set_context - SPAKE2P MATTER - SHA256 \n",
  .type             = PSA_KEY_TYPE_SPAKE2P_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
  .data             = spake2p_key_pair,
  .data_length      = 64,
  .usage_flag       = PSA_KEY_USAGE_DERIVE,
  .alg              = PSA_ALG_SPAKE2P_MATTER,
  .pake_primitive   = PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC, PSA_ECC_FAMILY_SECP_R1, 256),
  .user_id          = userid,
  .user_id_len      = sizeof(userid),
  .peer_id          = peerid,
  .peer_id_len      = PEER_ID_LEN,
  .context          = context,
  .context_len      = CONTEXT_LEN,
  .role             = PSA_PAKE_ROLE_CLIENT,
  .key_confirmation = 0,
  .expected_status  = PSA_SUCCESS
},
#endif
#ifdef ARCH_TEST_SPAKE2P_HMAC
{
  .test_desc        = "Test psa_pake_set_context - SPAKE2P HMAC-SHA256 - invalid context \n",
  .type             = PSA_KEY_TYPE_SPAKE2P_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1),
  .data             = spake2p_pub_key,
  .data_length      = 97,
  .usage_flag       = PSA_KEY_USAGE_DERIVE,
  .alg              = PSA_ALG_SPAKE2P_HMAC(PSA_ALG_SHA_256),
  .pake_primitive   = PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC, PSA_ECC_FAMILY_SECP_R1, 256),
  .user_id          = userid,
  .user_id_len      = sizeof(userid),
  .peer_id          = peerid,
  .peer_id_len      = sizeof(peerid),
  .context          = NULL,
  .context_len      = CONTEXT_LEN,
  .role             = PSA_PAKE_ROLE_SERVER,
  .key_confirmation = 0,
  .expected_status  = PSA_ERROR_INVALID_ARGUMENT
},
#endif
#ifdef ARCH_TEST_SPAKE2P_CMAC
{
  .test_desc        = "Test psa_pake_set_context - SPAKE2P CMAC-SHA256 - invalid context\n",
  .type             = PSA_KEY_TYPE_SPAKE2P_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1),
  .data             = spake2p_pub_key,
  .data_length      = 97,
  .usage_flag       = PSA_KEY_USAGE_DERIVE,
  .alg              = PSA_ALG_SPAKE2P_CMAC(PSA_ALG_SHA_256),
  .pake_primitive   = PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC, PSA_ECC_FAMILY_SECP_R1, 256),
  .user_id          = userid,
  .user_id_len      = sizeof(userid),
  .peer_id          = peerid,
  .peer_id_len      = sizeof(peerid),
  .context          = NULL,
  .context_len      = CONTEXT_LEN,
  .role             = PSA_PAKE_ROLE_SERVER,
  .key_confirmation = 0,
  .expected_status  = PSA_ERROR_INVALID_ARGUMENT
},
#endif
#endif
#ifdef ARCH_TEST_JPAKE
{
  .test_desc        = "Test psa_pake_set_context - JPAKE-SHA256 - invalid algorithm\n",
  .type             = PSA_KEY_TYPE_PASSWORD,
  .data             = input_password,
  .data_length      = INPUT_PASSWORD_LEN,
  .usage_flag       = PSA_KEY_USAGE_DERIVE,
  .alg              = PSA_ALG_JPAKE(PSA_ALG_SHA_256),
  .pake_primitive   = PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC, PSA_ECC_FAMILY_SECP_R1, 256),
  .role             = PSA_PAKE_ROLE_NONE,
  .user_id          = userid,
  .user_id_len      = USER_ID_LEN,
  .peer_id          = peerid,
  .peer_id_len      = PEER_ID_LEN,
  .context          = context,
  .context_len      = CONTEXT_LEN,
  .key_confirmation = 1,
  .expected_status  = PSA_ERROR_BAD_STATE
 },
#endif
};
#endif
