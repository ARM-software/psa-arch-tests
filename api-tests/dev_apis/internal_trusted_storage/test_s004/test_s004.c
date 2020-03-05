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
#include "test_s004.h"
#ifdef ITS_TEST
#include "test_its_data.h"
#elif PS_TEST
#include "test_ps_data.h"
#endif

#define TEST_BUFF_SIZE 20

client_test_t test_s004_sst_list[] = {
    NULL,
    psa_sst_get_data_check,
    NULL,
};

static psa_storage_uid_t uid = UID_BASE_VALUE + 5;
static uint8_t read_buff[TEST_BUFF_SIZE] = {0};
static uint8_t write_buff[TEST_BUFF_SIZE] = {0x99, 0x01, 0x30, 0x50, 0x04, 0x23, 0xF6, 0x07, 0x08, \
                                  0x0D, 0x70, 0xA1, 0xFF, 0xFF, 0x14, 0x73, 0x46, 0x97, 0xE8, 0xDD};

int32_t psa_sst_get_data_check(caller_security_t caller)
{
    uint32_t status, p_data_length = 0;

    /* Set data for UID */
    status = SST_FUNCTION(s004_data[1].api, uid, TEST_BUFF_SIZE, write_buff, PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s004_data[1].status, TEST_CHECKPOINT_NUM(1));

    /* Call get function and check the data consistency */
    status = SST_FUNCTION(s004_data[2].api, uid, 0, TEST_BUFF_SIZE, read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, s004_data[2].status, TEST_CHECKPOINT_NUM(2));
    TEST_ASSERT_MEMCMP(read_buff, write_buff, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(3));
    TEST_ASSERT_EQUAL(p_data_length, TEST_BUFF_SIZE, TEST_CHECKPOINT_NUM(4));

    /* Call the set again for same uid and set the length as half */
    val->print(PRINT_TEST, "[Check 1] Call set API with reduced length - TEST_BUFF_SIZE/2\n", 0);
    status = SST_FUNCTION(s004_data[4].api, uid, TEST_BUFF_SIZE/2, write_buff,
                          PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s004_data[4].status, TEST_CHECKPOINT_NUM(5));

    /* Call get function with incorrect buffer length  */
    val->print(PRINT_TEST, "[Check 2] Call get API with default length - TEST_BUFF_SIZE\n", 0);
    status = SST_FUNCTION(s004_data[5].api, uid, 0, TEST_BUFF_SIZE, read_buff, &p_data_length);
    TEST_ASSERT_EQUAL(status, s004_data[5].status, TEST_CHECKPOINT_NUM(6));
    TEST_ASSERT_MEMCMP(read_buff, write_buff, TEST_BUFF_SIZE/2, TEST_CHECKPOINT_NUM(7));
    /* Expect p_data_length = TEST_BUFF_SIZE/2 */
    TEST_ASSERT_EQUAL(p_data_length, TEST_BUFF_SIZE/2,  TEST_CHECKPOINT_NUM(8));

    /* Remove the UID */
    status = SST_FUNCTION(s004_data[7].api, uid);
    TEST_ASSERT_EQUAL(status, s004_data[7].status, TEST_CHECKPOINT_NUM(9));

    return VAL_STATUS_SUCCESS;
}
