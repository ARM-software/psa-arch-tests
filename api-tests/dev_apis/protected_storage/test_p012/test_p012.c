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
    psa_sst_optional_api_success_check,
    NULL,
};

static psa_ps_uid_t p_uid = UID_BASE_VALUE + 11;
static uint8_t write_buff[TEST_BUFF_SIZE/2] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
static uint8_t write_buff_2[TEST_BUFF_SIZE/2] = {0xFF, 0x11, 0xA2, 0xE3, 0x04, 0xA5, 0xD6, 0x97};
static uint8_t read_buff[TEST_BUFF_SIZE]  = {0};
static uint8_t write_buff_3[TEST_BUFF_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                             0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

static int32_t psa_sst_set_extended_create_success()
{
    uint32_t status;
    struct psa_ps_info_t info;

    /* Create storage of zero length using create api */
    val->print(PRINT_TEST, "[Check 1] Create storage using create api for 0 length\n", 0);
    status = SST_FUNCTION(p012_data[1].api, p_uid, 0, 0);
    TEST_ASSERT_EQUAL(status, p012_data[1].status, TEST_CHECKPOINT_NUM(1));

    /* Set some data in the storage created */
    val->print(PRINT_TEST, "[Check 2] Call set_extended for zero storage length\n", 0);
    status = SST_FUNCTION(p012_data[2].api, p_uid, 0, 0, write_buff);
    TEST_ASSERT_EQUAL(status, p012_data[2].status, TEST_CHECKPOINT_NUM(2));

    /* Call the GET_INFO function to match attributes */
    status = SST_FUNCTION(p012_data[3].api, p_uid, &info);
    TEST_ASSERT_EQUAL(status, p012_data[3].status, TEST_CHECKPOINT_NUM(3));
    TEST_ASSERT_EQUAL(info.flags, 0, TEST_CHECKPOINT_NUM(4));
    TEST_ASSERT_EQUAL(info.size, 0, TEST_CHECKPOINT_NUM(5));

    /* Remove the storage */
    val->print(PRINT_TEST, "[Check 3] Remove the storage\n", 0);
    status = SST_FUNCTION(p012_data[6].api, p_uid);
    TEST_ASSERT_EQUAL(status, p012_data[6].status, TEST_CHECKPOINT_NUM(6));

    /* Create a valid storage */
    status = SST_FUNCTION(p012_data[7].api, p_uid, TEST_BUFF_SIZE, 0);
    TEST_ASSERT_EQUAL(status, p012_data[7].status, TEST_CHECKPOINT_NUM(7));

    /* Try to set data in first half of buffer */
    status = SST_FUNCTION(p012_data[8].api, p_uid, 0, TEST_BUFF_SIZE/2, write_buff);
    TEST_ASSERT_EQUAL(status, p012_data[8].status, TEST_CHECKPOINT_NUM(8));

    /* Try to set data in second half of buffer */
    status = SST_FUNCTION(p012_data[9].api, p_uid, TEST_BUFF_SIZE/2, TEST_BUFF_SIZE/2,\
                          write_buff_2);
    TEST_ASSERT_EQUAL(status, p012_data[9].status, TEST_CHECKPOINT_NUM(9));

    /* Call the GET function to match the data */
    val->print(PRINT_TEST, "[Check 4] Valid data written by multiple set_extended\n", 0);
    status = SST_FUNCTION(p012_data[10].api, p_uid, 0, TEST_BUFF_SIZE, read_buff);
    TEST_ASSERT_EQUAL(status, p012_data[10].status, TEST_CHECKPOINT_NUM(10));
    TEST_ASSERT_MEMCMP(read_buff, write_buff, TEST_BUFF_SIZE/2, TEST_CHECKPOINT_NUM(11));
    TEST_ASSERT_MEMCMP(read_buff + TEST_BUFF_SIZE/2, write_buff_2, TEST_BUFF_SIZE/2,\
                       TEST_CHECKPOINT_NUM(12));

    /* Overwrite data using set api */
    val->print(PRINT_TEST, "[Check 5] Overwrite whole data using set api\n", 0);
    status = SST_FUNCTION(p012_data[13].api, p_uid, TEST_BUFF_SIZE, write_buff_3, 0);
    TEST_ASSERT_EQUAL(status, p012_data[13].status, TEST_CHECKPOINT_NUM(13));

    /* Call the GET function to match the data */
    val->print(PRINT_TEST, "[Check 6] Validate the data using get api\n", 0);
    status = SST_FUNCTION(p012_data[14].api, p_uid, 0, TEST_BUFF_SIZE, read_buff);
    TEST_ASSERT_EQUAL(status, p012_data[14].status, TEST_CHECKPOINT_NUM(14));
    TEST_ASSERT_MEMCMP(read_buff, write_buff_3, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(15));

    /* Call create api for existing uid with same parameters */
    val->print(PRINT_TEST, "[Check 7] Call create api for existing uid with same parameters\n", 0);
    status = SST_FUNCTION(p012_data[16].api, p_uid, TEST_BUFF_SIZE, 0);
    TEST_ASSERT_EQUAL(status, p012_data[16].status, TEST_CHECKPOINT_NUM(16));

    /* Call the GET function to match the data */
    val->print(PRINT_TEST, "[Check 8] validity of data after create api call\n", 0);
    status = SST_FUNCTION(p012_data[17].api, p_uid, 0, TEST_BUFF_SIZE, read_buff);
    TEST_ASSERT_EQUAL(status, p012_data[17].status, TEST_CHECKPOINT_NUM(17));
    TEST_ASSERT_MEMCMP(read_buff, write_buff_3, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(18));

    /* Remove the storage */
    val->print(PRINT_TEST, "[Check 9] Remove the uid\n", 0);
    status = SST_FUNCTION(p012_data[19].api, p_uid);
    TEST_ASSERT_EQUAL(status, p012_data[19].status, TEST_CHECKPOINT_NUM(19));

    /* Validate there should not be duplicate uid present */
    val->print(PRINT_TEST, "[Check 10] No duplicate entry of uid present\n", 0);
    status = SST_FUNCTION(p012_data[20].api, p_uid, 0, TEST_BUFF_SIZE, read_buff);
    TEST_ASSERT_EQUAL(status, p012_data[20].status, TEST_CHECKPOINT_NUM(20));

    return VAL_STATUS_SUCCESS;
}

int32_t psa_sst_optional_api_success_check(security_t caller)
{
    uint32_t status;

    /* Call the get_support api and check if create and set_extended api are supported */
    status = SST_FUNCTION(p012_data[0].api);

    if (status == p012_data[0].status)
    {
       val->print(PRINT_INFO, "Optional PS APIs are supported.\n", 0);
       psa_sst_set_extended_create_success();
    }
    else
    {
       val->print(PRINT_TEST, "Test Case not needed as Optional PS APIs are not supported.\n", 0);
       return RESULT_SKIP(VAL_STATUS_UNSUPPORTED);
    }

    return VAL_STATUS_SUCCESS;
}