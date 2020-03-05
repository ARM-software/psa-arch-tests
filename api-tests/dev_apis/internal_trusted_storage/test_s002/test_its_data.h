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
#ifndef _TEST_S002_ITS_DATA_TESTS_H_
#define _TEST_S002_ITS_DATA_TESTS_H_

#include "val_internal_trusted_storage.h"

#define SST_FUNCTION val->its_function

typedef struct {
    enum its_function_code  api;
    psa_status_t        status;
} test_data;

static struct psa_storage_info_t orig_info;
static struct psa_storage_info_t new_info;
static const test_data s002_data[] = {
{
 0, 0 /* This is dummy for Index0 */
},
{
 VAL_ITS_SET, PSA_SUCCESS /* Index1 - Create a valid storage with create flag value 0 */
},
{
 VAL_ITS_GET_INFO, PSA_SUCCESS /* Index2 - Call the get_info API to validate the attributes */
},
{
 0, 0 /* This is dummy for Index3 */
},
{
 0, 0 /* This is dummy for Index4 */
},
{
 VAL_ITS_GET, PSA_SUCCESS /* Index5 - Validate the data using get API */
},
{
 0, 0 /* This is dummy for Index6 */
},
{
 VAL_ITS_SET, PSA_SUCCESS /* Index7 - Change the flag to WRITE_ONCE using set API */
},
{
 VAL_ITS_GET_INFO, PSA_SUCCESS /* Index8 - Call the get_info API to validate the flag change */
},
{
 0, 0 /* This is dummy for Index9 */
},
{
 0, 0 /* This is dummy for Index10 */
},
{
 VAL_ITS_GET, PSA_SUCCESS /* Index11 - Validate the data using get API after flag change */
},
{
 0, 0 /* This is dummy for Index12 */
},
{
 VAL_ITS_REMOVE, PSA_ERROR_NOT_PERMITTED /* Index13 - Storage should not be removed after
                                            WRITE_ONCE flag */
},
{
 VAL_ITS_SET, PSA_SUCCESS /* Index14 - Create a storage with different UID and flag value
                             WRITE_ONCE */
},
{
 VAL_ITS_REMOVE, PSA_ERROR_NOT_PERMITTED /* Index15 - Storage should not be removed */
},
{
 VAL_ITS_GET, PSA_SUCCESS /* Index16 - Validate the data using get API after flag change */
},
{
 0, 0 /* This is dummy for Index17 */
},
{
 VAL_ITS_GET_INFO, PSA_SUCCESS /* Index18 - Call the get_info API to validate the flag change */
},
{
 0, 0 /* This is dummy for Index19 */
},
{
 0, 0 /* This is dummy for Index20 */
},
{
 VAL_ITS_SET, PSA_ERROR_NOT_PERMITTED /* Index21 - Try to set different size for same UID and
                                         flag value */
},
{
 VAL_ITS_REMOVE, PSA_ERROR_NOT_PERMITTED /* Index22 - Storage should not be removed */
},
{
 VAL_ITS_GET_INFO, PSA_SUCCESS /* Index23 - Call the get_info API to validate the flag change */
},
{
 0, 0 /* This is dummy for Index24 */
},
{
 0, 0 /* This is dummy for Index25 */
},
{
 VAL_ITS_GET, PSA_SUCCESS /* Index26 - Validate the data using get API after flag change */
},
{
 0, 0 /* This is dummy for Index27 */
},
{
 VAL_ITS_SET, PSA_ERROR_NOT_PERMITTED /* Index28 - Setting flag to zero for UID should fail */
},
{
 VAL_ITS_REMOVE, PSA_ERROR_NOT_PERMITTED /* Index29 - Storage should not be removed */
},
{
 VAL_ITS_GET_INFO, PSA_SUCCESS /* Index30 - Check that the WRITE_ONCE flag is preserved */
},
{
 0, 0 /* This is dummy for Index31 */
},
{
 0, 0 /* This is dummy for Index32 */
},
};
#endif /* _TEST_S002_ITS_DATA_TESTS_H_ */
