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

#include "test_c001.h"

client_test_t test_c001_crypto_list[] = {
    NULL,
    psa_generate_random_without_init_test,
    psa_crypto_init_test,
    multiple_psa_crypto_init_test,
    NULL,
};

int32_t psa_generate_random_without_init_test(security_t caller)
{
    uint8_t         output[GENERATE_SIZE];

    val->print(PRINT_TEST, "[Check1] Test calling crypto functions before psa_crypto_init \n", 0);

    /* Generate random bytes */
    if (val->crypto_function(VAL_CRYPTO_GENERATE_RANDOM, output, GENERATE_SIZE) == PSA_SUCCESS)
    {
        val->print(PRINT_ERROR, "The crypto function should have failed but succeeded\n", 0);
        return VAL_STATUS_CRYPTO_FAILURE;
    }

    return VAL_STATUS_SUCCESS;
}

int32_t psa_crypto_init_test(security_t caller)
{
    int32_t         status = VAL_STATUS_SUCCESS;

    val->print(PRINT_TEST, "[Check2] Test psa_crypto_init\n", status);

    /* Initialize the PSA crypto library*/
    if (val->crypto_function(VAL_CRYPTO_INIT) != PSA_SUCCESS)
    {
        val->print(PRINT_ERROR, "Crypto init failed\n", 0);
        status = VAL_STATUS_INIT_FAILED;
    }

   return status;
}

int32_t multiple_psa_crypto_init_test(security_t caller)
{
    int32_t         i, status = VAL_STATUS_SUCCESS;

    val->print(PRINT_TEST, "[Check3] Test multiple psa_crypto_init \n", 0);
    for (i = 0; i < 5; i++)
    {
        /* Initialize the PSA crypto library*/
        if (val->crypto_function(VAL_CRYPTO_INIT) != PSA_SUCCESS)
        {
            status = VAL_STATUS_INIT_FAILED;
            return status;
        }
    }

   return status;
}
