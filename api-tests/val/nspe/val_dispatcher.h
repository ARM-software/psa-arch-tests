/** @file
 * Copyright (c) 2018-2026, Arm Limited or its affiliates. All rights reserved.
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

#ifndef _VAL_DISPATCHER_H_
#define _VAL_DISPATCHER_H_

#include "val.h"

typedef struct {
    uint32_t suite_num;
    uint32_t test_num;
    uint32_t end_test_num;
    uint32_t test_progress;
} test_info_t;

typedef struct {
    uint32_t total_pass;
    uint32_t total_fail;
    uint32_t total_skip;
    uint32_t total_error;
} regre_report_t;

#define VAL_INVALID_TEST_ID 0xffffffff

uint32_t is_reboot_run(uint32_t test_progress, const uint8_t *pattern, uint32_t length);
void val_reset_regression_report(regre_report_t *report);
void val_update_regression_report(uint32_t test_result, regre_report_t *regre_report);
void val_print_regression_report(regre_report_t *regre_report);

int32_t val_dispatcher(test_id_t test_id_prev);
#endif
