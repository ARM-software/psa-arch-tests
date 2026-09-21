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

#include "test_crypto_common.h"

typedef struct {
    const char     *test_desc;
    psa_algorithm_t alg;
    size_t          max_parallel_ops;
    psa_status_t    expected_failure_status;
} test_data;

static const test_data check1[] = {
#if defined(ARCH_TEST_SHA256) && defined(ARCH_TEST_MAX_PARALLEL_HASH_OPS) && \
    defined(ARCH_TEST_PARALLEL_HASH_OPS_FAILURE_STATUS)
{
    .test_desc               = "Test configured parallel hash operation limit with SHA256 - "
                               "expect %d sessions to open, next to fail\n",
    .alg                     = PSA_ALG_SHA_256,
    .max_parallel_ops        = ARCH_TEST_MAX_PARALLEL_HASH_OPS,
    .expected_failure_status = ARCH_TEST_PARALLEL_HASH_OPS_FAILURE_STATUS,
},
#endif
};
