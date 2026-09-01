/** @file
 * Copyright (c) 2026, Arm Limited or its affiliates. All rights reserved.
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
#include "test_c081.h"
#include "test_data.h"

const client_test_t test_c081_crypto_list[] = {
    NULL,
    psa_hash_parallel_capacity_test,
    NULL,
};

static uint32_t g_test_count = 1;

int32_t psa_hash_parallel_capacity_test(caller_security_t caller __UNUSED)
{
#if defined(ARCH_TEST_SHA256) && defined(ARCH_TEST_MAX_PARALLEL_HASH_OPS) && \
    defined(ARCH_TEST_PARALLEL_HASH_OPS_FAILURE_STATUS)
    int32_t              status;
    int32_t              i;
    size_t               op_index;
    int32_t              num_checks = sizeof(check1) / sizeof(check1[0]);
    psa_hash_operation_t operations[ARCH_TEST_MAX_PARALLEL_HASH_OPS];
    psa_hash_operation_t extra_operation = PSA_HASH_OPERATION_INIT;

    if (num_checks == 0)
    {
        val->print(TEST, "No test available for the selected crypto configuration\n", 0);
        return RESULT_SKIP(VAL_STATUS_NO_TESTS);
    }

    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    for (i = 0; i < num_checks; i++)
    {
        val->print(TEST, "Check %d: ", g_test_count++);
        val->print(TEST, check1[i].test_desc, check1[i].max_parallel_ops);

        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

        for (op_index = 0; op_index < check1[i].max_parallel_ops; op_index++)
        {
            val->crypto_function(VAL_CRYPTO_HASH_OPERATION_INIT,
                                 &operations[op_index]);
            status = val->crypto_function(VAL_CRYPTO_HASH_SETUP,
                                          &operations[op_index],
                                          check1[i].alg);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));
        }

        val->crypto_function(VAL_CRYPTO_HASH_OPERATION_INIT,
                             &extra_operation);
        status = val->crypto_function(VAL_CRYPTO_HASH_SETUP,
                                      &extra_operation,
                                      check1[i].alg);
        TEST_ASSERT_EQUAL(status, check1[i].expected_failure_status,
                          TEST_CHECKPOINT_NUM(4));

        status = val->crypto_function(VAL_CRYPTO_HASH_ABORT,
                                      &extra_operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

        for (op_index = 0; op_index < check1[i].max_parallel_ops; op_index++)
        {
            status = val->crypto_function(VAL_CRYPTO_HASH_ABORT,
                                          &operations[op_index]);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));
        }

        status = val->crypto_function(VAL_CRYPTO_HASH_SETUP,
                                      &extra_operation,
                                      check1[i].alg);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));

        status = val->crypto_function(VAL_CRYPTO_HASH_ABORT,
                                      &extra_operation);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));
    }

    return VAL_STATUS_SUCCESS;
#else
    val->print(TEST, "No test available for the selected crypto configuration\n", 0);
    return RESULT_SKIP(VAL_STATUS_NO_TESTS);
#endif
}
