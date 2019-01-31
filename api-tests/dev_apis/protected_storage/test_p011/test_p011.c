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
#include "test_p011.h"
#include "test_ps_data.h"

#define TEST_BUFF_SIZE 16

client_test_t test_p011_sst_list[] = {
    NULL,
    psa_sst_optional_api_uid_not_found,
    NULL,
};

static uint8_t write_buff[TEST_BUFF_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                             0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
static uint8_t read_buff[TEST_BUFF_SIZE]  = {0};

static int32_t psa_sst_uid_not_found()
{
    uint32_t status,j;
    psa_ps_uid_t p_uid = UID_BASE_VALUE + 5;
    struct psa_ps_info_t orig_info;

    /* Call the set_extended API with UID which is not created */
    val->print(PRINT_TEST, "[Check 1] set_extended API call for UID %d which is not set\n", p_uid);
    status = SST_FUNCTION(p011_data[1].api, p_uid, 0, TEST_BUFF_SIZE, write_buff);
    TEST_ASSERT_EQUAL(status, p011_data[1].status, TEST_CHECKPOINT_NUM(1));

    /* Create a valid storage with set API */
    status = SST_FUNCTION(p011_data[2].api, p_uid, TEST_BUFF_SIZE, write_buff, 0);
    TEST_ASSERT_EQUAL(status, p011_data[2].status, TEST_CHECKPOINT_NUM(2));

    /* Try to change data length for same UID using create API */
    val->print(PRINT_TEST, "[Check 2] Call create API with different data length than used to"
                           " create the asset using set API\n", 0);
    status = SST_FUNCTION(p011_data[3].api, p_uid, TEST_BUFF_SIZE/2, 0);
    TEST_ASSERT_EQUAL(status, p011_data[3].status, TEST_CHECKPOINT_NUM(3));

    /* Try to change flag value associated with the UID */
    val->print(PRINT_TEST, "[Check 3] Call create API with different flag value than used to"
                           " create the asset using set API\n", 0);
    status = SST_FUNCTION(p011_data[4].api, p_uid, TEST_BUFF_SIZE, PSA_PS_FLAG_WRITE_ONCE);
    TEST_ASSERT_EQUAL(status, p011_data[4].status, TEST_CHECKPOINT_NUM(4));

    /* Check the flag value should be same as original*/
    status = SST_FUNCTION(p011_data[5].api, p_uid, &orig_info);
    TEST_ASSERT_EQUAL(status, p011_data[5].status, TEST_CHECKPOINT_NUM(5));
    TEST_ASSERT_EQUAL(orig_info.size, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(6));
    TEST_ASSERT_EQUAL(orig_info.flags, 0, TEST_CHECKPOINT_NUM(7));

    /* Remove the UID */
    status = SST_FUNCTION(p011_data[8].api, p_uid);
    TEST_ASSERT_EQUAL(status, p011_data[8].status, TEST_CHECKPOINT_NUM(8));

    /* Create a valid storage */
    status = SST_FUNCTION(p011_data[9].api, p_uid, TEST_BUFF_SIZE/2, 0);
    TEST_ASSERT_EQUAL(status, p011_data[9].status, TEST_CHECKPOINT_NUM(9));

    /* Try to change length using create API */
    val->print(PRINT_TEST, "[Check 4] Call create API with different parameters than used to"
                           " create the asset using create API\n", 0);
    status = SST_FUNCTION(p011_data[10].api, p_uid, TEST_BUFF_SIZE, 0);
    TEST_ASSERT_EQUAL(status, p011_data[10].status, TEST_CHECKPOINT_NUM(10));

    /* Check the storage should be empty */
    status = SST_FUNCTION(p011_data[11].api, p_uid, 0, TEST_BUFF_SIZE, read_buff);
    TEST_ASSERT_EQUAL(status, p011_data[11].status, TEST_CHECKPOINT_NUM(11));
    for (j = 0; j < TEST_BUFF_SIZE; j++)
    {
        TEST_ASSERT_EQUAL(read_buff[j], 0, TEST_CHECKPOINT_NUM(12));
    }

    /* Remove the UID */
    status = SST_FUNCTION(p011_data[13].api, p_uid);
    TEST_ASSERT_EQUAL(status, p011_data[13].api, TEST_CHECKPOINT_NUM(13));

    /* Call the set_extended API with UID which is removed */
    val->print(PRINT_TEST, "[Check 5] set_extended API call for UID %d which is removed\n", p_uid);
    status = SST_FUNCTION(p011_data[14].api, p_uid, 0, TEST_BUFF_SIZE, write_buff);
    TEST_ASSERT_EQUAL(status, p011_data[14].status, TEST_CHECKPOINT_NUM(14));

    return VAL_STATUS_SUCCESS;
}

int32_t psa_sst_optional_api_uid_not_found(security_t caller)
{
    uint32_t status;

    /* Call the get_support API and check if create and set_extended API are supported */
    status = SST_FUNCTION(p011_data[0].api);

    if (status == p011_data[0].status)
    {
       val->print(PRINT_INFO, "Optional PS APIs are supported.\n", 0);
       psa_sst_uid_not_found();
    }
    else
    {
       val->print(PRINT_TEST, "Test Case not needed as Optional PS APIs are not supported.\n", 0);
       return RESULT_SKIP(VAL_STATUS_UNSUPPORTED);
    }

    return VAL_STATUS_SUCCESS;
}

