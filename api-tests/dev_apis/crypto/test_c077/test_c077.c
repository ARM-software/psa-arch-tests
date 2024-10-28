/** @file
 * Copyright (c) 2024, Arm Limited or its affiliates. All rights reserved.
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
#include "test_c077.h"
#include "test_data.h"

const client_test_t test_c077_crypto_list[] = {
    NULL,
    psa_pake_get_shared_key_test,
    NULL,
};

extern  uint32_t g_test_count;

int32_t do_jpake_setup(psa_pake_operation_t *op, const uint8_t *user, const uint8_t *peer,
                       psa_key_id_t pw_key, psa_pake_role_t role,
                       psa_pake_cipher_suite_t *cipher_suite)
{
   psa_status_t status;

   val->print(PRINT_DEBUG, "Starting with jpake setup \n", 0);

  /* Initialize PSA crypto library */
  status = val->crypto_function(VAL_CRYPTO_INIT);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

  /* Setting up the watchdog timer for each check */
  status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
  TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

   /* Setup PAKE operation object */
    status = val->crypto_function(VAL_CRYPTO_PAKE_SETUP,
                                  op,
                                  pw_key,
                                  cipher_suite);
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

   return VAL_STATUS_SUCCESS;
}

int32_t send_message(psa_pake_operation_t *from, psa_pake_operation_t *to, psa_pake_step_t step,
                     uint8_t n)
{
  int32_t status;
  uint8_t data[1024];
  size_t op_len;

  status = val->crypto_function(VAL_CRYPTO_PAKE_OUTPUT, from, step, data, sizeof(data), &op_len);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(n));

  status = val->crypto_function(VAL_CRYPTO_PAKE_INPUT, to, step, data, op_len);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(n + 1));

 return VAL_STATUS_SUCCESS;
}

int32_t do_jpake_rounds(psa_pake_operation_t *user, psa_pake_operation_t *peer, int flag)
{
   psa_status_t status;
   psa_key_attributes_t    attributes = PSA_KEY_ATTRIBUTES_INIT;
   psa_key_id_t            derv_key;

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

   /* Initialize PSA crypto library */
   status = val->crypto_function(VAL_CRYPTO_INIT);
   TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

   /* Setting up the watchdog timer for each check */
   status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
   TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

   if (flag) {
  /* Excpect a bad state error if psa_pake_get_shared_key called before key-exchange is completed */
    status = val->crypto_function(VAL_CRYPTO_PAKE_GET_SHARED_KEY,
                                  user, &attributes, &derv_key);

    TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE, TEST_CHECKPOINT_NUM(3));

    val->print(PRINT_DEBUG, "Check for bad state is successful \n", 0);
   } else {

  // Get and set g1
   status = send_message(user, peer, PSA_PAKE_STEP_KEY_SHARE, 4);

   // Get and set V1, ZKP public key for x1
   status = send_message(user, peer, PSA_PAKE_STEP_ZK_PUBLIC, 6);

   //Get and set r1, ZKP proof for x1
   status = send_message(user, peer, PSA_PAKE_STEP_ZK_PROOF, 8);

   // Get and set  g2
   status = send_message(user, peer, PSA_PAKE_STEP_KEY_SHARE, 10);

   // Get V2, ZKP public key for x2
   status = send_message(user, peer, PSA_PAKE_STEP_ZK_PUBLIC, 12);

   // Get r2, ZKP proof for x2
   status = send_message(user, peer, PSA_PAKE_STEP_ZK_PROOF, 14);

   //Get and set g3
   status = send_message(peer, user, PSA_PAKE_STEP_KEY_SHARE, 16);

   // Get and set V3, ZKP public key for x3
   status = send_message(peer, user, PSA_PAKE_STEP_ZK_PUBLIC, 18);

   // get and set r3, ZKP proof for x3
   status = send_message(peer, user, PSA_PAKE_STEP_ZK_PROOF, 20);

   // get and set g4
   status = send_message(peer, user, PSA_PAKE_STEP_KEY_SHARE, 22);

   // Get and set V4, ZKP public key for x4
   status = send_message(peer, user, PSA_PAKE_STEP_ZK_PUBLIC, 24);

   //Get and set r4, ZKP proof for x4
    status = send_message(peer, user, PSA_PAKE_STEP_ZK_PROOF, 26);

   /* Round 2 Key exchange */

    // Get and set A
    status = send_message(user, peer, PSA_PAKE_STEP_KEY_SHARE, 28);

    //Get and set V5, ZKP public key for x2*s
    status = send_message(user, peer, PSA_PAKE_STEP_ZK_PUBLIC, 30);

    // Get and set r5, ZKP proof for x2*s
    status = send_message(user, peer, PSA_PAKE_STEP_ZK_PROOF, 32);

    // Get and Set B
    status = send_message(peer, user, PSA_PAKE_STEP_KEY_SHARE, 34);

    // Get and Set V6, ZKP public key for x4*s
    status = send_message(peer, user, PSA_PAKE_STEP_ZK_PUBLIC, 36);

    // Get and set r6, ZKP proof for x4*s
    status = send_message(peer, user, PSA_PAKE_STEP_ZK_PROOF, 38);
 }
  return VAL_STATUS_SUCCESS;
}

