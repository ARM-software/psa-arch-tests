/** @file
 * Copyright (c) 2024-2025, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
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

#include "val_interfaces.h"
#include "val_target.h"
#include "test_c078.h"
#include "test_crypto_common.h"

const client_test_t test_c078_crypto_list[] = {
    NULL,
    psa_pake_jpake_test,
    NULL,
};

int32_t psa_pake_jpake_setup(psa_pake_operation_t *op, const uint8_t *user, const uint8_t *peer,
                              psa_key_id_t pw_key)
{
  psa_status_t status;
  psa_pake_cipher_suite_t cipher_suite = PSA_PAKE_CIPHER_SUITE_INIT;
  psa_pake_primitive_t jpake_primitive =
        PSA_PAKE_PRIMITIVE(PSA_PAKE_PRIMITIVE_TYPE_ECC, PSA_ECC_FAMILY_SECP_R1, 256);

  /* Initialize PSA crypto library */
  status = val->crypto_function(VAL_CRYPTO_INIT);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

  /* Setting up the watchdog timer for each check */
  status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
  TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));


   /* Setup cipher_suite object parameters */
   val->crypto_function(VAL_CRYPTO_PAKE_CS_SET_ALGORITHM,
                        &cipher_suite,
                        PSA_ALG_JPAKE(PSA_ALG_SHA_256));
   val->crypto_function(VAL_CRYPTO_PAKE_CS_SET_PRIMITIVE,
                        &cipher_suite,
                        jpake_primitive);
   val->crypto_function(VAL_CRYPTO_PAKE_CS_SET_KEY_CONFIRMATION,
                        &cipher_suite,
                        PSA_PAKE_UNCONFIRMED_KEY);

   /* Setup PAKE operation object */
    status = val->crypto_function(VAL_CRYPTO_PAKE_SETUP,
                                   op,
                                   pw_key,
                                   &cipher_suite);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

   /* Set role for PAKE Operation */
    status = val->crypto_function(VAL_CRYPTO_PAKE_SET_ROLE,
                                  op,
                                  PSA_PAKE_ROLE_NONE);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

   /* Set user ID for PAKE operation */
    status = val->crypto_function(VAL_CRYPTO_PAKE_SET_USER,
                                  op,
                                  user,
                                  sizeof(user));
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

   /* Set peer ID for PAKE operation */
    status = val->crypto_function(VAL_CRYPTO_PAKE_SET_PEER,
	                          op,
                                  peer,
                                  sizeof(peer));
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));

   return status;
}

int32_t send_message_jpake(psa_pake_operation_t *from, psa_pake_operation_t *to,
                           psa_pake_step_t step)
{
  int32_t status;
  uint8_t data[1024];
  size_t op_len;

  status = val->crypto_function(VAL_CRYPTO_PAKE_OUTPUT, from, step, data, sizeof(data), &op_len);

  status = val->crypto_function(VAL_CRYPTO_PAKE_INPUT, to, step, data, op_len);

 return status;
}

