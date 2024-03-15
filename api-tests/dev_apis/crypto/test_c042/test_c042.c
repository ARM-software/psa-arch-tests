/** @file
 * Copyright (c) 2019-2024, Arm Limited or its affiliates. All rights reserved.
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
#include "test_c042.h"
#include "test_data.h"

const client_test_t test_c042_crypto_list[] = {
    NULL,
    psa_verify_hash_test,
    psa_verify_hash_negative_test,
    NULL,
};

/* pre calculated signature and hash values to verify, message = hello */
unsigned char sign[] = {0x7b, 0xff, 0x8a, 0xc2, 0x59, 0xfd, 0xad, 0xd6, 0xe9, 0x17, 0xf6, 0x93,
0x12, 0xc4, 0x30, 0x6e, 0xef, 0x3a, 0x2b, 0x9c, 0x2f, 0xc2, 0x2b, 0x32, 0xc8, 0xd2, 0x68, 0xde,
0xb7, 0x79, 0xdc, 0x07, 0x6a, 0x07, 0x22, 0x68, 0xbf, 0xf9, 0x56, 0x92, 0xe7, 0x0d, 0x88, 0x6a,
0x2d, 0x5c, 0xdc, 0x5f, 0x63, 0x5a, 0x04, 0x65, 0x11, 0x50, 0xeb, 0x1c, 0x04, 0xc9, 0xb0, 0xfe,
0xa0, 0x3a, 0xcc, 0x9a, 0x0f, 0x62, 0x8e, 0x26, 0x35, 0xe8, 0x1e, 0x35, 0x94, 0x4a, 0xaf, 0x76,
0x17, 0xcf, 0x19, 0x3f, 0x84, 0x3b, 0x1b, 0x72, 0x1a, 0x89, 0xa2, 0x10, 0xf6, 0xfb, 0xc9, 0x6e,
0x20, 0xdb, 0x66, 0x53, 0x47, 0x81, 0x45, 0x86, 0xc5, 0x03, 0x28, 0xec, 0xb0, 0xc4, 0x15, 0x56,
0x4c, 0xf3, 0xce, 0x64, 0x13, 0xc6, 0xcc, 0x93, 0x04, 0xd0, 0x79, 0x2e, 0x9e, 0x8f, 0x73, 0x5c,
0x26, 0x09, 0x53, 0x8e};
unsigned char hash[] = {0x2c, 0xf2, 0x4d, 0xba, 0x5f, 0xb0, 0xa3, 0x0e, 0x26, 0xe8, 0x3b, 0x2a,
0xc5, 0xb9, 0xe2, 0x9e, 0x1b, 0x16, 0x1e, 0x5c, 0x1f, 0xa7, 0x42, 0x5e, 0x73, 0x04, 0x33, 0x62,
0x93, 0x8b, 0x98, 0x24};
extern  uint32_t g_test_count;
static int32_t  valid_test_input_index = -1;

int32_t psa_verify_hash_test(caller_security_t caller __UNUSED)
{
    int32_t                 num_checks = sizeof(check1)/sizeof(check1[0]);
    int32_t                 i, status;
    psa_key_attributes_t    attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t            key;

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

        /* Setup the attributes for the key */
        val->crypto_function(VAL_CRYPTO_SET_KEY_TYPE,        &attributes, check1[i].type);
        val->crypto_function(VAL_CRYPTO_SET_KEY_ALGORITHM,   &attributes, check1[i].alg);
        val->crypto_function(VAL_CRYPTO_SET_KEY_USAGE_FLAGS, &attributes, check1[i].usage_flags);

        /* Import the key data into the key slot */
        status = val->crypto_function(VAL_CRYPTO_IMPORT_KEY, &attributes, check1[i].data,
                                      check1[i].data_length, &key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(3));

        /* Verify the signature a hash or short message using a public key */
        status = val->crypto_function(VAL_CRYPTO_VERIFY_HASH,
                                      key,
                                      check1[i].alg,
                                      check1[i].hash,
                                      check1[i].hash_length,
                                      check1[i].signature,
                                      check1[i].signature_length);
        TEST_ASSERT_EQUAL(status, check1[i].expected_status, TEST_CHECKPOINT_NUM(4));

        /* Destroy a key and restore the slot to its default state */
        status = val->crypto_function(VAL_CRYPTO_DESTROY_KEY, key);
        TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(5));

        if (valid_test_input_index < 0)
            valid_test_input_index = i;
    }

    return VAL_STATUS_SUCCESS;
}

int32_t psa_verify_hash_negative_test(caller_security_t caller __UNUSED)
{
    int32_t                 status;
    psa_key_id_t            key = 13;

    if (valid_test_input_index < 0)
        return RESULT_SKIP(VAL_STATUS_NO_TESTS);

    /* Initialize the PSA crypto library*/
    status = val->crypto_function(VAL_CRYPTO_INIT);
    TEST_ASSERT_EQUAL(status, PSA_SUCCESS, TEST_CHECKPOINT_NUM(1));

    val->print(PRINT_TEST, "[Check %d] Test psa_verify_hash - Invalid key handle\n",
                                                                             g_test_count++);

    /* Setting up the watchdog timer for each check */
    status = val->wd_reprogram_timer(WD_CRYPTO_TIMEOUT);
    TEST_ASSERT_EQUAL(status, VAL_STATUS_SUCCESS, TEST_CHECKPOINT_NUM(2));

    /* Verify the signature a hash or short message using a public key */
    status = val->crypto_function(VAL_CRYPTO_VERIFY_HASH,
                                  key,
                                  check1[valid_test_input_index].alg,
                                  check1[valid_test_input_index].hash,
                                  check1[valid_test_input_index].hash_length,
                                  check1[valid_test_input_index].signature,
                                  check1[valid_test_input_index].signature_length);
    TEST_ASSERT_EQUAL(status, PSA_ERROR_INVALID_HANDLE, TEST_CHECKPOINT_NUM(3));

    val->print(PRINT_TEST, "[Check %d] Test psa_verify_hash - Zero as key handle\n",
                                                                             g_test_count++);
    /* Verify the signature a hash or short message using a public key */
    status = val->crypto_function(VAL_CRYPTO_VERIFY_HASH,
                                  0,
                                  check1[valid_test_input_index].alg,
                                  check1[valid_test_input_index].hash,
                                  check1[valid_test_input_index].hash_length,
                                  check1[valid_test_input_index].signature,
                                  check1[valid_test_input_index].signature_length);
    TEST_ASSERT_EQUAL(status, PSA_ERROR_INVALID_HANDLE, TEST_CHECKPOINT_NUM(4));

    return VAL_STATUS_SUCCESS;
}
