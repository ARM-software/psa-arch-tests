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
#ifndef _TEST_P012_PS_DATA_TESTS_H_
#define _TEST_P012_PS_DATA_TESTS_H_

#include "val_protected_storage.h"

#define SST_FUNCTION val->ps_function

typedef struct {
    char                   test_desc[100];
    enum ps_function_code  api;
    psa_ps_status_t        status;
} test_data;

static test_data p012_data[] = {
{
 "Check if optional PS api supported", VAL_PS_GET_SUPPORT, PSA_PS_SUPPORT_SET_EXTENDED
},
{
 "Create storage of zero length", VAL_PS_CREATE, PSA_PS_SUCCESS
},
{
 "Call set_extened with zero length", VAL_PS_SET_EXTENDED, PSA_PS_SUCCESS
},
{
 "Validate the storage attributes", VAL_PS_GET_INFO, PSA_PS_SUCCESS
},
{
 "This is dummy for index4", 0, 0
},
{
 "This is dummy for index5", 0, 0
},
{
 "Remove the uid", VAL_PS_REMOVE, PSA_PS_SUCCESS
},
{
 "Create storage of non-zero length", VAL_PS_CREATE, PSA_PS_SUCCESS
},
{
 "Call set_extened to write data in first half of buffer", VAL_PS_SET_EXTENDED, PSA_PS_SUCCESS
},
{
 "Call set_extened to write data in second half of buffer", VAL_PS_SET_EXTENDED, PSA_PS_SUCCESS
},
{
 "Validate the data stored", VAL_PS_GET, PSA_PS_SUCCESS
},
{
 "This is dummy for index11", 0, 0
},
{
 "This is dummy for index12", 0, 0
},
{
 "Overwrite data with set api", VAL_PS_SET, PSA_PS_SUCCESS
},
{
 "Validate the data written", VAL_PS_GET, PSA_PS_SUCCESS
},
{
 "This is dummy for index15", 0, 0
},
{
 "Call Create api for existing uid with same parameters ", VAL_PS_CREATE, PSA_PS_SUCCESS
},
{
 "Validate the data", VAL_PS_GET, PSA_PS_SUCCESS
},
{
 "This is dummy for index18", 0, 0
},
{
 "Remove the uid", VAL_PS_REMOVE, PSA_PS_SUCCESS
},
{
 "Check no duplicate entry of uid present", VAL_PS_GET, PSA_PS_ERROR_UID_NOT_FOUND
}
};
#endif /* _TEST_P012_PS_DATA_TESTS_H_ */
