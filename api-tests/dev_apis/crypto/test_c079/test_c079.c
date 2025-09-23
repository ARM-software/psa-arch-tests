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
#include "test_c079.h"
#include "test_data.h"

int32_t psa_pake_spake2p_setup(psa_pake_operation_t *op, const uint8_t *user, const uint8_t *peer,
                               const uint8_t *context, psa_algorithm_t alg, psa_pake_role_t role,
                               psa_key_id_t key)
{
  psa_status_t status;
  psa_pake_cipher_suite_t cipher_suite = PSA_PAKE_CIPHER_SUITE_INIT;
  psa_pake_primitive_t pake_primitive =
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
                        alg);
   val->crypto_function(VAL_CRYPTO_PAKE_CS_SET_PRIMITIVE,
                        &cipher_suite,
                        pake_primitive);
   val->crypto_function(VAL_CRYPTO_PAKE_CS_SET_KEY_CONFIRMATION,
                        &cipher_suite,
                        PSA_PAKE_CONFIRMED_KEY);

   /* Setup PAKE operation object */
    status = val->crypto_function(VAL_CRYPTO_PAKE_SETUP,
                                  op,
                                  key,
                                  &cipher_suite);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

    /* Set role for PAKE Operation */
    status = val->crypto_function(VAL_CRYPTO_PAKE_SET_ROLE,
                                  op,
                                  role);
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

   /* Set context for PAKE operation */
    status = val->crypto_function(VAL_CRYPTO_PAKE_SET_CONTEXT,
                                  op,
                                  context,
                                  sizeof(context));
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));

  return status;
}

int32_t send_message_spake2p(psa_pake_operation_t *from, psa_pake_operation_t *to,
                             psa_pake_step_t step)
{
  int32_t status;
  uint8_t data[1024];
  size_t op_len;

  status = val->crypto_function(VAL_CRYPTO_PAKE_OUTPUT, from, step, data, sizeof(data), &op_len);

  status = val->crypto_function(VAL_CRYPTO_PAKE_INPUT, to, step, data, op_len);

 return status;
}

const client_test_t test_c079_crypto_list[] = {
    NULL,
    psa_pake_spake2p_test,
    NULL,
};

extern  uint32_t g_test_count;

