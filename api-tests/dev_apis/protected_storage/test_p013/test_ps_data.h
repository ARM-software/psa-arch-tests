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
#ifndef _TEST_P013_PS_DATA_TESTS_H_
#define _TEST_P013_PS_DATA_TESTS_H_

#include "val_protected_storage.h"

#define SST_FUNCTION val->ps_function

typedef struct {
    enum ps_function_code  api;
    psa_status_t        status;
} test_data;

static const test_data p013_data[] = {
{
 VAL_PS_GET_SUPPORT, PSA_STORAGE_SUPPORT_SET_EXTENDED /* Index0 - Check if optional PS API
                                                                                 supported */
},
{
 VAL_PS_CREATE, PSA_SUCCESS /* Index1 - Create storage of zero length */
},
{
 VAL_PS_SET_EXTENDED, PSA_SUCCESS /* Index2 - Call set_extended with zero length */
},
{
 VAL_PS_GET_INFO, PSA_SUCCESS /* Index3 - Validate the storage attributes */
},
{
 0, 0 /* This is dummy for Index4 */
},
{
 0, 0 /* This is dummy for Index5 */
},
{
 VAL_PS_REMOVE, PSA_SUCCESS /* Index6 - Remove the UID */
},
{
 VAL_PS_CREATE, PSA_SUCCESS /* Index7 - Create storage of non-zero length */
},
{
 VAL_PS_SET_EXTENDED, PSA_SUCCESS /* Index8 - Call set_extended to write data in first half of
                                                                                        buffer */
},
{
 VAL_PS_SET_EXTENDED, PSA_SUCCESS /* Index9 - Call set_extended to write data in second half of
                                                                                         buffer */
},
{
 VAL_PS_GET, PSA_SUCCESS /* Index10 - Validate the data stored */
},
{
 0, 0 /* This is dummy for Index11 */
},
{
 0, 0 /* This is dummy for Index12 */
},
{
 VAL_PS_SET, PSA_SUCCESS /* Index13 - Overwrite data with set API */
},
{
 VAL_PS_GET, PSA_SUCCESS /* Index14 - Validate the data written */
},
{
 0, 0 /* This is dummy for Index15 */
},
{
 VAL_PS_CREATE, PSA_SUCCESS /* Index16 - Call Create API for existing UID with same parameters */
},
{
 VAL_PS_GET, PSA_SUCCESS /* Index17 - Validate the data */
},
{
 0, 0 /* This is dummy for Index18 */
},
{
 VAL_PS_REMOVE, PSA_SUCCESS /* Index19 - Remove the UID */
},
{
 VAL_PS_GET, PSA_ERROR_DOES_NOT_EXIST /* Index20 - Check no duplicate entry of UID present */
}
};
#endif /* _TEST_P013_PS_DATA_TESTS_H_ */
