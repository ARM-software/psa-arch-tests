/** @file
 * Copyright (c) 2019-2025, Arm Limited or its affiliates. All rights reserved.
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
#include "test_s015.h"

#define TEST_NUM  VAL_CREATE_TEST_ID(VAL_STORAGE_BASE, 15)
#if defined(STORAGE)
    #define TEST_DESC "STORAGE : Desc=Create API write_once flag value check : "
#elif defined(INTERNAL_TRUSTED_STORAGE)
    #define TEST_DESC "ITS : Desc=Create API write_once flag value check : "
#elif defined(PROTECTED_STORAGE)
    #define TEST_DESC "PS : Desc=Create API write_once flag value check : "
#endif

TEST_PUBLISH(TEST_NUM, test_entry);
val_api_t *val = NULL;
psa_api_t *psa = NULL;

void test_entry(val_api_t *val_api, psa_api_t *psa_api)
{
    int32_t   status = VAL_STATUS_SUCCESS;

    val = val_api;
    psa = psa_api;

    /* test init */
    val->test_init(TEST_NUM, VAL_STORAGE_BASE, TEST_DESC,
                             TEST_FIELD(TEST_ISOLATION_L1, WD_HIGH_TIMEOUT));

    if (!IS_TEST_START(val->get_status()))
    {
        goto test_exit;
    }

    /* Execute list of tests available in test[num]_protected_storage_list from Non-secure side*/
    status = val->execute_non_secure_tests(TEST_NUM, s015_storage_test_list, FALSE);

    if (VAL_ERROR(status))
    {
        goto test_exit;
    }

test_exit:
    val->test_exit();
}
