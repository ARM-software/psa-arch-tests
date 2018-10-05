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

#ifndef _CLIENT_PART_H_
#define _CLIENT_PART_H_
#include "val/common/val_client_defs.h"
#include "val/spe/val_partition_common.h"

typedef client_test_t (*client_test_list_t);

__WEAK extern client_test_t test_i001_client_tests_list[];
__WEAK extern client_test_t test_i002_client_tests_list[];
__WEAK extern client_test_t test_i003_client_tests_list[];
__WEAK extern client_test_t test_i004_client_tests_list[];
__WEAK extern client_test_t test_i005_client_tests_list[];
__WEAK extern client_test_t test_i006_client_tests_list[];
__WEAK extern client_test_t test_i007_client_tests_list[];
__WEAK extern client_test_t test_i008_client_tests_list[];
__WEAK extern client_test_t test_i009_client_tests_list[];
__WEAK extern client_test_t test_i010_client_tests_list[];
__WEAK extern client_test_t test_i011_client_tests_list[];
__WEAK extern client_test_t test_i012_client_tests_list[];
__WEAK extern client_test_t test_i013_client_tests_list[];
__WEAK extern client_test_t test_i014_client_tests_list[];
__WEAK extern client_test_t test_i015_client_tests_list[];
__WEAK extern client_test_t test_i016_client_tests_list[];
__WEAK extern client_test_t test_i017_client_tests_list[];
__WEAK extern client_test_t test_i018_client_tests_list[];
__WEAK extern client_test_t test_i019_client_tests_list[];
__WEAK extern client_test_t test_i020_client_tests_list[];
__WEAK extern client_test_t test_i021_client_tests_list[];
__WEAK extern client_test_t test_i022_client_tests_list[];
__WEAK extern client_test_t test_i023_client_tests_list[];
__WEAK extern client_test_t test_i024_client_tests_list[];
__WEAK extern client_test_t test_i025_client_tests_list[];
__WEAK extern client_test_t test_i026_client_tests_list[];
__WEAK extern client_test_t test_i027_client_tests_list[];
__WEAK extern client_test_t test_i028_client_tests_list[];
__WEAK extern client_test_t test_i029_client_tests_list[];
__WEAK extern client_test_t test_i030_client_tests_list[];
__WEAK extern client_test_t test_i031_client_tests_list[];
__WEAK extern client_test_t test_i032_client_tests_list[];
__WEAK extern client_test_t test_i033_client_tests_list[];
__WEAK extern client_test_t test_i034_client_tests_list[];
__WEAK extern client_test_t test_i035_client_tests_list[];
__WEAK extern client_test_t test_i036_client_tests_list[];
__WEAK extern client_test_t test_i037_client_tests_list[];
__WEAK extern client_test_t test_i038_client_tests_list[];
__WEAK extern client_test_t test_i039_client_tests_list[];
__WEAK extern client_test_t test_i040_client_tests_list[];
__WEAK extern client_test_t test_i041_client_tests_list[];
__WEAK extern client_test_t test_i042_client_tests_list[];
__WEAK extern client_test_t test_i043_client_tests_list[];
__WEAK extern client_test_t test_i044_client_tests_list[];
__WEAK extern client_test_t test_i045_client_tests_list[];
__WEAK extern client_test_t test_i046_client_tests_list[];


client_test_list_t client_ipc_test_list[] = {
    NULL,
    test_i001_client_tests_list,
    test_i002_client_tests_list,
    test_i003_client_tests_list,
    test_i004_client_tests_list,
    test_i005_client_tests_list,
    test_i006_client_tests_list,
    test_i007_client_tests_list,
    test_i008_client_tests_list,
    test_i009_client_tests_list,
    test_i010_client_tests_list,
    test_i011_client_tests_list,
    test_i012_client_tests_list,
    test_i013_client_tests_list,
    test_i014_client_tests_list,
    test_i015_client_tests_list,
    test_i016_client_tests_list,
    test_i017_client_tests_list,
    test_i018_client_tests_list,
    test_i019_client_tests_list,
    test_i020_client_tests_list,
    test_i021_client_tests_list,
    test_i022_client_tests_list,
    test_i023_client_tests_list,
    test_i024_client_tests_list,
    test_i025_client_tests_list,
    test_i026_client_tests_list,
    test_i027_client_tests_list,
    test_i028_client_tests_list,
    test_i029_client_tests_list,
    test_i030_client_tests_list,
    test_i031_client_tests_list,
    test_i032_client_tests_list,
    test_i033_client_tests_list,
    test_i034_client_tests_list,
    test_i035_client_tests_list,
    test_i036_client_tests_list,
    test_i037_client_tests_list,
    test_i038_client_tests_list,
    test_i039_client_tests_list,
    test_i040_client_tests_list,
    test_i041_client_tests_list,
    test_i042_client_tests_list,
    test_i043_client_tests_list,
    test_i044_client_tests_list,
    test_i045_client_tests_list,
    test_i046_client_tests_list,
    NULL,
};

#endif
