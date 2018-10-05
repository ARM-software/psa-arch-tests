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
#ifndef _SERVER_PART_H_
#define _SERVER_PART_H_

#include "val/common/val_client_defs.h"
#include "val/spe/val_partition_common.h"

typedef server_test_t (*server_test_list_t);

__WEAK extern server_test_t test_i001_server_tests_list[];
__WEAK extern server_test_t test_i002_server_tests_list[];
__WEAK extern server_test_t test_i003_server_tests_list[];
__WEAK extern server_test_t test_i004_server_tests_list[];
__WEAK extern server_test_t test_i005_server_tests_list[];
__WEAK extern server_test_t test_i006_server_tests_list[];
__WEAK extern server_test_t test_i007_server_tests_list[];
__WEAK extern server_test_t test_i008_server_tests_list[];
__WEAK extern server_test_t test_i009_server_tests_list[];
__WEAK extern server_test_t test_i010_server_tests_list[];
__WEAK extern server_test_t test_i011_server_tests_list[];
__WEAK extern server_test_t test_i012_server_tests_list[];
__WEAK extern server_test_t test_i013_server_tests_list[];
__WEAK extern server_test_t test_i014_server_tests_list[];
__WEAK extern server_test_t test_i015_server_tests_list[];
__WEAK extern server_test_t test_i016_server_tests_list[];
__WEAK extern server_test_t test_i017_server_tests_list[];
__WEAK extern server_test_t test_i018_server_tests_list[];
__WEAK extern server_test_t test_i019_server_tests_list[];
__WEAK extern server_test_t test_i020_server_tests_list[];
__WEAK extern server_test_t test_i021_server_tests_list[];
__WEAK extern server_test_t test_i022_server_tests_list[];
__WEAK extern server_test_t test_i023_server_tests_list[];
__WEAK extern server_test_t test_i024_server_tests_list[];
__WEAK extern server_test_t test_i025_server_tests_list[];
__WEAK extern server_test_t test_i026_server_tests_list[];
__WEAK extern server_test_t test_i027_server_tests_list[];
__WEAK extern server_test_t test_i028_server_tests_list[];
__WEAK extern server_test_t test_i029_server_tests_list[];
__WEAK extern server_test_t test_i030_server_tests_list[];
__WEAK extern server_test_t test_i031_server_tests_list[];
__WEAK extern server_test_t test_i032_server_tests_list[];
__WEAK extern server_test_t test_i033_server_tests_list[];
__WEAK extern server_test_t test_i034_server_tests_list[];
__WEAK extern server_test_t test_i035_server_tests_list[];
__WEAK extern server_test_t test_i036_server_tests_list[];
__WEAK extern server_test_t test_i037_server_tests_list[];
__WEAK extern server_test_t test_i038_server_tests_list[];
__WEAK extern server_test_t test_i039_server_tests_list[];
__WEAK extern server_test_t test_i040_server_tests_list[];
__WEAK extern server_test_t test_i041_server_tests_list[];
__WEAK extern server_test_t test_i042_server_tests_list[];
__WEAK extern server_test_t test_i043_server_tests_list[];
__WEAK extern server_test_t test_i044_server_tests_list[];
__WEAK extern server_test_t test_i045_server_tests_list[];
__WEAK extern server_test_t test_i046_server_tests_list[];

server_test_list_t server_ipc_test_list[] = {
    NULL,
    test_i001_server_tests_list,
    test_i002_server_tests_list,
    test_i003_server_tests_list,
    test_i004_server_tests_list,
    test_i005_server_tests_list,
    test_i006_server_tests_list,
    test_i007_server_tests_list,
    test_i008_server_tests_list,
    test_i009_server_tests_list,
    test_i010_server_tests_list,
    test_i011_server_tests_list,
    test_i012_server_tests_list,
    test_i013_server_tests_list,
    test_i014_server_tests_list,
    test_i015_server_tests_list,
    test_i016_server_tests_list,
    test_i017_server_tests_list,
    test_i018_server_tests_list,
    test_i019_server_tests_list,
    test_i020_server_tests_list,
    test_i021_server_tests_list,
    test_i022_server_tests_list,
    test_i023_server_tests_list,
    test_i024_server_tests_list,
    test_i025_server_tests_list,
    test_i026_server_tests_list,
    test_i027_server_tests_list,
    test_i028_server_tests_list,
    test_i029_server_tests_list,
    test_i030_server_tests_list,
    test_i031_server_tests_list,
    test_i032_server_tests_list,
    test_i033_server_tests_list,
    test_i034_server_tests_list,
    test_i035_server_tests_list,
    test_i036_server_tests_list,
    test_i037_server_tests_list,
    test_i038_server_tests_list,
    test_i039_server_tests_list,
    test_i040_server_tests_list,
    test_i041_server_tests_list,
    test_i042_server_tests_list,
    test_i043_server_tests_list,
    test_i044_server_tests_list,
    test_i045_server_tests_list,
    test_i046_server_tests_list,
    NULL,
};
#endif
