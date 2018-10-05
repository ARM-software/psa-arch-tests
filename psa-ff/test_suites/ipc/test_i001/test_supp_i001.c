/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
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

#include "val/common/val_client_defs.h"
#include "val/spe/val_partition_common.h"

int32_t server_test_psa_framework_version(void);
int32_t server_test_psa_version(void);

server_test_t test_i001_server_tests_list[] = {
    NULL,
    server_test_psa_framework_version,
    server_test_psa_version,
    NULL,
};

int32_t server_test_psa_framework_version(void)
{
    val_err_check_set(TEST_CHECKPOINT_NUM(201), VAL_STATUS_SUCCESS);
    return VAL_STATUS_SUCCESS;
}

int32_t server_test_psa_version(void)
{
    val_err_check_set(TEST_CHECKPOINT_NUM(202), VAL_STATUS_SUCCESS);
    return VAL_STATUS_SUCCESS;
}
