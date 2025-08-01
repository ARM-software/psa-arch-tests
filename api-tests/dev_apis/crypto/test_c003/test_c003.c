/** @file
 * Copyright (c) 2018-2025, Arm Limited or its affiliates. All rights reserved.
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
#include "test_c003.h"
#include "test_data.h"

const client_test_t test_c003_crypto_list[] = {
    NULL,
    psa_export_key_test,
    NULL,
};

int32_t psa_export_key_test(caller_security_t caller __UNUSED)
{
    int32_t               i, status;
    size_t                expected_data_length;
    int                   num_checks = sizeof(check1)/sizeof(check1[0]);
    psa_key_attributes_t  attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t          key;

    if (num_checks == 0)
    {
        val->print(TEST, "No test available for the selected crypto configuration\n", 0);
        return RESULT_SKIP(VAL_STATUS_NO_TESTS);
    }

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    /* Set the key data buffer to the input base on algorithm */
    for (i = 0; i < num_checks; i++)
    {
        val->print(TEST, "Check %d: ", i+1);
        val->print(TEST, check1[i].test_desc, 0);

        memset(check1[i].expected_data, 0, BUFFER_SIZE);

        /* Setting up the watchdog timer for each check */
        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

        /* Setup the attributes for the key */
        val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE, &attributes, check1[i].type);
        val->crypto_function(VAL_CRYPTO_SET_KEY_BITS, &attributes, check1[i].bits);
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &attributes, check1[i].usage_flags);

        /* Import the key data into the key slot */
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY,
                                      &attributes, check1[i].data,
                                      check1[i].data_length,
                                      &key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        /* Export a key in binary format */
        status = val->crypto_function(VAL_CRYPTO_EXPORT_KEY,
                                      key,
                                      check1[i].expected_data,
                                      check1[i].data_size,
                                      &expected_data_length);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(4));

        /* If failure is expected, continue with the next data set */
        if (check1[i].expected_status != PSA_SUCCESS)
        {
            status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, key);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));
            continue;
        }

        /* Check the attributes of the exported key */
        TEST_ASSERT_EQUAL(expected_data_length, check1[i].expected_data_length,
                          TEST_CHECKPOINT_NUM(6));

        TEST_ASSERT_MEMCMP(check1[i].expected_data, check1[i].data, expected_data_length,
                                                     TEST_CHECKPOINT_NUM(7));

        /* Destroy the key handle and check if export key fails */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));

        status = val->crypto_function(VAL_CRYPTO_EXPORT_KEY,
                                      key,
                                      check1[i].expected_data,
                                      check1[i].data_size,
                                      &expected_data_length);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_INVALID_HANDLE, TEST_CHECKPOINT_NUM(9));
    }

    return VAL_STATUS_SUCCESS;
}
