/** @file
 * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
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
#include "test_c045.h"
#include "test_data.h"
#include "val_crypto.h"

client_test_t test_c045_crypto_list[] = {
    NULL,
    psa_hash_clone_test,
    NULL,
};

static int g_test_count = 1;

int32_t psa_hash_clone_test(caller_security_t caller)
{
    int                     num_checks = sizeof(check1)/sizeof(check1[0]);
    int32_t                 i, status;
    const char              *expected_hash;
    char                    hash[HASH_64B];
    size_t                  hash_length, hash_size = sizeof(hash);
    psa_hash_operation_t    source_operation = PSA_HASH_OPERATION_INIT;
    psa_hash_operation_t    target_operation = PSA_HASH_OPERATION_INIT;

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

        if (check1[i].alg == PSA_ALG_SHA_384)
            expected_hash = sha384_hash;
        else if (check1[i].alg == PSA_ALG_SHA_512)
            expected_hash = sha512_hash;
        else
            expected_hash = check1[i].hash;

        /* Start a multipart hash operation */
        status = val->crypto_function(VAL_CRYPTO_HASH_SETUP, &source_operation, check1[i].alg);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        /* Clone a hash operation */
        status = val->crypto_function(VAL_CRYPTO_HASH_CLONE, &source_operation, &target_operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(4));

        /* Add a message fragment to a multipart hash source_operation */
        status = val->crypto_function(VAL_CRYPTO_HASH_UPDATE, &target_operation,
                                      &check1[i].input, check1[i].input_length);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

        /* Cloning to an active hash operation should be an error*/
        status = val->crypto_function(VAL_CRYPTO_HASH_CLONE, &source_operation, &target_operation);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE, TEST_CHECKPOINT_NUM(6));

        /* Finish the calculation of the hash of a message */
        status = val->crypto_function(VAL_CRYPTO_HASH_FINISH, &target_operation, hash, hash_size,
                                      &hash_length);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(7));

        if (check1[i].expected_status != PSA_SUCCESS)
        {
            continue;
        }

        TEST_ASSERT_EQUAL(hash_length, PSA_HASH_SIZE(check1[i].alg), TEST_CHECKPOINT_NUM(8));
        TEST_ASSERT_MEMCMP(hash, expected_hash, hash_length, TEST_CHECKPOINT_NUM(9));

        /*Abort the hash operation */
        status = val->crypto_function(VAL_CRYPTO_HASH_ABORT, &source_operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(10));

        status = val->crypto_function(VAL_CRYPTO_HASH_ABORT, &target_operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(11));

        /* Cloning on an aborted operator should be an error */
        status = val->crypto_function(VAL_CRYPTO_HASH_CLONE, &source_operation, &target_operation);
        TEST_ASSERT_EQUAL(status, PSA_ERROR_BAD_STATE, TEST_CHECKPOINT_NUM(12));

    }

    return VAL_STATUS_SUCCESS;
}
