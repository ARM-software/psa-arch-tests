/** @file
 * Copyright (c) 2019-2025, Arm Limited or its affiliates. All rights reserved.
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
#include "test_c056.h"
#include "test_data.h"

const client_test_t test_c056_crypto_list[] = {
    NULL,
    psa_aead_generate_nonce_test,
    NULL,
};

extern  uint32_t g_test_count;

int32_t psa_aead_generate_nonce_test(caller_security_t caller __UNUSED)
{
#if (((defined(ARCH_TEST_CCM) || defined(ARCH_TEST_GCM)) && defined(ARCH_TEST_AES_128)) && \
(!defined(ARCH_TEST_AEAD_MULTISTAGE_OFF)))
    int32_t               i, j, status, nonce_sum;
    size_t                nonce_length;
    int32_t               num_checks = sizeof(check1)/sizeof(check1[0]);
    psa_key_attributes_t  attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_aead_operation_t  operation;
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
        val->print(TEST, "Check %d: ", i+1);
        val->print(TEST, check1[i].test_desc, 0);

        /* Reset the nonce output buffer */
        memset((void *)check1[i].nonce, 0x0, check1[i].nonce_size);

        val->crypto_function(VAL_CRYPTO_AEAD_OPERATION_INIT,
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
                             check1[i].usage_flags);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM,
                             &attributes,
                             check1[i].alg);

        /* Import the key data into the key slot */
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY,
                                      &attributes,
                                      check1[i].data,
                                      check1[i].data_length,
                                      &key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        if (check1[i].operation_state) {
            if (check1[i].usage_flags == PSA_KEY_USAGE_ENCRYPT) {
                /* Set the key for a multipart authenticated encryption operation */
                status = val->crypto_function(VAL_CRYPTO_AEAD_ENCRYPT_SETUP,
                                              &operation,
                                              key,
                                              check1[i].setup_alg);
            } else {
                /* Set the key for a multipart authenticated decryption operation */
                status = val->crypto_function(VAL_CRYPTO_AEAD_DECRYPT_SETUP,
                                              &operation,
                                              key,
                                              check1[i].setup_alg);
            }
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

            /* Declare the lengths of the message and additional data for AEAD */
            status = val->crypto_function(VAL_CRYPTO_AEAD_SET_LENGTHS,
                                          &operation,
                                          0,
                                          0);

            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));
        }


        /* Generate a random nonce for an authenticated encryption operation */
        status = val->crypto_function(VAL_CRYPTO_AEAD_GENERATE_NONCE,
                                      &operation,
                                      check1[i].nonce,
                                      check1[i].nonce_size,
                                      &nonce_length);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(6));

        if (check1[i].expected_status != PSA_SUCCESS)
        {
            /* Abort the AEAD operation */
            status = val->crypto_function(VAL_CRYPTO_AEAD_ABORT,
                                          &operation);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));

            /* Destroy the key */
            status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY,
                                          key);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));

            /* Reset the key attributes */
            val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES,
                                 &attributes);
            continue;
        }

        /* Check that if generated iv are non-zero */
        nonce_sum = 0;
        for (j = 0; j < (int32_t)nonce_length; j++)
            nonce_sum += check1[i].nonce[j];

        TEST_ASSERT_NOT_EQUAL(nonce_sum, 0, TEST_CHECKPOINT_NUM(9));

        /* Generate a random nonce on an ongoing operation should be an error */
        status = val->crypto_function(VAL_CRYPTO_AEAD_GENERATE_NONCE,
                                      &operation,
                                      check1[i].nonce,
                                      check1[i].nonce_size,
                                      &nonce_length);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE,  TEST_CHECKPOINT_NUM(10));

        /* Abort the AEAD operation */
        status = val->crypto_function(VAL_CRYPTO_AEAD_ABORT,
                                      &operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(11));

        /* Destroy the key */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY,
                                      key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(12));

        /* Reset the key attributes */
        val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES,
                             &attributes);
    }

    return VAL_STATUS_SUCCESS;
#else
    val->print(TEST, "No test available for the selected crypto configuration\n", 0);
    return RESULT_SKIP(VAL_STATUS_NO_TESTS);
#endif
}
