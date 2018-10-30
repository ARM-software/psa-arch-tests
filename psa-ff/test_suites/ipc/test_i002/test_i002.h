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
#ifndef _TEST002_CLIENT_TESTS_H_
#define _TEST002_CLIENT_TESTS_H_

#ifdef NONSECURE_TEST_BUILD
#include "val.h"
#else
#include "val/common/val_client_defs.h"
#endif

#define CONNECT_LIMIT 20
#define CONNECT_NUM 2

extern val_api_t *val;
extern psa_api_t *psa;

extern client_test_t test_i002_client_tests_list[];

int32_t client_test_connection_busy_and_reject(security_t);
int32_t client_test_accept_and_close_connect(security_t);
int32_t client_test_connect_with_allowed_minor_version_policy(security_t);
int32_t client_test_psa_call_with_allowed_status_code(security_t);
int32_t client_test_identity(security_t);
int32_t client_test_spm_concurrent_connect_limit(security_t);
int32_t client_test_psa_block_behave(security_t);
int32_t client_test_psa_poll_behave(security_t);
int32_t client_test_psa_wait_bitmask(security_t);
#endif
