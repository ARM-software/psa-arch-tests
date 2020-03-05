/** @file
 * Copyright (c) 2019-2020, Arm Limited or its affiliates. All rights reserved.
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
#include "test_p013.h"
#include "test_ps_data.h"

#define TEST_BUFF_SIZE 16

client_test_t test_p013_sst_list[] = {
    NULL,
    psa_sst_optional_api_success_check,
    NULL,
};

static psa_storage_uid_t p_uid = UID_BASE_VALUE + 4;
static uint8_t write_buff[TEST_BUFF_SIZE/2] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
static uint8_t write_buff_2[TEST_BUFF_SIZE/2] = {0xFF, 0x11, 0xA2, 0xE3, 0x04, 0xA5, 0xD6, 0x97};
static uint8_t read_buff[TEST_BUFF_SIZE]  = {0};
static uint8_t write_buff_3[TEST_BUFF_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                             0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

static int32_t psa_sst_set_extended_create_success()
{
    uint32_t status, p_data_length = 0;
    struct psa_storage_info_t info;

    /* Create storage of zero length using create API */
    val->print(PRINT_TEST, "[Check 1] Create storage using create API for 0 length\n", 0);
    status = SST_FUNCTION(p013_data[1].api, p_uid, 0, PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, p013_data[1].status, TEST_CHECKPOINT_NUM(1));

    /* Set some data in the storage created */
    val->print(PRINT_TEST, "[Check 2] Call set_extended for zero storage length\n", 0);
    status = SST_FUNCTION(p013_data[2].api, p_uid, 0, 0, write_buff);
    TEST_ASSERT_EQUAL(status, p013_data[2].status, TEST_CHECKPOINT_NUM(2));

    /* Call the get_info function to match attributes */
    status = SST_FUNCTION(p013_data[3].api, p_uid, &info);
    TEST_ASSERT_EQUAL(status, p013_data[3].status, TEST_CHECKPOINT_NUM(3));
    TEST_ASSERT_EQUAL(info.flags, PSA_STORAGE_FLAG_NONE, TEST_CHECKPOINT_NUM(4));
    TEST_ASSERT_EQUAL(info.size, 0, TEST_CHECKPOINT_NUM(5));

    /* Remove the storage */
    val->print(PRINT_TEST, "[Check 3] Remove the storage\n", 0);
    status = SST_FUNCTION(p013_data[6].api, p_uid);
    TEST_ASSERT_EQUAL(status, p013_data[6].status, TEST_CHECKPOINT_NUM(6));

    /* Create a valid storage */
    status = SST_FUNCTION(p013_data[7].api, p_uid, TEST_BUFF_SIZE, PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, p013_data[7].status, TEST_CHECKPOINT_NUM(7));

    /* Try to set data in first half of buffer */
    status = SST_FUNCTION(p013_data[8].api, p_uid, 0, TEST_BUFF_SIZE/2, write_buff);
    TEST_ASSERT_EQUAL(status, p013_data[8].status, TEST_CHECKPOINT_NUM(8));

    /* Try to set data in second half of buffer */
    status = SST_FUNCTION(p013_data[9].api, p_uid, TEST_BUFF_SIZE/2, TEST_BUFF_SIZE/2,\
                          write_buff_2);
    TEST_ASSERT_EQUAL(status, p013_data[9].status, TEST_CHECKPOINT_NUM(9));

    /* Call the get function to match the data */
    val->print(PRINT_TEST, "[Check 4] Valid data written by multiple set_extended\n", 0);
    status = SST_FUNCTION(p013_data[10].api, p_uid, 0, TEST_BUFF_SIZE, read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, p013_data[10].status, TEST_CHECKPOINT_NUM(10));
    TEST_ASSERT_MEMCMP(read_buff, write_buff, TEST_BUFF_SIZE/2, TEST_CHECKPOINT_NUM(11));
    TEST_ASSERT_MEMCMP(read_buff + TEST_BUFF_SIZE/2, write_buff_2, TEST_BUFF_SIZE/2,\
                       TEST_CHECKPOINT_NUM(12));
    TEST_ASSERT_EQUAL(p_data_length, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(13));

    /* Overwrite data using set API */
    val->print(PRINT_TEST, "[Check 5] Overwrite whole data using SET API\n", 0);
    status = SST_FUNCTION(p013_data[13].api, p_uid, TEST_BUFF_SIZE, write_buff_3,
                          PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, p013_data[13].status, TEST_CHECKPOINT_NUM(14));

    /* Call the get function to match the data */
    val->print(PRINT_TEST, "[Check 6] Validate the data using get API\n", 0);
    status = SST_FUNCTION(p013_data[14].api, p_uid, 0, TEST_BUFF_SIZE, read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, p013_data[14].status, TEST_CHECKPOINT_NUM(15));
    TEST_ASSERT_MEMCMP(read_buff, write_buff_3, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(16));
    TEST_ASSERT_EQUAL(p_data_length, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(17));

    /* Call create API for existing UID with same parameters */
    val->print(PRINT_TEST, "[Check 7] Call create API for existing UID with same parameters\n", 0);
    status = SST_FUNCTION(p013_data[16].api, p_uid, TEST_BUFF_SIZE, PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, p013_data[16].status, TEST_CHECKPOINT_NUM(18));

    /* Call the get function to match the data */
    val->print(PRINT_TEST, "[Check 8] Validity of data after create API call\n", 0);
    status = SST_FUNCTION(p013_data[17].api, p_uid, 0, TEST_BUFF_SIZE, read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, p013_data[17].status, TEST_CHECKPOINT_NUM(19));
    TEST_ASSERT_MEMCMP(read_buff, write_buff_3, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(20));
    TEST_ASSERT_EQUAL(p_data_length, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(21));

    /* Remove the storage */
    val->print(PRINT_TEST, "[Check 9] Remove the UID\n", 0);
    status = SST_FUNCTION(p013_data[19].api, p_uid);
    TEST_ASSERT_EQUAL(status, p013_data[19].status, TEST_CHECKPOINT_NUM(22));

    /* Validate there should not be duplicate UID present */
    val->print(PRINT_TEST, "[Check 10] No duplicate entry of UID present\n", 0);
    status = SST_FUNCTION(p013_data[20].api, p_uid, 0, TEST_BUFF_SIZE, read_buff);
    TEST_ASSERT_EQUAL(status, p013_data[20].status, TEST_CHECKPOINT_NUM(23));

    return VAL_STATUS_SUCCESS;
}

int32_t psa_sst_optional_api_success_check(caller_security_t caller)
{
    uint32_t status;
    int32_t test_status;

    /* Call the get_support API and check if create and set_extended API are supported */
    status = SST_FUNCTION(p013_data[0].api);

    if (status == p013_data[0].status)
    {
       val->print(PRINT_INFO, "Optional PS APIs are supported.\n", 0);
       test_status = psa_sst_set_extended_create_success();
       if (test_status != VAL_STATUS_SUCCESS)
          return test_status;
    }
    else
    {
       val->print(PRINT_TEST, "Test Case skipped as Optional PS APIs are not supported.\n", 0);
       return RESULT_SKIP(VAL_STATUS_UNSUPPORTED);
    }

    return VAL_STATUS_SUCCESS;
}
