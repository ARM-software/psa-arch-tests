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
#ifndef _TEST_P011_PS_DATA_TESTS_H_
#define _TEST_P011_PS_DATA_TESTS_H_

#include "val_protected_storage.h"

#define SST_FUNCTION val->ps_function

typedef struct {
    enum ps_function_code  api;
    psa_status_t        status;
} test_data;

static const test_data p011_data[] = {
{
 VAL_PS_GET_SUPPORT, PSA_STORAGE_SUPPORT_SET_EXTENDED /* Index0 - Check if optional PS API
                                                                                 supported */
},
{
 VAL_PS_SET_EXTENDED, PSA_ERROR_DOES_NOT_EXIST /* Index1 - Call set_extended call for
                                                                     non-existing UID */
},
{
 VAL_PS_SET, PSA_SUCCESS /* Index2 - Create valid storage using set API */
},
{
 VAL_PS_CREATE, PSA_ERROR_INVALID_ARGUMENT /* Index3 - Create API for present UID with different
                                                                                          length */
},
{
 VAL_PS_CREATE, PSA_ERROR_INVALID_ARGUMENT /* Index4 - Call create API for to set WRITE_ONCE flag */
},
{
 VAL_PS_GET_INFO, PSA_SUCCESS /* Index5 - Validate existing UID attributes maintained */
},
{
 0, 0 /* This is dummy for Index6 */
},
{
 0, 0 /* This is dummy for Index7 */
},
{
 VAL_PS_REMOVE, PSA_SUCCESS /* Index8 - Remove the UID */
},
{
 VAL_PS_CREATE, PSA_SUCCESS /* Index9 - Create valid storage using create API */
},
{
 VAL_PS_CREATE, PSA_ERROR_INVALID_ARGUMENT /* Index10 - Again call create API with different
                                                                                     length */
},
{
 VAL_PS_GET, PSA_SUCCESS /* Index11 - Validate the storage is empty */
},
{
 0, 0 /* This is dummy for Index12 */
},
{
 VAL_PS_REMOVE, PSA_SUCCESS /* Index13 - Remove the UID */
},
{
 VAL_PS_SET_EXTENDED, PSA_ERROR_DOES_NOT_EXIST /* Index14 - Set_extended call for removed UID */
}
};
#endif /* _TEST_P011_PS_DATA_TESTS_H_ */
