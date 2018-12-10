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

#include "test_c012.h"
#include "test_data.h"
#include "val_crypto.h"

client_test_t test_c012_crypto_list[] = {
    NULL,
    psa_hash_update_test,
    psa_hash_update_invalid_handle,
    psa_hash_update_with_completed_handle,
    NULL,
};

int g_test_count;

int32_t psa_hash_update_test(security_t caller)
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
        if (status != PSA_SUCCESS)
        {
            val->print(PRINT_ERROR, "\tPSA hash setup failed\n", 0);
            return VAL_STATUS_INVALID;
        }

        /* Add a message fragment to a multipart hash operation */
        status = val->crypto_function(VAL_CRYPTO_HASH_UPDATE, &operation,
                                                check1[i].input, check1[i].input_length);
        if (status != check1[i].expected_status)
        {
            val->print(PRINT_ERROR, "\tPSA hash update failed\n", 0);
            return VAL_STATUS_INVALID;
        }
    }

    return VAL_STATUS_SUCCESS;
}

int32_t psa_hash_update_invalid_handle(security_t caller)
{
    int32_t                 status = VAL_STATUS_SUCCESS;
    psa_hash_operation_t    operation;
    uint8_t                 input[] = "Hello World";
    size_t                  input_length = sizeof(input)/sizeof(input[0]);

    /* Initialize the PSA crypto library*/
    if (val->crypto_function(VAL_CRYPTO_INIT) != PSA_SUCCESS)
    {
        return VAL_STATUS_INIT_FAILED;
    }

    val->print(PRINT_TEST, "[Check %d] ", g_test_count++);
    val->print(PRINT_TEST, "Test psa_hash_update without hash setup\n", 0);

    status = val->crypto_function(VAL_CRYPTO_HASH_UPDATE, &operation,
                                            input, input_length);
    if (status != PSA_ERROR_INVALID_ARGUMENT)
    {
        val->print(PRINT_ERROR, "\tPSA hash update should have failed but succeeded\n", 0);
        return VAL_STATUS_INVALID;
    }

    return VAL_STATUS_SUCCESS;
}

int32_t psa_hash_update_with_completed_handle(security_t caller)
{
    int32_t                 status = VAL_STATUS_SUCCESS;
    psa_hash_operation_t    operation;
    uint8_t                 input[] = {0xbd};
    size_t                  input_length = sizeof(input)/sizeof(input[0]);
    psa_algorithm_t         alg = PSA_ALG_SHA_256;
    uint8_t                 hash[] = {0x68, 0x32, 0x57, 0x20, 0xAA, 0xBD, 0x7C, 0x82, 0xF3, 0x0F,
                                      0x55, 0x4B, 0x31, 0x3D, 0x05, 0x70, 0xC9, 0x5A, 0xCC, 0xBB,
                                      0x7D, 0xC4, 0xB5, 0xAA, 0xE1, 0x12, 0x04, 0xC0, 0x8F, 0xFE,
                                      0x73, 0x2B};
    size_t                  hash_length = sizeof(hash)/sizeof(hash[0]);

    /* Initialize the PSA crypto library*/
    if (val->crypto_function(VAL_CRYPTO_INIT) != PSA_SUCCESS)
    {
        return VAL_STATUS_INIT_FAILED;
    }

    val->print(PRINT_TEST, "[Check %d] ", g_test_count++);
    val->print(PRINT_TEST, "Test psa_hash_update with completed opertaion handle \n", 0);

    /* Start a multipart hash operation */
    status = val->crypto_function(VAL_CRYPTO_HASH_SETUP, &operation, alg);
    if (status != PSA_SUCCESS)
    {
        val->print(PRINT_ERROR, "\tPSA hash setup failed\n", 0);
        return VAL_STATUS_INVALID;
    }

    /* Add a message fragment to a multipart hash operation */
    status = val->crypto_function(VAL_CRYPTO_HASH_UPDATE, &operation,
                                            input, input_length);
    if (status != PSA_SUCCESS)
    {
        val->print(PRINT_ERROR, "\tPSA hash update failed\n", 0);
        return VAL_STATUS_INVALID;
    }

    /* Finish the calculation of the hash of a message and compare it with an expected value*/
    status = val->crypto_function(VAL_CRYPTO_HASH_VERIFY, &operation, hash, hash_length);
    if (status != PSA_SUCCESS)
    {
        val->print(PRINT_ERROR, "\tPSA hash verify failed\n", 0);
        return VAL_STATUS_INVALID;
    }

    /* Add a message fragment to a multipart hash operation */
    status = val->crypto_function(VAL_CRYPTO_HASH_UPDATE, &operation,
                                            input, input_length);
    if (status != PSA_ERROR_INVALID_ARGUMENT)
    {
        val->print(PRINT_ERROR, "\tPSA hash update should have failed but succeeded\n", 0);
        return VAL_STATUS_INVALID;
    }
    return VAL_STATUS_SUCCESS;
}
