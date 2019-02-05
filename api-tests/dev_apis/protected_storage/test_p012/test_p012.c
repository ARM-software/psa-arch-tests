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
#include "test_p012.h"
#include "test_ps_data.h"

#define TEST_BUFF_SIZE 16

client_test_t test_p012_sst_list[] = {
    NULL,
    psa_sst_optional_api_offset_invalid,
    NULL,
};

static psa_ps_uid_t p_uid = UID_BASE_VALUE + 6;
static uint8_t write_buff[TEST_BUFF_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                             0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
static uint8_t read_buff[TEST_BUFF_SIZE]  = {0};
static uint8_t write_buff_2[TEST_BUFF_SIZE] = {0xFF, 0xC1, 0xA2, 0xE3, 0x04, 0x05, 0x06, 0x07,
                                             0x03, 0x09, 0x0A, 0x1B, 0x0C, 0x0D, 0x0E, 0x0F};

int32_t psa_sst_offset_invalid()
{
    uint32_t status;

    /* Create valid storage using create API */
    status = SST_FUNCTION(p012_data[1].api, p_uid, TEST_BUFF_SIZE, 0);
    TEST_ASSERT_EQUAL(status, p012_data[1].status, TEST_CHECKPOINT_NUM(1));

    /* Set some data in the storage created */
    status = SST_FUNCTION(p012_data[2].api, p_uid, TEST_BUFF_SIZE/2, 5, write_buff);
    TEST_ASSERT_EQUAL(status, p012_data[2].status, TEST_CHECKPOINT_NUM(2));

    /* Try to set data at invalid location with incorrect data len + offset  */
    val->print(PRINT_TEST, "[Check 1] Call set_extended API with invalid offset + length\n", 0);
    status = SST_FUNCTION(p012_data[3].api, p_uid, TEST_BUFF_SIZE, 2, write_buff);
    TEST_ASSERT_EQUAL(status, p012_data[3].status, TEST_CHECKPOINT_NUM(3));

    /* Try to set data at invalid location with incorrect offset */
    val->print(PRINT_TEST, "[Check 2] Call set_extended API with invalid offset\n", 0);
    status = SST_FUNCTION(p012_data[4].api, p_uid, TEST_BUFF_SIZE + 2, 0, write_buff);
    TEST_ASSERT_EQUAL(status, p012_data[4].status, TEST_CHECKPOINT_NUM(4));

    /* Try to set data at correct offset, but zero data len */
    val->print(PRINT_TEST, "[Check 3] Call set_extended API with offset equals length\n", 0);
    status = SST_FUNCTION(p012_data[5].api, p_uid, TEST_BUFF_SIZE, 0, write_buff);
    TEST_ASSERT_EQUAL(status, p012_data[5].status, TEST_CHECKPOINT_NUM(5));

    /* Try to set data at invalid location with incorrect data len + offset */
    val->print(PRINT_TEST, "[Check 4] Call set_extended API with invalid offset + length\n", 0);
    status = SST_FUNCTION(p012_data[6].api, p_uid, 1, TEST_BUFF_SIZE, write_buff);
    TEST_ASSERT_EQUAL(status, p012_data[6].status, TEST_CHECKPOINT_NUM(6));

    /* Try to set data at invalid location with incorrect data len */
    val->print(PRINT_TEST, "[Check 5] Call set_extended API with invalid length\n", 0);
    status = SST_FUNCTION(p012_data[7].api, p_uid, 0, TEST_BUFF_SIZE + 1, write_buff);
    TEST_ASSERT_EQUAL(status, p012_data[7].status, TEST_CHECKPOINT_NUM(7));

    /* Set data using set API */
    val->print(PRINT_TEST, "[Check 6] Overwrite the whole data with set API\n", 0);
    status = SST_FUNCTION(p012_data[8].api, p_uid, TEST_BUFF_SIZE, write_buff, 0);
    TEST_ASSERT_EQUAL(status, p012_data[8].status, TEST_CHECKPOINT_NUM(8));

    /* Call the get function to check data is correctly overwritten */
    status = SST_FUNCTION(p012_data[9].api, p_uid, 0, TEST_BUFF_SIZE, read_buff);
    TEST_ASSERT_EQUAL(status, p012_data[9].status, TEST_CHECKPOINT_NUM(9));
    TEST_ASSERT_MEMCMP(read_buff, write_buff, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(10));

    return VAL_STATUS_SUCCESS;
}

static int32_t psa_sst_bad_pointer()
{
    uint32_t status;

    /* Call set extended with NULL write_buff */
    val->print(PRINT_TEST, "[Check 7] Call set_extended with NULL write buffer\n", 0);
    status = SST_FUNCTION(p012_data[11].api, p_uid, 0, TEST_BUFF_SIZE, NULL);
    TEST_ASSERT_EQUAL(status, p012_data[11].status, TEST_CHECKPOINT_NUM(11));

    /* Call set extended to overwrite data with new values */
    status = SST_FUNCTION(p012_data[12].api, p_uid, 0, TEST_BUFF_SIZE, write_buff_2);
    TEST_ASSERT_EQUAL(status, p012_data[12].status, TEST_CHECKPOINT_NUM(12));

    /* Call the get function to get the data buffer and match the buffer */
    status = SST_FUNCTION(p012_data[13].api, p_uid, 0, TEST_BUFF_SIZE, read_buff);
    TEST_ASSERT_EQUAL(status, p012_data[13].status, TEST_CHECKPOINT_NUM(13));
    TEST_ASSERT_MEMCMP(read_buff, write_buff_2, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(14));

    /* Remove the storage */
    status = SST_FUNCTION(p012_data[15].api, p_uid);
    TEST_ASSERT_EQUAL(status, p012_data[15].status, TEST_CHECKPOINT_NUM(15));

    return VAL_STATUS_SUCCESS;
}

int32_t psa_sst_optional_api_offset_invalid(security_t caller)
{
    uint32_t status;
    int32_t test_status;

    /* Call the get_support API and check if create and set_extended API are supported */
    status = SST_FUNCTION(p012_data[0].api);

    if (status == p012_data[0].status)
    {
       val->print(PRINT_INFO, "Optional PS APIs are supported.\n", 0);
       test_status = psa_sst_offset_invalid();
       if (test_status != VAL_STATUS_SUCCESS)
          return test_status;

       test_status = psa_sst_bad_pointer();
       if (test_status != VAL_STATUS_SUCCESS)
          return test_status;
    }
    else
    {
       val->print(PRINT_TEST, "Test Case not needed as Optional PS APIs are not supported.\n", 0);
       return RESULT_SKIP(VAL_STATUS_UNSUPPORTED);
    }

    return VAL_STATUS_SUCCESS;
}
