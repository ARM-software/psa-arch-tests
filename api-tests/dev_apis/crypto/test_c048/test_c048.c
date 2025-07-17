/** @file
 * Copyright (c) 2024-2025, Arm Limited or its affiliates. All rights reserved.
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
#include "test_c048.h"
#include "test_data.h"

const client_test_t test_c048_crypto_list[] = {
    NULL,
    psa_mac_sign_multipart_test,
    NULL,
};

extern  uint32_t g_test_count;

int32_t psa_mac_sign_multipart_test(caller_security_t caller __UNUSED)
{
    uint8_t               data[BUFFER_SIZE];
    int                   num_checks = sizeof(check1)/sizeof(check1[0]);
    int32_t               i, status;
    size_t                length;
    psa_key_attributes_t  attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_mac_operation_t   operation = PSA_MAC_OPERATION_INIT;
    psa_key_id_t          key;

    if (num_checks == 0)
    {
        val->print(TEST, "No test available for the selected crypto configuration\n", 0);
        return RESULT_SKIP(VAL_STATUS_NO_TESTS);
    }

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    for (i = 0; i < num_checks; i++)
    {
        val->print(TEST, "Check %d: ", g_test_count++);
        val->print(TEST, check1[i].test_desc, 0);
        memset(data, 0, sizeof(data));

        /* Setting up the watchdog timer for each check */
        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

        /* Setup the attributes for the key */
        val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE, &attributes, check1[i].key_type);
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &attributes, check1[i].usage);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM, &attributes, check1[i].key_alg);

        /* Import the key data into the key slot */
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, &attributes, check1[i].key_data,
                 check1[i].key_length, &key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        /* Setting up multi-part MAC calculation operation */
        status = val->crypto_function(VAL_CRYPTO_MAC_SIGN_SETUP, &operation, key,
                 check1[i].mac_alg);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

        /* Add message fragment to the multi-part MAC operation */
        status = val->crypto_function(VAL_CRYPTO_MAC_UPDATE, &operation,
                 &check1[i].data, check1[i].data_size);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

        /* Finish the calculation of MAC of the message */
        status = val->crypto_function(VAL_CRYPTO_MAC_SIGN_FINISH, &operation,
                 &data, check1[i].mac_size, &length);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(6));

        /* Check if the MAC data matches with the expected data */
        TEST_ASSERT_MEMCMP(data, check1[i].expected_data, length, TEST_CHECKPOINT_NUM(7));

        memset(data, 0, sizeof(data));

        /* Abort the MAC operation */
        status = val->crypto_function(VAL_CRYPTO_MAC_ABORT, &operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));

        /* Destroy the key */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(9));

        /* Reset the key attributes and check if psa_import_key fails */
        val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &attributes);

    }
    return VAL_STATUS_SUCCESS;
}