int32_t psa_pake_jpake_test(caller_security_t caller __UNUSED)
{
#if defined(ARCH_TEST_JPAKE)
    uint8_t i = 0;
    int32_t status;
    psa_pake_operation_t user = PSA_PAKE_OPERATION_INIT;
    psa_pake_operation_t peer = PSA_PAKE_OPERATION_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_derivation_operation_t kdf = PSA_KEY_DERIVATION_OPERATION_INIT;
    psa_key_id_t pw_key = 0;
    psa_key_id_t key = 0;
    uint8_t secret1[32], secret2[32];

    val->print(PRINT_ALWAYS, "[check 1] JPAKE Test \n", 0);
    /* Initialize PSA crypto library */
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    /* Setting up the watchdog timer for each check */
    status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
    TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

    /* Setup the attributes for the key */
    val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,
                         &attributes,
                         PSA_KEY_TYPE_PASSWORD);
    val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS,
                         &attributes,
                         PSA_KEY_USAGE_DERIVE);
    val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM,
                         &attributes,
                         PSA_ALG_JPAKE(PSA_ALG_SHA_256));

   /* Import the key data into the key slot */
    status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY,
                                  &attributes,
                                  input_password,
                                  INPUT_PASSWORD_LEN,
                                  &pw_key);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

   /* setup Jpake operation for user */
    status = psa_pake_jpake_setup(&user, (const uint8_t *)"client", (const uint8_t *)"server",
                                   pw_key);
    val->print(PRINT_DEBUG, "[check 1] : JPAKE operation setup for user done\n", 0);

    /* Setup jpake operation for peer */
    status = psa_pake_jpake_setup(&peer, (const uint8_t *)"server", (const uint8_t *)"client",
                                    pw_key);
    val->print(PRINT_DEBUG, "[check 2] : JPAKE operation setup for peer done\n", 0);
    /* Round 1 key exchange */
    val->print(PRINT_DEBUG, "[check 3] : Starting Round1 Key Exchange from user to peer\n", 0);
    // Get and set g1
    status = send_message_jpake(&user, &peer, PSA_PAKE_STEP_KEY_SHARE);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

    // Get and set V1, ZKP public key for x1
    status = send_message_jpake(&user, &peer, PSA_PAKE_STEP_ZK_PUBLIC);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

   // Get and set r1, ZKP proof for x1
    status = send_message_jpake(&user, &peer, PSA_PAKE_STEP_ZK_PROOF);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));

   // Get and set  g2
   status = send_message_jpake(&user, &peer, PSA_PAKE_STEP_KEY_SHARE);
   TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));

   // Get V2, ZKP public key for x2
   status = send_message_jpake(&user, &peer, PSA_PAKE_STEP_ZK_PUBLIC);
   TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));

   // Get r2, ZKP proof for x2
   status = send_message_jpake(&user, &peer, PSA_PAKE_STEP_ZK_PROOF);
   TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(9));

   val->print(PRINT_DEBUG, "            Round1 Key Exchange from user to peer done\n", 0);

   val->print(PRINT_DEBUG, "[check 4] : Starting Round1 Key Exchange from peer to user\n", 0);

   //Get and set g3
   status = send_message_jpake(&peer, &user, PSA_PAKE_STEP_KEY_SHARE);
   TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(10));

   // Get and set V3, ZKP public key for x3
   status = send_message_jpake(&peer, &user, PSA_PAKE_STEP_ZK_PUBLIC);
   TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(11));

   // get and set r3, ZKP proof for x3
   status = send_message_jpake(&peer, &user, PSA_PAKE_STEP_ZK_PROOF);
   TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(12));

   // get and set g4
   status = send_message_jpake(&peer, &user, PSA_PAKE_STEP_KEY_SHARE);
   TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(13));

   // Get and set V4, ZKP public key for x4
   status = send_message_jpake(&peer, &user, PSA_PAKE_STEP_ZK_PUBLIC);
   TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(14));

   // Get and set r4, ZKP proof for x4
   status = send_message_jpake(&peer, &user, PSA_PAKE_STEP_ZK_PROOF);
   TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(15));

   val->print(PRINT_DEBUG, "            Round1 Key Exchange from peer to user done\n", 0);

  /* Round 2 Key exchange */

    val->print(PRINT_DEBUG, "[check 5] : Starting Round2 Key Exchange from user to peer\n", 0);

   // Get and set A
   status = send_message_jpake(&user, &peer, PSA_PAKE_STEP_KEY_SHARE);
   TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(16));

   //Get and set V5, ZKP public key for x2*s
   status = send_message_jpake(&user, &peer, PSA_PAKE_STEP_ZK_PUBLIC);
   TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(17));

  // Get and set r5, ZKP proof for x2*s
  status = send_message_jpake(&user, &peer, PSA_PAKE_STEP_ZK_PROOF);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(18));

   val->print(PRINT_DEBUG, "            Round2 Key Exchange from user to peer done\n", 0);

   val->print(PRINT_DEBUG, "[check 6] : Starting Round2 Key Exchange from peer to user\n", 0);

  // Get and Set B
  status = send_message_jpake(&peer, &user, PSA_PAKE_STEP_KEY_SHARE);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(19));

  // Get and Set V6, ZKP public key for x4*s
  status = send_message_jpake(&peer, &user, PSA_PAKE_STEP_ZK_PUBLIC);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(20));

  // Get and set r6, ZKP proof for x4*s
  status = send_message_jpake(&peer, &user, PSA_PAKE_STEP_ZK_PROOF);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(21));

  val->print(PRINT_DEBUG, "            Round2 Key Exchange from peer to user done\n", 0);

  /* Setup the attributes for the key */
  val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,
                       &attributes,
                       PSA_KEY_TYPE_DERIVE);
  val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS,
                       &attributes,
                       PSA_KEY_USAGE_DERIVE);
  val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM,
                       &attributes,
                       PSA_ALG_TLS12_ECJPAKE_TO_PMS);

  val->print(PRINT_DEBUG, "[check 7] : Derive shared secret from user \n", 0);

  /* Setup Key Derivation for User */
  status = val->crypto_function(VAL_CRYPTO_PAKE_GET_SHARED_KEY, &user, &attributes, &key);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(22));

  status = val->crypto_function(VAL_CRYPTO_PAKE_ABORT, &user);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(23));

  status =  val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SETUP,
                       &kdf, PSA_ALG_TLS12_ECJPAKE_TO_PMS);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(24));

  status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_INPUT_KEY,
                                &kdf, PSA_KEY_DERIVATION_INPUT_SECRET,
                                key);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(25));

  status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_OUTPUT_BYTES,
                                &kdf, secret1, sizeof(secret1));
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(26));

  status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_ABORT, &kdf);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(27));

  val->print(PRINT_DEBUG, "[secret1] : ", 0);
  for (i = 0; i < 32; i++)
  {
    val->print(PRINT_DEBUG, "%x", secret1[i]);
  }

  status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, key);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(28));

  val->print(PRINT_DEBUG, "\n[check 8] : Derive shared secret from peer \n", 0);

  /* Setup Key Derivation for Peer */
  status = val->crypto_function(VAL_CRYPTO_PAKE_GET_SHARED_KEY, &peer, &attributes, &key);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(29));

  status = val->crypto_function(VAL_CRYPTO_PAKE_ABORT, &peer);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(30));

  status =  val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SETUP,
                                 &kdf, PSA_ALG_TLS12_ECJPAKE_TO_PMS);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(31));

  status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_INPUT_KEY,
                                &kdf, PSA_KEY_DERIVATION_INPUT_SECRET,
                                key);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(32));

  status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_OUTPUT_BYTES,
                               &kdf, secret2, sizeof(secret2));
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(33));

  val->print(PRINT_DEBUG, "[secret2] : ", 0);

  for (i = 0; i < 32; i++)
  {
    val->print(PRINT_DEBUG, "%x", secret2[i]);
  }

  val->print(PRINT_DEBUG, "\n", 0);

  status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_ABORT, &kdf);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(34));

  status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, key);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(35));

  TEST_ASSERT_MEMCMP(secret1, secret2, sizeof(secret1), TEST_CHECKPOINT_NUM(36));

  status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, pw_key);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(37));

  return VAL_STATUS_SUCCESS;

#else
  val->print(PRINT_TEST, "No test available for the selected crypto configuration\n", 0);
  return RESULT_SKIP(VAL_STATUS_NO_TESTS);
#endif
}
