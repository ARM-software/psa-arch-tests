/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
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

#ifdef NONSECURE_TEST_BUILD
#include "val_interfaces.h"
#include "val_target.h"
#else
#include "val/common/val_client_defs.h"
#include "val/spe/val_partition_common.h"
#endif

#include "test_c011.h"
#include "test_data.h"
#include "val_crypto.h"

client_test_t test_c011_crypto_list[] = {
    NULL,
    psa_hash_setup_test,
    NULL,
};

int g_test_count;

int32_t psa_hash_setup_test(security_t caller)
{
    int32_t                 status = VAL_STATUS_SUCCESS;
    int                     num_checks = sizeof(check1)/sizeof(check1[0]);
    uint32_t                i;
    psa_hash_operation_t    operation;

    g_test_count = 1;

    /* Initialize the PSA crypto library*/
    if (val->crypto_function(VAL_CRYPTO_INIT) != PSA_SUCCESS)
    {
        return VAL_STATUS_INIT_FAILED;
    }

    for (i = 0; i < num_checks; i++)
    {
        val->print(PRINT_TEST, "[Check %d] ", g_test_count++);
        val->print(PRINT_TEST, check1[i].test_desc, 0);

        /* Start a multipart hash operation */
        status = val->crypto_function(VAL_CRYPTO_HASH_SETUP, &operation, check1[i].alg);
        if (status != check1[i].expected_status)
        {
            val->print(PRINT_ERROR, "\tPSA hash setup failed", 0);
            return VAL_STATUS_INVALID;
        }
    }

    return VAL_STATUS_SUCCESS;
}