int32_t do_spake2p_setup(psa_pake_operation_t *op, const uint8_t *user, const uint8_t *peer,
                         const uint8_t *context, psa_key_id_t key, psa_pake_role_t role,
                         psa_pake_cipher_suite_t *cipher_suite)
{
  psa_status_t status;

  val->print(PRINT_DEBUG, "Starting with SPAKE2P setup...\n", 0);

  /* Initialize PSA crypto library */
  status = val->crypto_function(VAL_CRYPTO_INIT);
  TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

  /* Setting up the watchdog timer for each check */
  status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
  TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

   /* Setup PAKE operation object */
    status = val->crypto_function(VAL_CRYPTO_PAKE_SETUP,
                                   op,
                                   key,
                                   cipher_suite);
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

    /* Set context for the PAKE operation */
     status = val->crypto_function(VAL_CRYPTO_PAKE_SET_CONTEXT,
                                   op,
                                   context,
                                   sizeof(context));
     TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));

   return VAL_STATUS_SUCCESS;
}

int32_t do_spake2p_rounds(psa_pake_operation_t *client, psa_pake_operation_t *server, int flag)
{
   psa_status_t status;
   psa_key_attributes_t    attributes = PSA_KEY_ATTRIBUTES_INIT;
   psa_key_id_t            derv_key;

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

   /* Initialize PSA crypto library */
   status = val->crypto_function(VAL_CRYPTO_INIT);
   TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

   /* Setting up the watchdog timer for each check */
   status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
   TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

   if (flag) {
  /* Excpect a bad state error if psa_pake_get_shared_key called before key-exchange is completed */
    status = val->crypto_function(VAL_CRYPTO_PAKE_GET_SHARED_KEY,
                                  client, &attributes, &derv_key);

    TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE, TEST_CHECKPOINT_NUM(3));

    val->print(PRINT_DEBUG, "Check for bad state is successful \n", 0);
   } else {

   /* Starting key exchange operation */

   // ShareP
    status = send_message(client, server, PSA_PAKE_STEP_KEY_SHARE, 4);

   // ShareV
   status = send_message(server, client, PSA_PAKE_STEP_KEY_SHARE, 6);

   // ConfirmV
   status = send_message(server, client, PSA_PAKE_STEP_CONFIRM, 8);

   //ConfirmP
   status = send_message(client, server, PSA_PAKE_STEP_CONFIRM, 10);
  }
  return VAL_STATUS_SUCCESS;
}

