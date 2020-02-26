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
#ifndef _TEST_S007_PS_DATA_TESTS_H_
#define _TEST_S007_PS_DATA_TESTS_H_

#include "val_protected_storage.h"

#define SST_FUNCTION val->ps_function

typedef struct {
    enum ps_function_code  api;
    psa_status_t        status;
} test_data;

static const test_data s007_data[] = {
{
 0, 0 /* This is dummy for Index0 */
},
{
 VAL_PS_SET, PSA_SUCCESS /* Index1 - Create a valid storage entity */
},
{
 VAL_PS_SET, PSA_SUCCESS /* Index2 - Increase the length of storage */
},
{
 VAL_PS_GET, PSA_SUCCESS /* Index3 - Try to access old length */
},
{
 VAL_PS_GET, PSA_SUCCESS /* Index4 - Try to access valid length less than set length */
},
{
 0, 0 /* This is dummy for Index5 */
},
{
 VAL_PS_SET, PSA_SUCCESS /* Index6 - Decrease the length of storage */
},
{
 VAL_PS_GET, PSA_SUCCESS /* Index7 - Try to access old length */
},
{
 VAL_PS_GET, PSA_SUCCESS /* Index8 - Try to access old length */
},
{
 VAL_PS_GET, PSA_SUCCESS /* Index9 - Try to access data with correct length */
},
{
 VAL_PS_REMOVE, PSA_SUCCESS /* Index10 - Remove the storage entity */
},
};
#endif /* _TEST_S007_PS_DATA_TESTS_H_ */
