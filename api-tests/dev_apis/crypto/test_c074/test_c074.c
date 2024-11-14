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
#include "test_c074.h"
#include "test_data.h"

const client_test_t test_c074_crypto_list[] = {
    NULL,
    psa_pake_output_test,
    NULL,
};

extern  uint32_t g_test_count;

int32_t get_output_buffer_size(psa_pake_step_t step, psa_algorithm_t alg,
                               psa_pake_primitive_t primitive)
{
 size_t output_size;

 switch (step)
            {
              case PSA_PAKE_STEP_KEY_SHARE:
                    output_size = PSA_PAKE_OUTPUT_SIZE(alg, primitive, step);
                    break;

              case PSA_PAKE_STEP_ZK_PUBLIC:
                    output_size = PSA_PAKE_OUTPUT_SIZE(alg, primitive, step);
                    break;

              case PSA_PAKE_STEP_ZK_PROOF:
                    output_size = PSA_PAKE_OUTPUT_SIZE(alg, primitive, step);
                    break;

              case PSA_PAKE_STEP_CONFIRM:
                    output_size = PSA_PAKE_OUTPUT_SIZE(alg, primitive, step);
                    break;

              default:
                   output_size = PSA_PAKE_OUTPUT_MAX_SIZE;
                   break;
            }

  return output_size;
}

int32_t jpake_setup(psa_pake_operation_t *op, const uint8_t *user, const uint8_t *peer,
                psa_key_id_t pw_key, psa_pake_role_t role, psa_pake_cipher_suite_t *cipher_suite)
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

