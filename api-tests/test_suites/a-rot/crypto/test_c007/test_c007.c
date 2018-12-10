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

#include "test_c007.h"
#include "test_data.h"
#include "val_crypto.h"

client_test_t test_c007_crypto_list[] = {
    NULL,
    psa_set_key_policy_test,
    psa_set_key_policy_negative_test,
    NULL,
};

int g_test_count;

int32_t psa_set_key_policy_test(security_t caller)
{
    int32_t          status = VAL_STATUS_SUCCESS;
    uint8_t          *key_data;
    psa_key_policy_t policy, expected_policy;
    int              num_checks = sizeof(check1)/sizeof(check1[0]);
    uint32_t         i;

    g_test_count = 1;

    /* Initialize the PSA crypto library*/
    if (val->crypto_function(VAL_CRYPTO_INIT) != PSA_SUCCESS)
    {
        return VAL_STATUS_INIT_FAILED;
    }

    /* Initialize a key policy structure to a default that forbids all
     * usage of the key
     */
    val->crypto_function(VAL_CRYPTO_KEY_POLICY_INIT, &policy);

    /* Set the key data buffer to the input base on algorithm */
    for (i = 0; i < num_checks; i++)
    {
        val->print(PRINT_TEST, "[Check %d] ", g_test_count++);
        val->print(PRINT_TEST, check1[i].test_desc, 0);
        if (PSA_KEY_TYPE_IS_RSA(check1[i].key_type))
        {
            if (check1[i].key_type == PSA_KEY_TYPE_RSA_KEYPAIR)
            {
                if (check1[i].expected_bit_length == BYTES_TO_BITS(384))
                    key_data = rsa_384_keypair;
                else if (check1[i].expected_bit_length == BYTES_TO_BITS(256))
                    key_data = rsa_256_keypair;
                else
                    return VAL_STATUS_INVALID;
            }
            else
            {
                if (check1[i].expected_bit_length == BYTES_TO_BITS(384))
                    key_data = rsa_384_keydata;
                else if (check1[i].expected_bit_length == BYTES_TO_BITS(256))
                    key_data = rsa_256_keydata;
                else
                    return VAL_STATUS_INVALID;
            }
        }
        else if (PSA_KEY_TYPE_IS_ECC(check1[i].key_type))
        {
            if (PSA_KEY_TYPE_IS_ECC_KEYPAIR(check1[i].key_type))
                key_data = ec_keypair;
            else
                key_data = ec_keydata;
        }
        else
            key_data = check1[i].key_data;

        /* Set the standard fields of a policy structure */
        val->crypto_function(VAL_CRYPTO_KEY_POLICY_SET_USAGE, &policy, check1[i].usage,
                                                                         check1[i].key_alg);

        /* Set the usage policy on a key slot */
        status = val->crypto_function(VAL_CRYPTO_SET_KEY_POLICY, check1[i].key_slot, &policy);
        if (status != PSA_SUCCESS)
        {
            val->print(PRINT_ERROR, "\tPSA set key policy failed\n", 0);
            return status;
        }

        /* Import the key data into the key slot */
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, check1[i].key_slot, check1[i].key_type,
                                                                key_data, check1[i].key_length);
        if (status != PSA_SUCCESS)
        {
            val->print(PRINT_ERROR, "\tPSA import key failed\n", 0);
            return status;
        }

        /*  Get the usage policy for a key slot */
        status = val->crypto_function(VAL_CRYPTO_GET_KEY_POLICY, check1[i].key_slot,
                                                                                &expected_policy);
        if (status != PSA_SUCCESS)
        {
            val->print(PRINT_ERROR, "\tPSA get key policy failed\n", 0);
            return status;
        }

        if ((expected_policy.usage != check1[i].usage) ||
            (expected_policy.alg !=  check1[i].key_alg))
        {
            val->print(PRINT_ERROR, "\tPolicy data Mismatch\n", 0);
            return VAL_STATUS_DATA_MISMATCH;
        }
    }

    return VAL_STATUS_SUCCESS;
}

int32_t psa_set_key_policy_negative_test(security_t caller)
{
    int32_t          status = VAL_STATUS_SUCCESS;
    int              num_checks = sizeof(check2)/sizeof(check2[0]);
    uint32_t         i;
    psa_key_policy_t policy;

    /* Initialize the PSA crypto library*/
    if (val->crypto_function(VAL_CRYPTO_INIT) != PSA_SUCCESS)
    {
        return VAL_STATUS_INIT_FAILED;
    }

    /* Initialize a key policy structure to a default that forbids all
    * usage of the key
    */
    val->crypto_function(VAL_CRYPTO_KEY_POLICY_INIT, &policy);

    for (i = 0; i < num_checks; i++)
    {
        val->print(PRINT_TEST, "[Check %d] ", g_test_count++);
        val->print(PRINT_TEST, check2[i].test_desc, 0);

       /* Set the usage policy on a key slot */
       val->crypto_function(VAL_CRYPTO_KEY_POLICY_SET_USAGE, &policy, check2[i].usage,
                                                                           check2[i].key_alg);

       /* Set the usage policy on a key slot */
       status = val->crypto_function(VAL_CRYPTO_SET_KEY_POLICY, check2[i].key_slot, &policy);
       if (status != check2[i].expected_status)
       {
           val->print(PRINT_ERROR, "\tPSA set key policy should have failed but succeeded\n", 0);
           return VAL_STATUS_ERROR;
       }
    }

    return VAL_STATUS_SUCCESS;
}
