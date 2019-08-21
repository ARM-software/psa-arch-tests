/** @file
 * Copyright (c) 2019, Arm Limited or its affiliates. All rights reserved.
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
#include "test_c019.h"
#include "test_data.h"

client_test_t test_c019_crypto_list[] = {
    NULL,
    psa_key_derivation_key_agreement_test,
    psa_key_derivation_key_agreement_negative_test,
    NULL,
};

static int         g_test_count = 1;

int32_t psa_key_derivation_key_agreement_test(caller_security_t caller)
{
    int                             num_checks = sizeof(check1)/sizeof(check1[0]);
    int32_t                         i, status;
    psa_key_attributes_t            attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_derivation_operation_t  operation = PSA_KEY_DERIVATION_OPERATION_INIT;

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

        /* Setting up the watchdog timer for each check */
        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

        /* Setup the attributes for the key */
        val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE, &attributes, check1[i].key_type);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM, &attributes, check1[i].key_alg);
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &attributes, check1[i].usage);

        /* Import the key data into the key slot */
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, &attributes, check1[i].key_data,
                 check1[i].key_length, &check1[i].key_handle);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        /* Set up a key agreement operation */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SETUP, &operation,
                 check1[i].key_alg);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

        /* Perform a key agreement */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_KEY_AGREEMENT, &operation,
                 check1[i].step, check1[i].key_handle, check1[i].peer_key,
                 check1[i].peer_key_length);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(5));

        /* Abort the key derivation operation */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_ABORT, &operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));

        /* Reset the key attributes and check if psa_import_key fails */
        val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &attributes);

        if (check1[i].expected_status != PSA_SUCCESS)
            continue;

        /* Destroy a key and restore the slot to its default state */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, check1[i].key_handle);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));
    }

    return VAL_STATUS_SUCCESS;
}

int32_t psa_key_derivation_key_agreement_negative_test(caller_security_t caller)
{
    int32_t                         i, status;
    int                             num_checks = sizeof(check2)/sizeof(check2[0]);
    psa_key_derivation_operation_t  operation = PSA_KEY_DERIVATION_OPERATION_INIT;

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    for (i = 0; i < num_checks; i++)
    {
        /* Setting up the watchdog timer for each check */
        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SETUP, &operation,
                 check2[i].key_alg);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        val->print(PRINT_TEST, "[Check %d] Test psa_key_derivation_key_agreement "
                               "- Invalid handle\n", g_test_count++);

        /* Set up a key agreement operation */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_KEY_AGREEMENT, &operation,
                 check2[i].step, check2[i].key_handle, check2[i].peer_key,
                 check2[i].peer_key_length);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_INVALID_HANDLE, TEST_CHECKPOINT_NUM(4));

        val->print(PRINT_TEST, "[Check %d] Test psa_key_derivation_key_agreement"
                               " - Zero as handle\n", g_test_count++);

        /* Set up a key agreement operation */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_KEY_AGREEMENT, &operation,
                 check2[i].step, 0, check2[i].peer_key, check2[i].peer_key_length);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_INVALID_HANDLE, TEST_CHECKPOINT_NUM(5));
    }

    return VAL_STATUS_SUCCESS;
}
