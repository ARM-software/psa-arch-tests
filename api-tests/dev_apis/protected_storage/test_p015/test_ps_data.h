/** @file
 * Copyright (c) 2019, Arm Limited or ps affiliates. All rights reserved.
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
#ifndef _TEST_P015_PS_DATA_TESTS_H_
#define _TEST_P015_PS_DATA_TESTS_H_

#include "val_protected_storage.h"

#define SST_FUNCTION val->ps_function
#define PSA_SST_FLAG_WRITE_ONCE PSA_PS_FLAG_WRITE_ONCE

typedef struct {
    enum ps_function_code  api;
    psa_ps_status_t        status;
} test_data;

static const test_data p015_data[] = {
{
 VAL_PS_GET_SUPPORT, PSA_PS_SUPPORT_SET_EXTENDED /* Check if optional PS API supported */
},
{
 VAL_PS_CREATE, PSA_PS_ERROR_FLAGS_NOT_SUPPORTED /* Call create API with write_once flag */
},
};
#endif /* _TEST_P015_PS_DATA_TESTS_H_ */
