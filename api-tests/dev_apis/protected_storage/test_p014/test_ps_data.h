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
#ifndef _TEST_P014_PS_DATA_TESTS_H_
#define _TEST_P014_PS_DATA_TESTS_H_

#include "val_protected_storage.h"

#define SST_FUNCTION val->ps_function

typedef struct {
    enum ps_function_code  api;
    psa_status_t        status;
} test_data;

static const test_data p014_data[] = {
{
 VAL_PS_GET_SUPPORT, PSA_STORAGE_SUPPORT_SET_EXTENDED /* Index0 -Check if Optional API supported */
},
{
 VAL_PS_CREATE, PSA_ERROR_NOT_SUPPORTED /* Index1 - Create API call should fail */
},
{
 VAL_PS_GET_INFO, PSA_ERROR_DOES_NOT_EXIST /* Index2 - UID should not exist */
},
{
 VAL_PS_SET, PSA_SUCCESS /* Index3 - Create storage of non-zero length */
},
{
 VAL_PS_SET_EXTENDED, PSA_ERROR_NOT_SUPPORTED /* Index4 - Set_extended API call should fail */
},
{
 VAL_PS_GET, PSA_SUCCESS /* Index5 - Validate the data should not changed */
},
{
 0, 0 /* Unused Index6 */
},
{
 VAL_PS_REMOVE, PSA_SUCCESS /* Index7 - Remove the UID */
},
};
#endif /* _TEST_P014_PS_DATA_TESTS_H_ */
