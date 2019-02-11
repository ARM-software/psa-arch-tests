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
#include "test_p014.h"
#include "test_ps_data.h"

#define TEST_BUFF_SIZE 16

client_test_t test_p014_sst_list[] = {
    NULL,
    psa_sst_optional_api_not_supported_check,
    NULL,
};

static psa_ps_uid_t p_uid = UID_BASE_VALUE + 5;
static uint8_t write_buff[TEST_BUFF_SIZE/4] = { 0x03, 0x04, 0x05, 0x07};
static uint8_t read_buff[TEST_BUFF_SIZE/4]  = {0};

static int32_t psa_sst_optional_api_not_supported()
{
    uint32_t status;
    struct psa_ps_info_t info;

    /* Try to create storage using create API */
    val->print(PRINT_TEST, "[Check 1] Call to create API should fail as API not supported\n", 0);
    status = SST_FUNCTION(p014_data[1].api, p_uid, TEST_BUFF_SIZE, 0);
    TEST_ASSERT_EQUAL(status, p014_data[1].status, TEST_CHECKPOINT_NUM(1));

    /* Verify that UID doesn't exist */
    status = SST_FUNCTION(p014_data[2].api, p_uid, &info);
    TEST_ASSERT_EQUAL(status, p014_data[2].status, TEST_CHECKPOINT_NUM(2));

    /* Create a valid storage using set API */
    val->print(PRINT_TEST, "[Check 2] Create valid storage with set API\n", 0);
    status = SST_FUNCTION(p014_data[3].api, p_uid, TEST_BUFF_SIZE, write_buff, 0);
    TEST_ASSERT_EQUAL(status, p014_data[3].status, TEST_CHECKPOINT_NUM(3));

    /* Partial data write with set_extended API should fail */
    val->print(PRINT_TEST, "[Check 3] Call to set_extended API call should fail\n", 0);
    status = SST_FUNCTION(p014_data[4].api, p_uid, 0, 2, write_buff);
    TEST_ASSERT_EQUAL(status, p014_data[4].status, TEST_CHECKPOINT_NUM(4));

    /* Call the get function to match the data */
    val->print(PRINT_TEST, "[Check 4] Verify data is unchanged\n", 0);
    status = SST_FUNCTION(p014_data[5].api, p_uid, 0, TEST_BUFF_SIZE, read_buff);
    TEST_ASSERT_EQUAL(status, p014_data[5].status, TEST_CHECKPOINT_NUM(5));
    TEST_ASSERT_MEMCMP(read_buff, write_buff, TEST_BUFF_SIZE/4, TEST_CHECKPOINT_NUM(6));

    /* Remove the storage */
    status = SST_FUNCTION(p014_data[7].api, p_uid);
    TEST_ASSERT_EQUAL(status, p014_data[7].status, TEST_CHECKPOINT_NUM(7));

    return VAL_STATUS_SUCCESS;
}

int32_t psa_sst_optional_api_not_supported_check(security_t caller)
{
    uint32_t status;
    int32_t test_status;

    /* Call the get_support API and check if create and set_extended API are supported */
    status = SST_FUNCTION(p014_data[0].api);

    if (status == p014_data[0].status)
    {
       val->print(PRINT_INFO, "Test Case skipped as Optional PS APIs are supported.\n", 0);
       return RESULT_SKIP(VAL_STATUS_UNSUPPORTED);
    }
    else
    {
        val->print(PRINT_TEST, "Optional PS APIs are not supported.\n", 0);
        test_status = psa_sst_optional_api_not_supported();
        if (test_status != VAL_STATUS_SUCCESS)
           return test_status;
    }

    return VAL_STATUS_SUCCESS;
}
