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
#include "test_s006.h"
#ifdef ITS_TEST
#include "test_its_data.h"
#elif PS_TEST
#include "test_ps_data.h"
#endif

#define TEST_BUFF_SIZE 30

client_test_t test_s006_sst_list[] = {
    NULL,
    psa_sst_flags_not_supported,
    NULL,
};

static uint8_t write_buff[TEST_BUFF_SIZE] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x23, 0xF6, 0x07, 0x08, 0x0D, 0x0A, 0x1B, 0x0C, 0x5D, 0x0E,\
  0x70, 0xA1, 0xFF, 0xFF, 0x14, 0x73, 0x46, 0x97, 0xE8, 0xDD, 0xCA, 0x0B, 0x3C, 0x0D, 0x2E};

static int32_t psa_sst_remove_api(psa_storage_uid_t uid, uint32_t data_len,
                                   uint8_t *data_buff, psa_storage_create_flags_t create_flag)
{
    uint32_t status;

    /* Call the get_info function and match the attributes */
    status = SST_FUNCTION(s006_data[2].api, uid, &info);
    TEST_ASSERT_EQUAL(status, s006_data[2].status, TEST_CHECKPOINT_NUM(1));
    if ((create_flag != PSA_STORAGE_FLAG_NO_CONFIDENTIALITY) ||
        (create_flag != PSA_STORAGE_FLAG_NO_REPLAY_PROTECTION))
    {
        TEST_ASSERT_EQUAL(info.flags, create_flag, TEST_CHECKPOINT_NUM(2));
    }

    /* Remove the UID  */
    status = SST_FUNCTION(s006_data[4].api, uid);
    TEST_ASSERT_EQUAL(status, s006_data[4].status, TEST_CHECKPOINT_NUM(3));

    return VAL_STATUS_SUCCESS;
}

static int32_t psa_sst_create_storage_api(psa_storage_uid_t uid, uint32_t data_len,
                                         uint8_t *data_buff, psa_storage_create_flags_t create_flag)
{
    uint32_t status;
    int32_t  test_status;

    status = SST_FUNCTION(s006_data[1].api, uid, data_len, data_buff, create_flag);
    if (status == s006_data[1].status)
    {
        test_status = psa_sst_remove_api(uid, data_len, data_buff, create_flag);
        if (test_status != VAL_STATUS_SUCCESS)
        {
            return test_status;
        }
    }
    else
    {
        /* Remove UID should fail  */
        status = SST_FUNCTION(s006_data[5].api, uid);
        TEST_ASSERT_EQUAL(status, s006_data[5].status, TEST_CHECKPOINT_NUM(4));
    }

    return VAL_STATUS_SUCCESS;
}

int32_t psa_sst_flags_not_supported(caller_security_t caller)
{
    psa_storage_create_flags_t flag;
    uint32_t                   status = VAL_STATUS_SUCCESS;
    psa_storage_uid_t          uid    = UID_BASE_VALUE + 5;
    int32_t                    test_status;

    /* Calling set function with different create flag value */

    val->print(PRINT_TEST, "[Check 1] Call set API with flag - PSA_STORAGE_FLAG_NONE\n", 0);
    /* Create storage with flag value */
    flag = PSA_STORAGE_FLAG_NONE;
    test_status = psa_sst_create_storage_api(uid, TEST_BUFF_SIZE, write_buff, flag);
    if (test_status != VAL_STATUS_SUCCESS)
    {
        return test_status;
    }

    val->print(PRINT_TEST, "[Check 2] Call set API with flag - "
                            "PSA_STORAGE_FLAG_NO_CONFIDENTIALITY\n", 0);
    /* Create storage with flag value */
    flag = PSA_STORAGE_FLAG_NO_CONFIDENTIALITY;
    test_status = psa_sst_create_storage_api(uid, TEST_BUFF_SIZE, write_buff, flag);
    if (test_status != VAL_STATUS_SUCCESS)
    {
        return test_status;
    }

    val->print(PRINT_TEST, "[Check 3] Call set API with flag - "
                            "PSA_STORAGE_FLAG_NO_REPLAY_PROTECTION\n", 0);
    /* Create storage with flag value */
    flag = PSA_STORAGE_FLAG_NO_REPLAY_PROTECTION;
    test_status = psa_sst_create_storage_api(uid, TEST_BUFF_SIZE, write_buff, flag);
    if (test_status != VAL_STATUS_SUCCESS)
    {
        return test_status;
    }

    return status;
}
