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
#include "test_c025.h"
#include "test_data.h"

#define SLOT_1      0
#define SLOT_2      1

client_test_t test_c025_crypto_list[] = {
    NULL,
    psa_aead_decrypt_test,
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

int32_t psa_aead_decrypt_test(security_t caller)
{
    int32_t          i, status;
    uint8_t          plaintext[BUFFER_SIZE];
    psa_key_policy_t policy;
    size_t           plaintext_length;
    int              num_checks = sizeof(check1)/sizeof(check1[0]);
    uint8_t          *nonce, *additional_data;

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    for (i = 0; i < num_checks; i++)
    {
        val->print(PRINT_TEST, "[Check %d] ", g_test_count++);
        val->print(PRINT_TEST, check1[i].test_desc, 0);

        /* Initialize a key policy structure to a default that forbids all
         * usage of the key
         */
        val->crypto_function(VAL_CRYPTO_KEY_POLICY_INIT, &policy);

        /* Setting up the watchdog timer for each check */
        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

        /* Set the standard fields of a policy structure */
        val->crypto_function(VAL_CRYPTO_KEY_POLICY_SET_USAGE, &policy, check1[i].usage,
                                                                          check1[i].key_alg);

        memset(plaintext, 0, sizeof(plaintext));

        /* Set the usage policy on a key slot */
        status = val->crypto_function(VAL_CRYPTO_SET_KEY_POLICY, check1[i].key_slot[SLOT_1],
                                      &policy);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        /* Import the key data into the key slot */
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, check1[i].key_slot[SLOT_1],
                    check1[i].key_type, check1[i].key_data, check1[i].key_length);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

        if (is_buffer_empty(check1[i].nonce, check1[i].nonce_length) == TRUE)
            nonce = NULL;
        else
            nonce = check1[i].nonce;

        if (is_buffer_empty(check1[i].additional_data, check1[i].additional_data_length) == TRUE)
            additional_data = NULL;
        else
            additional_data = check1[i].additional_data;

        /* Process an authenticated decryption operation */
        status = val->crypto_function(VAL_CRYPTO_AEAD_DECRYPT, check1[i].key_slot[SLOT_2],
                  check1[i].key_alg, nonce, check1[i].nonce_length, additional_data,
                  check1[i].additional_data_length, check1[i].ciphertext, check1[i].ciphertext_size,
                  plaintext, check1[i].plaintext_size, &plaintext_length);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(5));

        if (check1[i].expected_status != PSA_SUCCESS)
            continue;

        /* Check if the length matches */
        TEST_ASSERT_EQUAL(plaintext_length, check1[i].expected_plaintext_length, TEST_CHECKPOINT_NUM(6));

        /* Check if the data matches */
        TEST_ASSERT_MEMCMP(plaintext, check1[i].expected_plaintext, plaintext_length,
                           TEST_CHECKPOINT_NUM(7));
    }

    return VAL_STATUS_SUCCESS;
}
