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
#ifndef _TEST_S001_PS_DATA_TESTS_H_
#define _TEST_S001_PS_DATA_TESTS_H_

#include "val_protected_storage.h"

#define SST_FUNCTION val->ps_function

typedef struct {
    enum ps_function_code  api;
    psa_status_t        status;
} test_data;

static struct psa_storage_info_t info;
static const test_data s001_data[] = {
{
 0, 0 /* This is dummy for Index0 */
},
{
 VAL_PS_GET, PSA_ERROR_DOES_NOT_EXIST /* Index1 - Call the get API when no UID is set */
},
{
 VAL_PS_GET_INFO, PSA_ERROR_DOES_NOT_EXIST /* Index2 - Call the get_info API when no UID is set */
},
{
 VAL_PS_REMOVE, PSA_ERROR_DOES_NOT_EXIST /* Index3 - Call the remove API when no UID is set */
},
{
 VAL_PS_SET, PSA_SUCCESS /* Index4 - Create a valid storage entity with UID1 */
},
{
 VAL_PS_SET, PSA_SUCCESS /* Index5 - Create a valid storage entity with UID2 */
},
{
 VAL_PS_REMOVE, PSA_SUCCESS /* Index6 - Remove UID1 */
},
{
 VAL_PS_GET, PSA_ERROR_DOES_NOT_EXIST /* Index7 - Call get API for UID1 */
},
{
 VAL_PS_GET_INFO, PSA_ERROR_DOES_NOT_EXIST /* Index8 - Call get_info API for UID1 */
},
{
 VAL_PS_REMOVE, PSA_ERROR_DOES_NOT_EXIST /* Index9 - Call remove API for UID1 */
},
{
 VAL_PS_SET, PSA_SUCCESS /* Index10 - Create a valid storage entity again with UID1 */
},
{
 VAL_PS_GET, PSA_ERROR_DOES_NOT_EXIST /* Index11 - Call get API for UID not same as UID1 or UID2 */
},
{
 VAL_PS_GET_INFO, PSA_ERROR_DOES_NOT_EXIST /* Index12 - Call get_info for UID not same as UID1
                                                     or UID2 */
},
{
 VAL_PS_REMOVE, PSA_ERROR_DOES_NOT_EXIST /* Index13 - Call remove API for UID not same as UID1
                                                    or UID2 */
},
{
 VAL_PS_REMOVE, PSA_SUCCESS /* Index14 - Remove UID1 */
},
{
 VAL_PS_REMOVE, PSA_SUCCESS /* Index15 - Remove UID2 */
},
};
#endif /*  _TEST_S001_PS_DATA_TESTS_H_  */
