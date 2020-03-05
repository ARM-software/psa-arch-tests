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
#include "test_c044.h"
#include "test_data.h"
#include "val_crypto.h"

client_test_t test_c044_crypto_list[] = {
    NULL,
    psa_copy_key_test,
    NULL,
};

static int g_test_count = 1;
static uint8_t data[BUFFER_SIZE];

int32_t psa_copy_key_test(caller_security_t caller)
{
    uint32_t              length, i;
    const uint8_t        *key_data;
    psa_key_handle_t      target_handle = 0;
    psa_key_usage_t       get_key_usage_flags;
    psa_algorithm_t       get_key_algorithm;
    size_t                get_key_bits;
    psa_key_type_t        get_key_type;
    int                   num_checks = sizeof(check1)/sizeof(check1[0]);
    int32_t               status, export_status;
    psa_key_attributes_t  source_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t  target_attributes = PSA_KEY_ATTRIBUTES_INIT;

    if (num_checks == 0)
    {
        val->print(PRINT_TEST, "No test available for the selected crypto configuration\n", 0);
        return RESULT_SKIP(VAL_STATUS_NO_TESTS);
    }

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    /* Set the key data buffer to the input base on algorithm */
    for (i = 0; i < num_checks; i++)
    {
        val->print(PRINT_TEST, "[Check %d] ", g_test_count++);
        val->print(PRINT_TEST, check1[i].test_desc, 0);

        /* Setting up the watchdog timer for each check */
        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));


        if (PSA_KEY_TYPE_IS_RSA(check1[i].key_type))
        {
            if (check1[i].key_type == PSA_KEY_TYPE_RSA_KEY_PAIR)
            {
                if (check1[i].expected_bit_length == BYTES_TO_BITS(384))
                    key_data = rsa_384_keypair;
                else if (check1[i].expected_bit_length == BYTES_TO_BITS(256))
                    key_data = rsa_256_keypair;
                else
                    return VAL_STATUS_INVALID;
            }
            else
            {
                if (check1[i].expected_bit_length == BYTES_TO_BITS(384))
                    key_data = rsa_384_keydata;
                else if (check1[i].expected_bit_length == BYTES_TO_BITS(256))
                    key_data = rsa_256_keydata;
                else
                    return VAL_STATUS_INVALID;
            }
        }
        else if (PSA_KEY_TYPE_IS_ECC(check1[i].key_type))
        {
            if (PSA_KEY_TYPE_IS_ECC_KEY_PAIR(check1[i].key_type))
                key_data = ec_keypair;
            else
                key_data = ec_keydata;
        }
        else
            key_data = check1[i].key_data;

        /* Setup the attributes for the key */
        val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE, &source_attributes, check1[i].key_type);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM, &source_attributes, check1[i].key_alg);
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &source_attributes, check1[i].usage);

        /* Import the key data into the key slot */
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, &source_attributes, key_data,
                 check1[i].key_length, &check1[i].key_handle);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        /* Setup the attributes for the target key */
        val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE, &target_attributes,
        check1[i].key_type);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM, &target_attributes,
        check1[i].target_key_alg);
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &target_attributes,
        check1[i].target_usage);

        /* Make a copy of a key */
        status = val->crypto_function(VAL_CRYPTO_COPY_KEY, check1[i].key_handle,
                 &target_attributes, &target_handle);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(4));

        /* Destroy the source key */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, check1[i].key_handle);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

        if (check1[i].expected_status != PSA_SUCCESS)
        {
            val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &source_attributes);
            val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &target_attributes);

            continue;
        }

        /* Check if the target key attributes is as expected */
        status = val->crypto_function(VAL_CRYPTO_GET_KEY_ATTRIBUTES, target_handle,
                 &target_attributes);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));

        val->crypto_function(VAL_CRYPTO_GET_KEY_TYPE, &target_attributes, &get_key_type);
        TEST_ASSERT_EQUAL(get_key_type, check1[i].key_type, TEST_CHECKPOINT_NUM(7));

        val->crypto_function(VAL_CRYPTO_GET_KEY_USAGE_FLAGS, &target_attributes,
        &get_key_usage_flags);
        TEST_ASSERT_EQUAL(get_key_usage_flags, check1[i].expected_usage, TEST_CHECKPOINT_NUM(8));

        val->crypto_function(VAL_CRYPTO_GET_KEY_ALGORITHM, &target_attributes, &get_key_algorithm);
        TEST_ASSERT_EQUAL(get_key_algorithm, check1[i].expected_key_alg, TEST_CHECKPOINT_NUM(9));

        val->crypto_function(VAL_CRYPTO_GET_KEY_BITS, &target_attributes, &get_key_bits);
        TEST_ASSERT_EQUAL(get_key_bits, check1[i].expected_bit_length, TEST_CHECKPOINT_NUM(10));

        /* Export a key in binary format */
        export_status = val->crypto_function(VAL_CRYPTO_EXPORT_KEY, target_handle, data,
                        BUFFER_SIZE, &length);

        if (!(check1[i].expected_usage & PSA_KEY_USAGE_EXPORT))
        {
            export_status = PSA_ERROR_NOT_PERMITTED;
        }
        else
        {
            /* Check if the target key data is same as source data */
            export_status = PSA_SUCCESS;
            TEST_ASSERT_EQUAL(status, export_status, TEST_CHECKPOINT_NUM(11));

            TEST_ASSERT_EQUAL(length, check1[i].expected_key_length, TEST_CHECKPOINT_NUM(12));

            if (PSA_KEY_TYPE_IS_UNSTRUCTURED(check1[i].key_type))
            {
                TEST_ASSERT_MEMCMP(data, check1[i].key_data, length, TEST_CHECKPOINT_NUM(13));
            }
            else if (PSA_KEY_TYPE_IS_RSA(check1[i].key_type)
                  || PSA_KEY_TYPE_IS_ECC(check1[i].key_type))
            {
                TEST_ASSERT_MEMCMP(data, key_data, length, TEST_CHECKPOINT_NUM(14));
            }
            else
            {
                return VAL_STATUS_INVALID;
            }
        }

        /* Destroy the target key */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, target_handle);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(15));

        /* Copy on a destroyed source should be an error */
        status = val->crypto_function(VAL_CRYPTO_COPY_KEY, check1[i].key_handle,
                 &target_attributes, &target_handle);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_INVALID_HANDLE, TEST_CHECKPOINT_NUM(16));

        val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &source_attributes);
        val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &target_attributes);
    }

    return VAL_STATUS_SUCCESS;
}
