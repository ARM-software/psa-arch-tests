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
#include "test_c072.h"
#include "test_data.h"

const client_test_t test_c072_crypto_list[] = {
    NULL,
    psa_pake_set_peer_test,
    NULL,
};

extern  uint32_t g_test_count;

int32_t psa_pake_set_peer_test(caller_security_t caller __UNUSED)
{
    int                     num_checks = sizeof(check1)/sizeof(check1[0]);
    int32_t                 i, status;
    psa_key_attributes_t    attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_pake_cipher_suite_t cipher_suite = PSA_PAKE_CIPHER_SUITE_INIT;
    psa_pake_operation_t    operation = PSA_PAKE_OPERATION_INIT;
    psa_key_id_t            key;

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
                             check1[i].usage_flag);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM,
                             &attributes,
                             check1[i].alg);

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
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY,
                                      &attributes,
                                      check1[i].data,
                                      check1[i].data_length,
                                      &key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        /* Setup PAKE operation object */
        status = val->crypto_function(VAL_CRYPTO_PAKE_SETUP,
                                      &operation,
                                      key,
                                      &cipher_suite);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

        /* Set role for Key exchange operation */
        status = val->crypto_function(VAL_CRYPTO_PAKE_SET_ROLE,
                                      &operation,
                                      check1[i].role);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

        /* Set user-id for the key exchange operation */
         status = val->crypto_function(VAL_CRYPTO_PAKE_SET_USER,
                                       &operation,
                                       check1[i].user_id,
                                       check1[i].user_id_len);
         TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));

        /* Set peer-id for the key exchange operation */
        status = val->crypto_function(VAL_CRYPTO_PAKE_SET_PEER,
                                       &operation,
                                       check1[i].peer_id,
                                       check1[i].peer_id_len);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(7));

        /* Destroy the key */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY,
                                      key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));

        /* Reset the key attributes */
        val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES,
                             &attributes);

        /* Abort the PAKE Operation */
        status = val->crypto_function(VAL_CRYPTO_PAKE_ABORT,
                                       &operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(9));

      /* Expect a bad state when psa_pake_set_peer is called on aborted inactive operation object */
        status = val->crypto_function(VAL_CRYPTO_PAKE_SET_PEER,
                                       &operation,
                                       check1[i].peer_id,
                                       check1[i].peer_id_len);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE, TEST_CHECKPOINT_NUM(10));
}
    return VAL_STATUS_SUCCESS;
}