int32_t spake2p_setup(psa_pake_operation_t *op, const uint8_t *user, const uint8_t *peer,
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

int32_t psa_pake_output_test(caller_security_t caller __UNUSED)
{
    int                     num_checks = sizeof(check1)/sizeof(check1[0]);
    int32_t                 i, status;
    uint8_t                 output[PSA_PAKE_OUTPUT_MAX_SIZE];
    size_t                  output_length, output_size;
    psa_key_attributes_t    attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_pake_cipher_suite_t cipher_suite = PSA_PAKE_CIPHER_SUITE_INIT;
    psa_pake_operation_t    user = PSA_PAKE_OPERATION_INIT;
    psa_pake_operation_t    client = PSA_PAKE_OPERATION_INIT;
    psa_pake_operation_t    server = PSA_PAKE_OPERATION_INIT;
    psa_key_id_t            pw_key, ckey, skey;
    psa_pake_step_t         step;

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
                           &client);
      val->crypto_function(VAL_CRYPTO_PAKE_OPERATION_INIT,
                           &server);

      /* Setting up the watchdog timer for each check */
      status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
      TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

      /* Setup the attributes for the key */
      if (PSA_ALG_IS_JPAKE (check1[i].alg))
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
      if (PSA_ALG_IS_JPAKE (check1[i].alg))
       {
         status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY,
                                      &attributes,
                                      check1[i].data1,
                                      check1[i].data1_length,
                                      &pw_key);
         TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

         status = jpake_setup(&user, check1[i].user_id, check1[i].peer_id, pw_key,
                                      check1[i].user_role, &cipher_suite);

         } else
          {
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

            status = spake2p_setup(&client, check1[i].user_id, check1[i].peer_id, check1[i].context,
                                   ckey, check1[i].user_role, &cipher_suite);

            status = spake2p_setup(&server, check1[i].peer_id, check1[i].user_id, check1[i].context,
                                   skey, check1[i].peer_role, &cipher_suite);
        }

    for (step = 1; step < 5; step++)
      {
         output_size = get_output_buffer_size(step, check1[i].alg,
                                                    check1[i].pake_primitive);
        if (PSA_ALG_IS_JPAKE (check1[i].alg))
         {
          /* Expect a buffer too smaall error if output buffer size is too small */
           if (i == 5)
               {
                 status = val->crypto_function(VAL_CRYPTO_PAKE_OUTPUT,
                                                  &user,
                                                  step,
                                                  output,
                                                  output_size - 10,
                                                  &output_length);
                  TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(6));

                  break;
                } else
                {
                /* Get output for particular step of key exchange */
                status = val->crypto_function(VAL_CRYPTO_PAKE_OUTPUT,
                                              &user,
                                              step,
                                              output,
                                              output_size,
                                              &output_length);

                /* Expect a invalid argument error for a step PSA_PAKE_STEP_CONFIRM for JPAKE */
                 if (step == 4) {
                       TEST_ASSERT_DUAL(status, PSA_ERROR_INVALID_ARGUMENT,
                                         PSA_ERROR_NOT_SUPPORTED,  TEST_CHECKPOINT_NUM(7));
                     }
                 else{
                      TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(8));
                   }
              }
         }

       else if (PSA_ALG_IS_SPAKE2P (check1[i].alg))
        {
          if (step == 1)
              {
                /* Expect a buffer too smaall error if output buffer size is too small */
                if (i == 4) {
                   status = val->crypto_function(VAL_CRYPTO_PAKE_OUTPUT,
                                                 &client,
                                                 step,
                                                 output,
                                                 output_size - 10,
                                                 &output_length);
                   TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(9));
                   break;
                }

                /* Get output for PSA_STEP_KEY_SHARE step of key exchange from prover*/
                status = val->crypto_function(VAL_CRYPTO_PAKE_OUTPUT,
                                            &client,
                                            step,
                                            output,
                                            output_size,
                                            &output_length);
                TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(10));

                /* Verifier to provide and validate input of key share ny prover */
                status = val->crypto_function(VAL_CRYPTO_PAKE_INPUT,
                                            &server,
                                            step,
                                            output,
                                            output_size);
                TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(11));

                /* Verifier generates output for PSA_PAKE_KEY_SHARE step */
                status = val->crypto_function(VAL_CRYPTO_PAKE_OUTPUT,
                                              &server,
                                              step,
                                              output,
                                              output_size,
                                              &output_length);
               TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(12));
           }
            else if (step == 4) {
              /* Verifier generates output for PSA_PAKE_STEP_CONFIRM step */
              status = val->crypto_function(VAL_CRYPTO_PAKE_OUTPUT,
                                            &server,
                                            step,
                                            output,
                                            output_size,
                                            &output_length);
             TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(13));
            } else
              {
                /* Expect a invalid argument error if input step is PSA_PAKE_STEAP_ZK_PUBLIC
                            or PSA_PAKE_STEP_ZK_PROOF for SPAKE2P key exchange  */

                status = val->crypto_function(VAL_CRYPTO_PAKE_OUTPUT,
                                              &client,
                                              step,
                                              output,
                                              output_size,
                                              &output_length);
                TEST_ASSERT_DUAL(status, PSA_ERROR_INVALID_ARGUMENT,
                                         PSA_ERROR_NOT_SUPPORTED,  TEST_CHECKPOINT_NUM(14));

                status = val->crypto_function(VAL_CRYPTO_PAKE_OUTPUT,
                                              &server,
                                              step,
                                              output,
                                              output_size,
                                              &output_length);
                TEST_ASSERT_DUAL(status, PSA_ERROR_INVALID_ARGUMENT,
                                         PSA_ERROR_NOT_SUPPORTED,  TEST_CHECKPOINT_NUM(15));

              }

          }

      }

      if (PSA_ALG_IS_JPAKE (check1[i].alg))
         {
           /* Abort the PAKE operation object */
         status = val->crypto_function(VAL_CRYPTO_PAKE_ABORT, &user);
         TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(16));

       /* Expect a bad state when psa_pake_output called on a aborted inactive operation object */
         status = val->crypto_function(VAL_CRYPTO_PAKE_OUTPUT,
                                              &user,
                                              step,
                                              output,
                                              output_size,
                                              &output_length);
         TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE, TEST_CHECKPOINT_NUM(17));

         /* Destroy the key */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, pw_key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(18));

        /* Reset the key attributes */
        val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES,
                             &attributes);

       } else
       {
         /* Abort the PAKE operation object */
         status = val->crypto_function(VAL_CRYPTO_PAKE_ABORT, &client);
         TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(19));

         status = val->crypto_function(VAL_CRYPTO_PAKE_ABORT, &server);
         TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(20));

        /* Expect a bad state when psa_pake_output called on a aborted inactive operation object */
         status = val->crypto_function(VAL_CRYPTO_PAKE_OUTPUT,
                                       &client,
                                       step,
                                       output,
                                       output_size,
                                       &output_length);
         TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE, TEST_CHECKPOINT_NUM(21));

          /* Destroy the key */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, ckey);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(22));

        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, skey);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(23));

         /* Reset the key attributes */
        val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES,
                             &attributes);
     }

  }
    return VAL_STATUS_SUCCESS;
}

