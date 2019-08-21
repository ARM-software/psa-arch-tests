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
#include "test_s010.h"
#ifdef ITS_TEST
#include "test_its_data.h"
#elif PS_TEST
#include "test_ps_data.h"
#endif

#define TEST_BUFF_SIZE 1

client_test_t test_s010_sst_list[] = {
    NULL,
    psa_sst_uid_value_zero_check,
    NULL,
};

static uint8_t write_buff[TEST_BUFF_SIZE] = {0xFF};

int32_t psa_sst_uid_value_zero_check(caller_security_t caller)
{
    int32_t status;
    psa_storage_uid_t uid = 0;
    struct psa_storage_info_t info;

    /* Set with UID value zero should fail */
    val->print(PRINT_TEST, "[Check 1] Creating storage with UID 0 should fail\n", 0);
    status = SST_FUNCTION(s010_data[0].api, uid, TEST_BUFF_SIZE, write_buff, PSA_STORAGE_FLAG_NONE);
    TEST_ASSERT_EQUAL(status, s010_data[0].status, TEST_CHECKPOINT_NUM(1));

    /* Call to get_info call with UID zero should fail */
    val->print(PRINT_TEST, "[Check 2] Get_info for UID 0 should fail\n", 0);
    status = SST_FUNCTION(s010_data[1].api, uid, &info);
    TEST_ASSERT_EQUAL(status, s010_data[1].status, TEST_CHECKPOINT_NUM(2));

    /* UID removal should fail  */
    val->print(PRINT_TEST, "[Check 3] Removing storage with UID 0 should fail\n", 0);
    status = SST_FUNCTION(s010_data[2].api, uid);
    TEST_ASSERT_EQUAL(status, s010_data[2].status, TEST_CHECKPOINT_NUM(3));

    return VAL_STATUS_SUCCESS;
}

