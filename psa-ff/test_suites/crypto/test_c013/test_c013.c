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

#include "test_c013.h"
#include "test_data.h"
#include "val_crypto.h"

client_test_t test_c013_crypto_list[] = {
    NULL,
    psa_hash_verify_test,
    psa_hash_verify_inactive_operation_handle,
    NULL,
};

int g_test_count;

int32_t psa_hash_verify_test(security_t caller)
{
    int32_t                 status = VAL_STATUS_SUCCESS;
    int                     num_checks = sizeof(check1)/sizeof(check1[0]);
    uint32_t                i;
    psa_hash_operation_t    operation;
    char                    *hash;

    g_test_count = 1;

    /* Initialize the PSA crypto library*/
    if (val->crypto_function(VAL_CRYPTO_INIT) != PSA_SUCCESS)
    {
        return VAL_STATUS_INIT_FAILED;
    }

    for (i = 0; i < num_checks; i++)
    {
        if (check1[i].alg == PSA_ALG_SHA_384)
            hash = sha384_hash;
        else if (check1[i].alg == PSA_ALG_SHA_512)
            hash = sha512_hash;
        else
            hash = check1[i].hash;

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

        /* Finish the calculation of the hash of a message and compare it with an expected value*/
        status = val->crypto_function(VAL_CRYPTO_HASH_VERIFY, &operation, hash,
                                                                            check1[i].hash_length);
        if (status != check1[i].expected_status)
        {
            val->print(PRINT_ERROR, "\tPSA hash verify failed\n", 0);
            return VAL_STATUS_INVALID;
        }
    }

    return VAL_STATUS_SUCCESS;
}

int32_t psa_hash_verify_inactive_operation_handle(security_t caller)
{
    int32_t                 status = VAL_STATUS_SUCCESS;
    psa_hash_operation_t    operation;
    char                    input = 0xbd;
    size_t                  input_length = 1;
    psa_algorithm_t         alg = PSA_ALG_SHA_256;
    size_t                  hash_length = PSA_HASH_SIZE(alg);
    char                    hash[] = {0x68, 0x32, 0x57, 0x20, 0xaa, 0xbd, 0x7c, 0x82, 0xf3, 0x0f,
                                      0x55, 0x4b, 0x31, 0x3d, 0x05, 0x70, 0xc9, 0x5a, 0xcc, 0xbb,
                                      0x7d, 0xc4, 0xb5, 0xaa, 0xe1, 0x12, 0x04, 0xc0, 0x8f, 0xfe,
                                      0x73, 0x2b};

    /* Initialize the PSA crypto library*/
    if (val->crypto_function(VAL_CRYPTO_INIT) != PSA_SUCCESS)
    {
        return VAL_STATUS_INIT_FAILED;
    }

    val->print(PRINT_TEST, "[Check %d] ", g_test_count++);
    val->print(PRINT_TEST, "test psa_hash_verify with inactive operation handle\n", 0);

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

    /* Finish the calculation of the hash of a message and compare it with an expected value*/
    status = val->crypto_function(VAL_CRYPTO_HASH_VERIFY, &operation, hash, hash_length);
    if (status != PSA_SUCCESS)
    {
        val->print(PRINT_ERROR, "\tPSA hash verify failed\n", 0);
        return VAL_STATUS_INVALID;
    }

    /* Retry the operation with completed operation handle */
    status = val->crypto_function(VAL_CRYPTO_HASH_VERIFY, &operation, hash, hash_length);
    if (status != PSA_ERROR_INVALID_ARGUMENT)
    {
        val->print(PRINT_ERROR, "\tPSA hash verify should have failed but succeeded\n", 0);
        return VAL_STATUS_INVALID;
    }

    return VAL_STATUS_SUCCESS;
}
