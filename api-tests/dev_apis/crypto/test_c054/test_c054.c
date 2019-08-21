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
#include "test_c054.h"
#include "test_data.h"

client_test_t test_c054_crypto_list[] = {
    NULL,
    psa_aead_generate_nonce_test,
    NULL,
};

static int g_test_count = 1;

int32_t psa_aead_generate_nonce_test(caller_security_t caller)
{
    int32_t               i, j, status, nonce_sum;
    uint8_t               nonce[SIZE_32B];
    size_t                nonce_length;
    int                   num_checks = sizeof(check1)/sizeof(check1[0]);
    psa_key_attributes_t  attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_aead_operation_t  operation = PSA_AEAD_OPERATION_INIT;

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
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &attributes, check1[i].usage);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM, &attributes, check1[i].key_alg);

        /* Import the key data into the key slot */
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, &attributes, check1[i].key_data,
                 check1[i].key_length, &check1[i].key_handle);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        /* Set the key for a multipart authenticated encryption operation */
        status = val->crypto_function(VAL_CRYPTO_AEAD_ENCRYPT_SETUP, &operation,
                 check1[i].key_handle, check1[i].alg);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

        /* Generate a random nonce for an authenticated encryption operation */
        status = val->crypto_function(VAL_CRYPTO_AEAD_GENERATE_NONCE, &operation, nonce,
                 check1[i].nonce_size, &nonce_length);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(5));

        if (check1[i].expected_status != PSA_SUCCESS)
        {
            /* Abort the AEAD operation */
            status = val->crypto_function(VAL_CRYPTO_AEAD_ABORT, &operation);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));

            /* Destroy the key */
            status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, check1[i].key_handle);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));

            /* Reset the key attributes */
            val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &attributes);
            continue;
        }

        /* Check that if generated iv are non-zero */
        nonce_sum = 0;
        for (j = 0; j < nonce_length; j++)
        {
            nonce_sum += nonce[j];
        }

        TEST_ASSERT_NOT_EQUAL(nonce_sum, 0, TEST_CHECKPOINT_NUM(8));

        /* Generate a random nonce on an ongoing operation should be an error */
        status = val->crypto_function(VAL_CRYPTO_AEAD_GENERATE_NONCE, &operation, nonce,
                 check1[i].nonce_size, &nonce_length);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE,  TEST_CHECKPOINT_NUM(9));

        /* Abort the AEAD operation */
        status = val->crypto_function(VAL_CRYPTO_AEAD_ABORT, &operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(10));

        /* Destroy the key */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, check1[i].key_handle);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(11));

        /* Reset the key attributes */
        val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &attributes);

        /* Generate a random nonce on an aborted operation should be an error */
        status = val->crypto_function(VAL_CRYPTO_AEAD_GENERATE_NONCE, &operation, nonce,
                 check1[i].nonce_size, &nonce_length);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE,  TEST_CHECKPOINT_NUM(12));

    }

    return VAL_STATUS_SUCCESS;
}
