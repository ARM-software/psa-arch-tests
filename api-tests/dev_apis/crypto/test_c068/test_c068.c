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
#include "test_c068.h"
#include "test_data.h"

const client_test_t test_c068_crypto_list[] = {
    NULL,
    psa_key_derivation_verify_key_test,
    NULL,
};

int32_t psa_key_derivation_verify_key_test(caller_security_t caller __UNUSED)
{
    int32_t                         status;
    int                             i;
    int                             num_checks = sizeof(check1)/sizeof(check1[0]);
    psa_key_attributes_t            attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_derivation_operation_t  operation = PSA_KEY_DERIVATION_OPERATION_INIT;
    size_t                          capacity;
    psa_key_id_t                    expected;

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
        val->print(TEST, "Check %d: ", i+1);
        val->print(TEST, check1[i].test_desc, 0);

        /* Setting up the watchdog timer for each check */
        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

        /* Start the key derivation operation */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SETUP, &operation,
                 check1[i].key_alg);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        /* Set the capacity for the generator */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SET_CAPACITY, &operation,
                 check1[i].capacity);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

        /* Setup the attributes for the key */
        val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE, &attributes,
		                     check1[i].key_type);
        val->crypto_function(VAL_CRYPTO_SET_KEY_BITS, &attributes,
                             check1[i].key_bits);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM, &attributes,
                             check1[i].key_alg);
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &attributes,
                             check1[i].usage);

        /* Provide number of rounds(cost) as first input for PBKDF2 algorithms */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_INPUT_INTEGER, &operation,
                     PSA_KEY_DERIVATION_INPUT_COST, input_rounds);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

        /* Provide input as data depending on the step */
        if (check1[i].step == PSA_KEY_DERIVATION_INPUT_SALT ||
            check1[i].step == PSA_KEY_DERIVATION_INPUT_PASSWORD)
        {
            status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_INPUT_BYTES, &operation,
                     PSA_KEY_DERIVATION_INPUT_SALT, check1[i].salt_data, check1[i].salt_length);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));

            if (check1[i].step == PSA_KEY_DERIVATION_INPUT_PASSWORD) {
                status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_INPUT_BYTES, &operation,
                         check1[i].step, check1[i].data, check1[i].data_length);
                TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));
            }
        }

        /* Import the verification data into its key slot */
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, &attributes,
                        &check1[i].expected_hash, check1[i].expected_hash_len, &expected);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));

        /*  Verify the key from an ongoing key derivation operation */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_VERIFY_KEY, &operation,
                                      expected);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(9));

        /* Verifying set zero capacity for provided invalid operation's capacity */
        if (check1[i].capacity < check1[i].expected_hash_len) {
            val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_GET_CAPACITY, &operation,
                                 &capacity);
            TEST_ASSERT_EQUAL(capacity, 0, TEST_CHECKPOINT_NUM(10));
        }

        /* Destroy the created key */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, expected);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(11));

        /* Abort the derivation operation */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_ABORT, &operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(12));

        /* Expect error-code when verification is called on an aborted operation */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_VERIFY_KEY, &operation,
                                      expected);
        TEST_ASSERT_DUAL(status, PSA_ERROR_BAD_STATE, PSA_ERROR_INVALID_HANDLE,\
                            TEST_CHECKPOINT_NUM(13));
    }

    return VAL_STATUS_SUCCESS;
}
