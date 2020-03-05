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
#include "test_c021.h"
#include "test_data.h"
#include "val_crypto.h"

#define     SLOT_1      1
#define     SLOT_2      2

client_test_t test_c021_crypto_list[] = {
    NULL,
    psa_key_derivation_output_key_test,
    NULL,
};

static int      g_test_count = 1;

int32_t psa_key_derivation_output_key_test(caller_security_t caller)
{
    uint32_t                        i, status;
    int                             num_checks = sizeof(check1)/sizeof(check1[0]);
    psa_key_handle_t                key_handle[2];
    psa_key_attributes_t            attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t            derv_attributes = PSA_KEY_ATTRIBUTES_INIT;
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

        /* Import the key if the derivation input is key */
        if (check1[i].step == PSA_KEY_DERIVATION_INPUT_SECRET)
        {
            /* Import the key data into the key slot */
            status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, &attributes, check1[i].key_data,
                     check1[i].key_length, &check1[i].key_handle);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));
        }

        /* Start the key derivation operation */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SETUP, &operation,
                 check1[i].key_alg);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

        /* Set the capacity for the generator */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_SET_CAPACITY, &operation,
                 check1[i].capacity);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

        /* Provide input as key or data depending on the step */
        if (check1[i].step == PSA_KEY_DERIVATION_INPUT_SECRET)
        {
            /* Provide an input for key derivation or key agreement */
            status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_INPUT_KEY, &operation,
                     check1[i].step, check1[i].key_handle);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));
        }
        else
        {
            status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_INPUT_BYTES, &operation,
                     check1[i].step, check1[i].data, check1[i].data_length);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));
        }

        /* Setup the attributes for the key */
        val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE, &derv_attributes, check1[i].derive_key_type);
        val->crypto_function(VAL_CRYPTO_SET_KEY_BITS, &derv_attributes, check1[i].derive_key_bits);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM, &derv_attributes,
        check1[i].derive_key_alg);
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &derv_attributes,
        check1[i].derive_usage);

        /*  Derive a key from an ongoing key derivation operation */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_OUTPUT_KEY, &derv_attributes,
                 &operation, &key_handle[SLOT_1]);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(8));

        if (check1[i].step == PSA_KEY_DERIVATION_INPUT_SECRET)
        {
            status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, check1[i].key_handle);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(9));

        }

        if (check1[i].expected_status != PSA_SUCCESS)
        {
            status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_ABORT, &operation);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(10));

            continue;
        }

        /* Read some key from a key derivation operation with no data in the operation */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_OUTPUT_KEY, &derv_attributes,
                 &operation, &key_handle[SLOT_2]);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_INSUFFICIENT_DATA, TEST_CHECKPOINT_NUM(11));

        /* Abort the derivation operation */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_ABORT, &operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(12));

        /* Expect bad state when derivation is called on an aborted operation */
        status = val->crypto_function(VAL_CRYPTO_KEY_DERIVATION_OUTPUT_KEY, &derv_attributes,
                 &operation, &key_handle[SLOT_2]);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE, TEST_CHECKPOINT_NUM(13));
    }

    return VAL_STATUS_SUCCESS;
}