int32_t psa_pake_get_shared_key_test(caller_security_t caller __UNUSED)
{
    int                     num_checks = sizeof(check1)/sizeof(check1[0]);
    int32_t                 i, status;
    psa_key_attributes_t    attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_pake_cipher_suite_t cipher_suite = PSA_PAKE_CIPHER_SUITE_INIT;
    psa_pake_operation_t    user = PSA_PAKE_OPERATION_INIT;
    psa_pake_operation_t    peer = PSA_PAKE_OPERATION_INIT;
    psa_pake_operation_t    client = PSA_PAKE_OPERATION_INIT;
    psa_pake_operation_t    server = PSA_PAKE_OPERATION_INIT;
    psa_key_id_t            pw_key, ckey, skey;
    psa_key_id_t            shared_key;

    if (num_checks == 0)
    {
        val->print(PRINT_TEST, "No test available for the selected crypto configuration\n", 0);
        return RESULT_SKIP(VAL_STATUS_NO_TESTS);
    }

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    for (i = 0; i < num_checks; i++)
     {
       val->print(PRINT_TEST, "[Check %d] ", g_test_count++);
       val->print(PRINT_TEST, check1[i].test_desc, 0);

       val->crypto_function(VAL_CRYPTO_PAKE_OPERATION_INIT,
                            &user);
       val->crypto_function(VAL_CRYPTO_PAKE_OPERATION_INIT,
                            &peer);
       val->crypto_function(VAL_CRYPTO_PAKE_OPERATION_INIT,
                            &client);
       val->crypto_function(VAL_CRYPTO_PAKE_OPERATION_INIT,
                            &server);

       /* Setting up the watchdog timer for each check */
        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

       /* Setup the attributes for the key */
       if (PSA_ALG_IS_JPAKE(check1[i].alg))
        {
          val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,
                               &attributes,
                               check1[i].type);
          val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS,
                               &attributes,
                               check1[i].usage_flag);
          val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM,
                               &attributes,
                               check1[i].alg);
       } else {
         val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,
                              &attributes,
                              check1[i].ctype);
         val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS,
                              &attributes,
                              check1[i].usage_flag);
         val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM,
                              &attributes,
                              check1[i].alg);
       }

        /* Setup cipher_suite object parameters */
         val->crypto_function(VAL_CRYPTO_PAKE_CS_SET_ALGORITHM,
                              &cipher_suite,
                              check1[i].alg);
         val->crypto_function(VAL_CRYPTO_PAKE_CS_SET_PRIMITIVE,
                              &cipher_suite,
                              check1[i].pake_primitive);
         val->crypto_function(VAL_CRYPTO_PAKE_CS_SET_KEY_CONFIRMATION,
                              &cipher_suite,
                              check1[i].key_confirmation);

        /* Import the key data into the key slot */
      if (PSA_ALG_IS_JPAKE(check1[i].alg))
       {
         status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY,
                                       &attributes,
                                       check1[i].data1,
                                       check1[i].data1_length,
                                       &pw_key);
         TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

         status = do_jpake_setup(&user, check1[i].user_id, check1[i].peer_id, pw_key,
                                     check1[i].user_role, &cipher_suite);

         status = do_jpake_setup(&peer, check1[i].peer_id, check1[i].user_id, pw_key,
                                     check1[i].peer_role, &cipher_suite);

         }
         else {
            status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY,
                                          &attributes,
                                          check1[i].data1,
                                          check1[i].data1_length,
                                          &ckey);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

            val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,
                                 &attributes,
                                 check1[i].stype);

            status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY,
                                          &attributes,
                                          check1[i].data2,
                                          check1[i].data2_length,
                                          &skey);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

            status = do_spake2p_setup(&client, check1[i].user_id, check1[i].peer_id,
                                      check1[i].context, ckey, check1[i].user_role, &cipher_suite);

            status = do_spake2p_setup(&server, check1[i].peer_id, check1[i].user_id,
                                      check1[i].context, skey, check1[i].peer_role, &cipher_suite);

      }

       /* Setup the attributes for the key */
        val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,
                             &attributes,
                             check1[i].derv_type);
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS,
                             &attributes,
                             check1[i].derv_flag);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM,
                             &attributes,
                             check1[i].derv_alg);

        if (PSA_ALG_IS_JPAKE(check1[i].alg))
        {
          /* check PSA_ERROR_BAD_STATE for psa_pake_get_shared_key */
          if (i == 1) {

               status = do_jpake_rounds(&user, &peer, 1);
           } else {

             status = do_jpake_rounds(&user, &peer, 0);

           /* Get shared secret as a key after key exchange rounds with psa_pake_get_shared_key */
             status = val->crypto_function(VAL_CRYPTO_PAKE_GET_SHARED_KEY,
                                           &user, &attributes, &shared_key);
             TEST_ASSERT_DUAL(status, check1[i].expected_status[0],
                                      check1[i].expected_status[1], TEST_CHECKPOINT_NUM(6));

             if (status == PSA_SUCCESS) {
             status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, shared_key);
             TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));
           }

           }

            status = val->crypto_function(VAL_CRYPTO_PAKE_ABORT, &user);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));

            status = val->crypto_function(VAL_CRYPTO_PAKE_ABORT, &peer);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(9));

     /* Expect a bad state if psa_pake_get_shared_key is called on inactive PAKE operation object */
          status = val->crypto_function(VAL_CRYPTO_PAKE_GET_SHARED_KEY,
                                        &user, &attributes, &shared_key);
          TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE, TEST_CHECKPOINT_NUM(10));

            /* Destroy the key */
            status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, pw_key);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(11));

       } else if (PSA_ALG_IS_SPAKE2P(check1[i].alg))
          {
            /* check PSA_ERROR_BAD_STATE for psa_pake_get_shared_key */
              if (i == 5) {
               status = do_spake2p_rounds(&client, &server, 1);

          } else{

             status = do_spake2p_rounds(&client, &server, 0);

             status = val->crypto_function(VAL_CRYPTO_PAKE_GET_SHARED_KEY,
                                           &client, &attributes, &shared_key);
             TEST_ASSERT_DUAL(status, check1[i].expected_status[0],
                                      check1[i].expected_status[1], TEST_CHECKPOINT_NUM(12));

             status = val->crypto_function(VAL_CRYPTO_PAKE_GET_SHARED_KEY,
                                           &server, &attributes, &shared_key);
             TEST_ASSERT_DUAL(status, check1[i].expected_status[0],
                                      check1[i].expected_status[1], TEST_CHECKPOINT_NUM(13));

             if (status == PSA_SUCCESS) {
             status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, shared_key);
             TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(14));

            }
           }

              status = val->crypto_function(VAL_CRYPTO_PAKE_ABORT, &client);
              TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(15));

              status = val->crypto_function(VAL_CRYPTO_PAKE_ABORT, &server);
              TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(16));

        /* Expect a bad state if psa_pake_get_shared_key called on inactive PAKE operation object */
              status = val->crypto_function(VAL_CRYPTO_PAKE_GET_SHARED_KEY,
                                            &client, &attributes, &shared_key);
              TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE, TEST_CHECKPOINT_NUM(17));

            /* Destroy the key */
              status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, ckey);
              TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(18));

              status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, skey);
              TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(19));

            }

   }

  return VAL_STATUS_SUCCESS;

}
