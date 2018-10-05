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

#include "test_c014.h"
#include "test_data.h"
#include "val_crypto.h"

client_test_t test_c014_crypto_list[] = {
    NULL,
    psa_hash_finish_test,
    psa_hash_finish_inactive_operation_handle,
    psa_hash_finish_invalid_hash_buffer_size,
    NULL,
};

int g_test_count;

int32_t psa_hash_finish_test(security_t caller)
{
    int32_t                 status = VAL_STATUS_SUCCESS;
    int                     num_checks = sizeof(check1)/sizeof(check1[0]);
    uint32_t                i, j;
    psa_hash_operation_t    operation;
    char                    *expected_hash, hash[HASH_64B];
    size_t                  hash_length, hash_size = sizeof(hash)/sizeof(hash[0]);

    g_test_count = 1;

    /* Initialize the PSA crypto library*/
    if (val->crypto_function(VAL_CRYPTO_INIT) != PSA_SUCCESS)
    {
        return VAL_STATUS_INIT_FAILED;
    }

    for (i = 0; i < num_checks; i++)
    {
        if (check1[i].alg == PSA_ALG_SHA_384)
            expected_hash = sha384_hash;
        else if (check1[i].alg == PSA_ALG_SHA_512)
            expected_hash = sha512_hash;
        else
            expected_hash = check1[i].hash;

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
                                                &check1[i].input, check1[i].input_length);
        if (status != PSA_SUCCESS)
        {
            val->print(PRINT_ERROR, "\tPSA hash update failed\n", 0);
            return VAL_STATUS_INVALID;
        }

        /* Finish the calculation of the hash of a message */
        status = val->crypto_function(VAL_CRYPTO_HASH_FINISH, &operation, hash, hash_size,
                                                                                &hash_length);
        if (status != check1[i].expected_status)
        {
            val->print(PRINT_ERROR, "\tPSA hash finish failed\n", 0);
            return VAL_STATUS_INVALID;
        }

        if (status != PSA_SUCCESS)
            continue;

        if (hash_length != PSA_HASH_SIZE(check1[i].alg))
        {
            val->print(PRINT_ERROR, "\tHash length mismatch\n", 0);
            return VAL_STATUS_INVALID;
        }
        for (j = 0; j < hash_length; j++)
        {
            if (hash[i] != expected_hash[i])
            {
                val->print(PRINT_ERROR, "\tHash data mismatch\n", 0);
                return VAL_STATUS_INVALID;
            }
        }
    }

    return VAL_STATUS_SUCCESS;
}

int32_t psa_hash_finish_inactive_operation_handle(security_t caller)
{
    int32_t                 status = VAL_STATUS_SUCCESS;
    psa_hash_operation_t    operation;
    char                    input = 0xbd;
    size_t                  input_length = 1;
    psa_algorithm_t         alg = PSA_ALG_SHA_256;
    char                    hash[HASH_64B];
    size_t                  hash_length, hash_size = sizeof(hash)/sizeof(hash[0]);

    /* Initialize the PSA crypto library*/
    if (val->crypto_function(VAL_CRYPTO_INIT) != PSA_SUCCESS)
    {
        return VAL_STATUS_INIT_FAILED;
    }

    val->print(PRINT_TEST, "[Check %d] ", g_test_count++);
    val->print(PRINT_TEST, "test psa_hash_finish with inactive operation handle\n", 0);

    /* Start a multipart hash operation */
    status = val->crypto_function(VAL_CRYPTO_HASH_SETUP, &operation, alg);
    if (status != PSA_SUCCESS)
    {
        val->print(PRINT_ERROR, "\tPSA hash setup failed\n", 0);
        return VAL_STATUS_INVALID;
    }

    /* Add a message fragment to a multipart hash operation */
    status = val->crypto_function(VAL_CRYPTO_HASH_UPDATE, &operation,
                                            &input, input_length);
    if (status != PSA_SUCCESS)
    {
        val->print(PRINT_ERROR, "\tPSA hash update failed\n", 0);
        return VAL_STATUS_INVALID;
    }

    /* Finish the calculation of the hash of a message */
    status = val->crypto_function(VAL_CRYPTO_HASH_FINISH, &operation, hash, hash_size,
                                                                            &hash_length);
    if (status != PSA_SUCCESS)
    {
        val->print(PRINT_ERROR, "\tPSA hash finish failed\n", 0);
        return VAL_STATUS_INVALID;
    }

    /* Retry the operation with completed operation handle */
    status = val->crypto_function(VAL_CRYPTO_HASH_FINISH, &operation, hash, hash_size,
                                                                            &hash_length);
    if (status != PSA_ERROR_INVALID_ARGUMENT)
    {
        val->print(PRINT_ERROR, "\tPSA hash finish should have failed\n", 0);
        return VAL_STATUS_INVALID;
    }

    return VAL_STATUS_SUCCESS;
}

int32_t psa_hash_finish_invalid_hash_buffer_size(security_t caller)
{
    int32_t                 status = VAL_STATUS_SUCCESS;
    psa_hash_operation_t    operation;
    char                    input = 0xbd;
    size_t                  input_length = 1;
    psa_algorithm_t         alg = PSA_ALG_SHA_256;
    char                    hash[HASH_64B];
    size_t                  hash_length, hash_size = 10;

    /* Initialize the PSA crypto library*/
    if (val->crypto_function(VAL_CRYPTO_INIT) != PSA_SUCCESS)
    {
        return VAL_STATUS_INIT_FAILED;
    }

    val->print(PRINT_TEST, "[Check %d] ", g_test_count++);
    val->print(PRINT_TEST, "test psa_hash_finish with invalid hash buffer size\n", 0);

    /* Start a multipart hash operation */
    status = val->crypto_function(VAL_CRYPTO_HASH_SETUP, &operation, alg);
    if (status != PSA_SUCCESS)
    {
        val->print(PRINT_ERROR, "\tPSA hash setup failed\n", 0);
        return VAL_STATUS_INVALID;
    }

    /* Add a message fragment to a multipart hash operation */
    status = val->crypto_function(VAL_CRYPTO_HASH_UPDATE, &operation,
                                            &input, input_length);
    if (status != PSA_SUCCESS)
    {
        val->print(PRINT_ERROR, "\tPSA hash update failed\n", 0);
        return VAL_STATUS_INVALID;
    }

    /* Finish the calculation of the hash of a message */
    status = val->crypto_function(VAL_CRYPTO_HASH_FINISH, &operation, hash, hash_size,
                                                                            &hash_length);
    if (status != PSA_ERROR_BUFFER_TOO_SMALL)
    {
        val->print(PRINT_ERROR, "\tPSA hash finish should have failed\n", 0);
        return VAL_STATUS_INVALID;
    }

    return VAL_STATUS_SUCCESS;
}
