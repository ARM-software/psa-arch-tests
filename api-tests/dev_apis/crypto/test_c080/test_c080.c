/** @file
 * Copyright (c) 2025, Arm Limited or its affiliates. All rights reserved.
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

#include "val_interfaces.h"
#include "val_target.h"
#include "test_c080.h"
#include "test_data.h"

const client_test_t test_c080_crypto_list[] = {
    NULL,
    psa_key_agreement_test,
    psa_key_agreement_negative_test,
    NULL,
};

extern uint32_t g_test_count;

static int32_t  valid_test_input_index = -1;

int32_t psa_key_agreement_test(caller_security_t caller __UNUSED)
{
    int                     num_checks = sizeof(check1)/sizeof(check1[0]);
    int32_t                 i, status;
    psa_key_attributes_t    attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t            key, derv_key;
    psa_key_derivation_operation_t kdf = PSA_KEY_DERIVATION_OPERATION_INIT;
    uint8_t                 secret[32];

    if (num_checks == 0)
    {
        val->print(PRINT_TEST, "No test available for the selected crypto configuration\n", 0);
        return RESULT_SKIP(VAL_STATUS_NO_TESTS);
    }

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

        /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    for (i = 0; i < num_checks; i++)
    {
        val->print(PRINT_TEST, "Check %d: ", g_test_count++);
        val->print(PRINT_TEST, check1[i].test_desc, 0);

        /* Setting up the watchdog timer for each check */
        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

        /* Setup the attributes for the key */
        val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE, &attributes, check1[i].key_type);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM, &attributes, check1[i].key_alg);
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &attributes, check1[i].usage);

        /* Import the key data into the key slot */
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, &attributes, check1[i].key_data,
                 check1[i].key_len, &key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        /* Setup the attributes for the derived key */
        val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE, &attributes, check1[i].derv_type);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM, &attributes, check1[i].derv_alg);
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &attributes, check1[i].derv_flag);

         /* Set up a key agreement operation */
       status = val->crypto_function(VAL_CRYPTO_KEY_AGREEMENT, key, check1[i].peer_key,
                                      check1[i].peer_key_len, check1[i].key_alg,
                                      &attributes, &derv_key);
        TEST_ASSERT_DUAL(status, check1[i].expected_status[0],
                                 check1[i].expected_status[1], TEST_CHECKPOINT_NUM(4));

        if ((check1[i].expected_status[0] == PSA_SUCCESS) &&
            (check1[i].derv_type == PSA_KEY_TYPE_DERIVE))
        {
        /* Set up a key derivation operation */
        status =  val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SETUP,
                                        &kdf, check1[i].derv_alg);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_INPUT_KEY,
                                      &kdf, PSA_KEY_DERIVATION_INPUT_SECRET,
                                      derv_key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));

        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_INPUT_BYTES,
                                       &kdf, PSA_KEY_DERIVATION_INPUT_INFO,
                                       input_bytes_data, INPUT_BYTES_DATA_LEN);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));

        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_OUTPUT_BYTES,
                                      &kdf, secret, sizeof(secret));
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));

        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_ABORT, &kdf);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(9));
        }

        /* Destroy a key and restore the slot to its default state */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(10));

        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, derv_key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(11));

         if (valid_test_input_index < 0)
            valid_test_input_index = i;

      }

   return VAL_STATUS_SUCCESS;

 }

int32_t psa_key_agreement_negative_test(caller_security_t caller __UNUSED)
{
  int32_t               status;
  psa_key_id_t          key = 8, derv_key;
  psa_key_attributes_t  attributes = PSA_KEY_ATTRIBUTES_INIT;

if (valid_test_input_index < 0)
        return RESULT_SKIP(VAL_STATUS_NO_TESTS);

  /* Initialize the PSA crypto library*/
     status = val->crypto_function(VAL_CRYPTO_INIT);
     TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

  /* Setting up the watchdog timer for each check */
     status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
     TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

  /* Setup the attributes for the derived key */
     val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE, &attributes,
                          check1[valid_test_input_index].derv_type);
     val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM, &attributes,
                          check1[valid_test_input_index].derv_alg);
     val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &attributes,
                          check1[valid_test_input_index].derv_flag);

    val->print(PRINT_TEST, "Check %d: Test psa_key_agreement - Invalid key handle\n",
                                                                                 g_test_count++);
   /* Set up a key agreement operation */
      status = val->crypto_function(VAL_CRYPTO_KEY_AGREEMENT, key,
                                     check1[valid_test_input_index].peer_key,
                                     check1[valid_test_input_index].peer_key_len,
                                     check1[valid_test_input_index].key_alg,
                                     &attributes, &derv_key);
      TEST_ASSERT_EQUAL(status, PSA_ERROR_INVALID_HANDLE, TEST_CHECKPOINT_NUM(3));

      val->print(PRINT_TEST, "Check %d: Test psa_key_agreement - Zero as key handle\n",
                                                                                 g_test_count++);

      status = val->crypto_function(VAL_CRYPTO_KEY_AGREEMENT, 0,
                                     check1[valid_test_input_index].peer_key,
                                     check1[valid_test_input_index].peer_key_len,
                                     check1[valid_test_input_index].key_alg,
                                     &attributes, &derv_key);
      TEST_ASSERT_EQUAL(status, PSA_ERROR_INVALID_HANDLE, TEST_CHECKPOINT_NUM(4));

  return VAL_STATUS_SUCCESS;
}