int32_t psa_pake_spake2p_test(caller_security_t caller __UNUSED)
{
    uint8_t i = 0, j = 0;
    int32_t num_checks = sizeof(check1)/sizeof(check1[0]);
    int32_t status;
    psa_pake_operation_t client = PSA_PAKE_OPERATION_INIT;
    psa_pake_operation_t server = PSA_PAKE_OPERATION_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_derivation_operation_t kdf = PSA_KEY_DERIVATION_OPERATION_INIT;
    psa_key_id_t ckey = 0, skey = 0;
    psa_key_id_t shared_key = 0;
    uint8_t secret1[32], secret2[32], pub_key[97];
    size_t pub_key_len;

    if (num_checks == 0)
    {
        val->print(TEST, "No test available for the selected crypto configuration\n", 0);
        return RESULT_SKIP(VAL_STATUS_NO_TESTS);
    }

    /* Initialize PSA crypto library */
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    for (j = 0 ; j < num_checks ; j++) {

    val->print(TEST, "Check %d: ", g_test_count++);
    val->print(TEST, check1[j].test_desc, 0);

    /* Setting up the watchdog timer for each check */
    status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
    TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));


    /* Setup the attributes for the key */
    val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,
                         &attributes,
                         PSA_KEY_USAGE_DERIVE);
    val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM,
                         &attributes,
                         check1[j].alg);
    val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS,
                         &attributes,
                         PSA_KEY_USAGE_DERIVE);
    val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,
                         &attributes,
                         PSA_KEY_TYPE_SPAKE2P_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));

    /* Import the client key data into the key slot */
    status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY,
                                  &attributes,
                                  spake2p_key_pair,
                                  64,
                                  &ckey);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

    status = val->crypto_function(VAL_CRYPTO_EXPORT_PUBLIC_KEY,
                                  ckey,
                                  pub_key,
                                  sizeof(pub_key),
                                  &pub_key_len);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

    TEST_ASSERT_MEMCMP(spake2p_pub_key, pub_key, pub_key_len, TEST_CHECKPOINT_NUM(5));

    /* Setup the key type for server key */
     status = val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,
                                   &attributes,
                                   PSA_KEY_TYPE_SPAKE2P_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));

    /* Import the server key data into the key slot */
    status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY,
                                  &attributes,
                                  spake2p_pub_key,
                                  97,
                                  &skey);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));

   /* Setup spake2p operation for prover/client */
    status = psa_pake_spake2p_setup(&client, (const uint8_t *)"client", (const uint8_t *)"server",
                            (const uint8_t *)"context", check1[j].alg, PSA_PAKE_ROLE_CLIENT, ckey);
    val->print(DBG, "       1 : SPAKE2P operation setup for prover done \n", 0);

  /* Setup spake2p operation for verifier/server */
   status = psa_pake_spake2p_setup(&server, (const uint8_t *)"server", (const uint8_t *)"client",
                            (const uint8_t *)"context", check1[j].alg, PSA_PAKE_ROLE_SERVER, skey);
   val->print(DBG, "       2 : SPAKE2P operation setup for verifier done \n", 0);

   /* Starting key exchange operation */

   // ShareP
   status = send_message_spake2p(&client, &server, PSA_PAKE_STEP_KEY_SHARE);
   TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));

   // ShareV
   status = send_message_spake2p(&server, &client, PSA_PAKE_STEP_KEY_SHARE);
   TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));

   //ConfirmV
   status = send_message_spake2p(&server, &client, PSA_PAKE_STEP_CONFIRM);
   TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(9));

   //ConfirmP
   status = send_message_spake2p(&client, &server, PSA_PAKE_STEP_CONFIRM);
   TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(10));

   val->print(DBG, "       3 : SPAKE2P key exchange between prover and verifier done\n", 0);
   /* Extract Shared Secret as a key derivation key */

   /* Setup the attributes for the key */
     val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,
                          &attributes,
                          PSA_KEY_TYPE_DERIVE);
     val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS,
                          &attributes,
                          PSA_KEY_USAGE_DERIVE);
     val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM,
                          &attributes,
                          PSA_ALG_HKDF(PSA_ALG_SHA_256));

     val->print(DBG, "       4 : Derive shared secret from prover \n", 0);

    /* Setup Key Derivation for prover/client */
     status = val->crypto_function(VAL_CRYPTO_PAKE_GET_SHARED_KEY,
                                   &client, &attributes, &shared_key);
     TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(11));

     status = val->crypto_function(VAL_CRYPTO_PAKE_ABORT, &client);
     TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(12));

     status =  val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SETUP,
                                    &kdf, PSA_ALG_HKDF(PSA_ALG_SHA_256));
     TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(13));

     status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_INPUT_KEY,
                                &kdf, PSA_KEY_DERIVATION_INPUT_SECRET,
                                shared_key);
     TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(14));

     status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_INPUT_BYTES,
                                   &kdf, PSA_KEY_DERIVATION_INPUT_INFO,
                                   input_bytes_data, INPUT_BYTES_DATA_LEN);
     TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(15));

     status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_OUTPUT_BYTES,
                                &kdf, secret1, sizeof(secret1));
     TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(16));

     status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_ABORT, &kdf);
     TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(17));

     val->print(DBG, " secret1 : ", 0);
     for (i = 0; i < 32; i++)
      {
        val->print(DBG, "%x", secret1[i]);
      }
      val->print(DBG, "\n", 0);

     status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, shared_key);
     TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(18));

     val->print(DBG, "       5 : Derive shared secret from verifier \n", 0);

    /* Setup Key Derivation for prover/client */
     status = val->crypto_function(VAL_CRYPTO_PAKE_GET_SHARED_KEY,
                                   &server, &attributes, &shared_key);
     TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(19));

     status = val->crypto_function(VAL_CRYPTO_PAKE_ABORT, &server);
     TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(20));

     status =  val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SETUP,
                                    &kdf, PSA_ALG_HKDF(PSA_ALG_SHA_256));
     TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(21));

     status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_INPUT_KEY,
                                &kdf, PSA_KEY_DERIVATION_INPUT_SECRET,
                                shared_key);
     TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(22));

     status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_INPUT_BYTES,
                                   &kdf, PSA_KEY_DERIVATION_INPUT_INFO,
                                   input_bytes_data, INPUT_BYTES_DATA_LEN);
     TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(23));

     status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_OUTPUT_BYTES,
                                   &kdf, secret2, sizeof(secret2));
     TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(24));

     status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_ABORT, &kdf);
     TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(25));

     val->print(DBG, " secret2 : ", 0);
     for (i = 0; i < 32; i++)
      {
        val->print(DBG, "%x", secret2[i]);
      }

       val->print(DBG, "\n", 0);

       status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, shared_key);
       TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(26));

       TEST_ASSERT_MEMCMP(secret1, secret2, sizeof(secret1), TEST_CHECKPOINT_NUM(27));

       status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, ckey);
       TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(28));

       status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, skey);
       TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(29));
}
  return VAL_STATUS_SUCCESS;
}
