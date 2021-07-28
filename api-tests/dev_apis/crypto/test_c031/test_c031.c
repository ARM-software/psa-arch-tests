/** @file
 * Copyright (c) 2019-2021, Arm Limited or its affiliates. All rights reserved.
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
#include "test_c031.h"
#include "test_data.h"

const client_test_t test_c031_crypto_list[] = {
    NULL,
    psa_mac_abort_test,
    psa_mac_abort_before_finish_test,
    NULL,
};

extern  uint32_t g_test_count;

static int32_t  valid_test_input_index = -1;

int32_t psa_mac_abort_test(caller_security_t caller __UNUSED)
{
    int32_t               num_checks = sizeof(check1)/sizeof(check1[0]);
    int32_t               i, status;
    psa_mac_operation_t   operation = PSA_MAC_OPERATION_INIT;
    psa_key_attributes_t  attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t          key = 0;

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

        val->crypto_function(VAL_CRYPTO_KEY_ATTRIBUTES_INIT,
                             &attributes);
        val->crypto_function(VAL_CRYPTO_MAC_OPERATION_INIT,
                             &operation);

        /* Setting up the watchdog timer for each check */
        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

        /* Setup the attributes for the key */
        val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,
                             &attributes,
                             check1[i].type);
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS,
                             &attributes,
                             check1[i].usage_flags);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM,
                             &attributes,
                             check1[i].alg);

        /* Import the key data into the key slot */
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY,
                                      &attributes,
                                      check1[i].data,
                                      check1[i].data_length,
                                      &key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        /* Start a multipart MAC calculation operation */
        status = val->crypto_function(VAL_CRYPTO_MAC_SIGN_SETUP,
                                      &operation,
                                      key,
                                      check1[i].alg);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

        /* Abort a MAC operation */
        status = val->crypto_function(VAL_CRYPTO_MAC_ABORT,
                                      &operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

        /* Multiple Abort a MAC operation should succeed */
        status = val->crypto_function(VAL_CRYPTO_MAC_ABORT,
                                      &operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));

        /* Destroy the key */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY,
                                      key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));

        if (valid_test_input_index < 0)
            valid_test_input_index = i;
    }

    return VAL_STATUS_SUCCESS;
}

int32_t psa_mac_abort_before_finish_test(caller_security_t caller __UNUSED)
{
    size_t                mac_length;
    psa_key_id_t          key;
    psa_mac_operation_t   operation = PSA_MAC_OPERATION_INIT;
    int32_t               status;
    psa_key_attributes_t  attributes = PSA_KEY_ATTRIBUTES_INIT;

    if (valid_test_input_index < 0)
        return RESULT_SKIP(VAL_STATUS_NO_TESTS);

    val->crypto_function(VAL_CRYPTO_MAC_OPERATION_INIT,
                         &operation);

    val->print(PRINT_TEST, "[Check %d] ", g_test_count++);
    val->print(PRINT_TEST, "Test psa_mac_sign_finish after calling psa_mac_abort\n", 0);

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    /* Setting up the watchdog timer for each check */
    status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
    TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

    /* Setup the attributes for the key */
    val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,
                         &attributes,
                         check1[valid_test_input_index].type);
    val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS,
                         &attributes,
                         check1[valid_test_input_index].usage_flags);
    val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM,
                         &attributes,
                         check1[valid_test_input_index].alg);

    /* Import the key data into the key slot */
    status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY,
                                  &attributes,
                                  check1[valid_test_input_index].data,
                                  check1[valid_test_input_index].data_length,
                                  &key);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

    /* Start a multipart MAC calculation operation */
    status = val->crypto_function(VAL_CRYPTO_MAC_SIGN_SETUP,
                                  &operation,
                                  key,
                                  check1[valid_test_input_index].alg);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

    /* Add a message fragment to a multipart MAC operation */
    status = val->crypto_function(VAL_CRYPTO_MAC_UPDATE,
                                  &operation,
                                  input_bytes_data,
                                  16);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

    /* Abort a MAC operation */
    status = val->crypto_function(VAL_CRYPTO_MAC_ABORT,
                                  &operation);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));

    /* Finish the calculation of the MAC of a message */
    status = val->crypto_function(VAL_CRYPTO_MAC_SIGN_FINISH,
                                  &operation,
                                  expected_output,
                                  BUFFER_SIZE,
                                  &mac_length);
    TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE, TEST_CHECKPOINT_NUM(7));

    /* Destroy the key */
    status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY,
                                  key);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));

    return VAL_STATUS_SUCCESS;
}
