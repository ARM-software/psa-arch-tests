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
#include "test_c024.h"
#include "test_data.h"

client_test_t test_c024_crypto_list[] = {
    NULL,
    psa_aead_encrypt_test,
    NULL,
};

static int g_test_count = 1;

static bool_t is_buffer_empty(uint8_t *buffer, size_t size)
{
    int i;

    for (i = 0; i < size; i++)
    {
        if (buffer[i] != 0)
            return FALSE;
    }

    return TRUE;
}

int32_t psa_aead_encrypt_test(caller_security_t caller)
{
    int32_t               i, status;
    uint8_t               ciphertext[BUFFER_SIZE];
    size_t                ciphertext_length;
    int                   num_checks = sizeof(check1)/sizeof(check1[0]);
    uint8_t              *nonce, *additional_data;
    psa_key_attributes_t  attributes = PSA_KEY_ATTRIBUTES_INIT;

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

        if (is_buffer_empty(check1[i].nonce, check1[i].nonce_length) == TRUE)
        {
            nonce = NULL;
            check1[i].nonce_length = 0;
        }
        else
            nonce = check1[i].nonce;

        if (is_buffer_empty(check1[i].additional_data, check1[i].additional_data_length) == TRUE)
        {
            additional_data = NULL;
            check1[i].additional_data_length = 0;
        }
        else
            additional_data = check1[i].additional_data;

        /* Process an authenticated encryption operation */
        status = val->crypto_function(VAL_CRYPTO_AEAD_ENCRYPT, check1[i].key_handle,
                    check1[i].key_alg, nonce, check1[i].nonce_length, additional_data,
                    check1[i].additional_data_length, check1[i].plaintext,
                    check1[i].plaintext_length, ciphertext, check1[i].ciphertext_size,
                    &ciphertext_length);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(4));

        if (check1[i].expected_status != PSA_SUCCESS)
        {
            /* Destroy the key */
            status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, check1[i].key_handle);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

            continue;
        }

        /* Check if the length matches */
        TEST_ASSERT_EQUAL(ciphertext_length,
                    check1[i].expected_ciphertext_length,
                    TEST_CHECKPOINT_NUM(6));

        /* Check if the data matches */
        TEST_ASSERT_MEMCMP(ciphertext, check1[i].expected_ciphertext, ciphertext_length,
                           TEST_CHECKPOINT_NUM(7));

        /* Destroy the key */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, check1[i].key_handle);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));

        /* Reset the key attributes and check if psa_import_key fails */
        val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &attributes);

        /* Process an authenticated encryption operation on a destroyed key*/
        status = val->crypto_function(VAL_CRYPTO_AEAD_ENCRYPT, check1[i].key_handle,
                    check1[i].key_alg, nonce, check1[i].nonce_length, additional_data,
                    check1[i].additional_data_length, check1[i].plaintext,
                    check1[i].plaintext_length, ciphertext, check1[i].ciphertext_size,
                    &ciphertext_length);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_INVALID_HANDLE, TEST_CHECKPOINT_NUM(9));
    }

    return VAL_STATUS_SUCCESS;
}
