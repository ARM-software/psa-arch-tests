/** @file
 * Copyright (c) 2019, Arm Limited or its affiliates. All rights reserved.
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
#ifndef _TEST_S009_ITS_DATA_TESTS_H_
#define _TEST_S009_ITS_DATA_TESTS_H_

#include "val_internal_trusted_storage.h"

#define SST_FUNCTION val->its_function
#define psa_sst_uid_t psa_its_uid_t

typedef struct {
    char                    test_desc[100];
    enum its_function_code  api;
    psa_its_status_t        status;
} test_data;

static struct psa_its_info_t info;
static test_data s009_data[] = {
{
 "This is dummy for index0", 0, 0
},
{
 "Call set api with 0 write buffer", VAL_ITS_SET, PSA_ITS_ERROR_BAD_POINTER
},
{
 "Call to get_info api should fail", VAL_ITS_GET_INFO, PSA_ITS_ERROR_KEY_NOT_FOUND
},
{
 "Create storage of zero size", VAL_ITS_SET, PSA_ITS_SUCCESS
},
{
 "Try to set 0 buffer for previous created storage", VAL_ITS_SET, PSA_ITS_ERROR_BAD_POINTER
},
{
 "Call get_info api to check data size", VAL_ITS_GET_INFO, PSA_ITS_SUCCESS
},
{
 "This is dummy for index6", 0, 0
},
{
 "Call get api with 0 read buffer", VAL_ITS_GET, PSA_ITS_ERROR_BAD_POINTER
},
{
 "Call get_info api with 0 info buffer", VAL_ITS_GET_INFO, PSA_ITS_ERROR_BAD_POINTER
},
{
 "Remove the storage entity ", VAL_ITS_REMOVE, PSA_ITS_SUCCESS
},
};
#endif /* _TEST_S009_ITS_DATA_TESTS_H_ */
