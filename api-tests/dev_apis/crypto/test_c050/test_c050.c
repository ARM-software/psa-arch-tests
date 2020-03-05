/** @file
 * Copyright (c) 2019, Arm Limited or its affiliates. All rights reserved.
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
#include "test_c050.h"
#include "test_data.h"
#include "val_crypto.h"

client_test_t test_c050_crypto_list[] = {
    NULL,
    psa_open_key_test,
    NULL,
};

static int      g_test_count = 1;
static uint8_t  data[BUFFER_SIZE];

int32_t psa_open_key_test(caller_security_t caller)
{
    int32_t               status, i = 0;
    size_t                length, get_key_bits;
    const uint8_t        *key_data;
    psa_key_type_t        get_key_type;
    psa_key_usage_t       get_key_usage_flags;
    psa_algorithm_t       get_key_algorithm;
    psa_key_id_t          key_id;
    psa_key_handle_t      key_handle;
    int                   num_checks = sizeof(check1)/sizeof(check1[0]);
    psa_key_attributes_t  attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t  get_attributes = PSA_KEY_ATTRIBUTES_INIT;
    boot_t                boot;

    if (num_checks == 0)
    {
        val->print(PRINT_TEST, "No test available for the selected crypto configuration\n", 0);
        return RESULT_SKIP(VAL_STATUS_NO_TESTS);
    }

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    /* Get the current boot state */
    status = val->get_boot_flag(&boot.state);
    TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

    /* If the boot is due to controlled reset get the Check ID */
    if (boot.state == BOOT_EXPECTED_CONT_TEST_EXEC)
    {
        status = val->nvmem_read(VAL_NVMEM_OFFSET(NV_TEST_DATA1), &i, sizeof(int32_t));
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(3));
    }

    while (i < num_checks)
    {
        g_test_count = i + 1;
        val->print(PRINT_TEST, "[Check %d] ", g_test_count);
        val->print(PRINT_TEST, check1[i].test_desc, 0);

        /* Setting up the watchdog timer for each check */
        status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(4));

        /* Set the key data buffer to the input base on algorithm */
        if (PSA_KEY_TYPE_IS_RSA(check1[i].key_type))
        {
            if (check1[i].key_type == PSA_KEY_TYPE_RSA_KEY_PAIR)
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
            if (PSA_KEY_TYPE_IS_ECC_KEY_PAIR(check1[i].key_type))
                key_data = ec_keypair;
            else
                key_data = ec_keydata;
        }
        else
            key_data = check1[i].key_data;

        /* Get the cuurent boot state */
        status = val->get_boot_flag(&boot.state);
        TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(5));

        if (boot.state == BOOT_NOT_EXPECTED)
        {
            /* Setup the attributes for the key */
            val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE, &attributes, check1[i].key_type);
            val->crypto_function(VAL_CRYPTO_SET_KEY_BITS, &attributes, check1[i].attr_bits);
            val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &attributes, check1[i].usage);

            if (check1[i].key_lifetime == PSA_KEY_LIFETIME_PERSISTENT)
               val->crypto_function(VAL_CRYPTO_SET_KEY_ID, &attributes, check1[i].key_id);

            val->crypto_function(VAL_CRYPTO_SET_KEY_LIFETIME, &attributes, check1[i].key_lifetime);
            val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM, &attributes, check1[i].key_alg);

            /* Import the key data into the key slot */
            status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, &attributes, key_data,
                     check1[i].key_length, &check1[i].key_handle);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(6));

            /* Save the details of current check, key id and key handle value in NV memory */
            status = val->nvmem_write(VAL_NVMEM_OFFSET(NV_TEST_DATA1), &i, sizeof(int32_t));
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(7));

            status = val->nvmem_write(VAL_NVMEM_OFFSET(NV_TEST_DATA2), &check1[i].key_id,
                     sizeof(psa_key_id_t));
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(8));

            status = val->nvmem_write(VAL_NVMEM_OFFSET(NV_TEST_DATA3), &check1[i].key_handle,
                     sizeof(psa_key_handle_t));
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(9));

            /* Set the boot flag indicating controlled reset */
            status = val->set_boot_flag(BOOT_EXPECTED_CONT_TEST_EXEC);
            TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(10));

            /* Wait for system to reset */
            val->crypto_function(VAL_CRYPTO_RESET);
            while (1);
        }
        else if (boot.state == BOOT_EXPECTED_CONT_TEST_EXEC)
        {
            /* Get the values of check, key id and key handle value before the system was reset */
            status = val->nvmem_read(VAL_NVMEM_OFFSET(NV_TEST_DATA2), &key_id,
                     sizeof(psa_key_id_t));
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(11));

            status = val->nvmem_read(VAL_NVMEM_OFFSET(NV_TEST_DATA3), &key_handle,
                     sizeof(psa_key_handle_t));
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(12));

            /* Open a handle to an existing persistent key */
            status = val->crypto_function(VAL_CRYPTO_OPEN_KEY, key_id, &key_handle);
            TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(13));

            /* If failure is expected, save and continue with the next data set */
            if (check1[i].expected_status != PSA_SUCCESS)
            {
                ++i;
                status = val->nvmem_write(VAL_NVMEM_OFFSET(NV_TEST_DATA1), &i, sizeof(int32_t));
                TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(14));

                status = val->set_boot_flag(BOOT_NOT_EXPECTED);
                TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(15));

                continue;
            }

            /* Get the attributes of the imported key and check if it matches the given value */
            status = val->crypto_function(VAL_CRYPTO_GET_KEY_ATTRIBUTES, key_handle,
                     &get_attributes);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(16));

            val->crypto_function(VAL_CRYPTO_GET_KEY_TYPE, &get_attributes, &get_key_type);
            TEST_ASSERT_EQUAL(get_key_type, check1[i].key_type, TEST_CHECKPOINT_NUM(17));

            val->crypto_function(VAL_CRYPTO_GET_KEY_BITS, &get_attributes, &get_key_bits);
            TEST_ASSERT_EQUAL(get_key_bits, check1[i].expected_bit_length, TEST_CHECKPOINT_NUM(18));

            val->crypto_function(VAL_CRYPTO_GET_KEY_USAGE_FLAGS, &attributes, &get_key_usage_flags);
            TEST_ASSERT_EQUAL(get_key_usage_flags, check1[i].usage, TEST_CHECKPOINT_NUM(19));

            val->crypto_function(VAL_CRYPTO_GET_KEY_ALGORITHM, &attributes, &get_key_algorithm);
            TEST_ASSERT_EQUAL(get_key_algorithm, check1[i].key_alg, TEST_CHECKPOINT_NUM(20));

            /* Export a key in binary format */
            status = val->crypto_function(VAL_CRYPTO_EXPORT_KEY, key_handle, data,
                                          BUFFER_SIZE, &length);
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(21));

            /* Check the value of the exported key */
            TEST_ASSERT_EQUAL(length, check1[i].expected_key_length, TEST_CHECKPOINT_NUM(22));

            if (PSA_KEY_TYPE_IS_UNSTRUCTURED(check1[i].key_type))
            {
                TEST_ASSERT_MEMCMP(data, check1[i].key_data, length, TEST_CHECKPOINT_NUM(23));
            }
            else if (PSA_KEY_TYPE_IS_RSA(check1[i].key_type)
                  || PSA_KEY_TYPE_IS_ECC(check1[i].key_type))
            {
                TEST_ASSERT_MEMCMP(key_data, data, length, TEST_CHECKPOINT_NUM(24));
            }
            else
            {
                return VAL_STATUS_INVALID;
            }

            /* Reset the key attributes */
            val->crypto_function(VAL_CRYPTO_RESET_KEY_ATTRIBUTES, &attributes);

            /* Save the check ID and set boot flags */
             ++i;
            status = val->nvmem_write(VAL_NVMEM_OFFSET(NV_TEST_DATA1), &i, sizeof(int32_t));
            TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(25));

            status = val->set_boot_flag(BOOT_NOT_EXPECTED);
            TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(26));

        }
        else
            return VAL_STATUS_ERROR;
    }

    return VAL_STATUS_SUCCESS;
}
