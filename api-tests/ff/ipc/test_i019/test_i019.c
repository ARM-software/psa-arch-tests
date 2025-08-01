/** @file
 * Copyright (c) 2018-2025, Arm Limited or its affiliates. All rights reserved.
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

#ifdef NONSECURE_TEST_BUILD
#include "val_interfaces.h"
#else
#include "val_client_defs.h"
#include "val_service_defs.h"
#endif

#include "test_i019.h"

const client_test_t test_i019_client_tests_list[] = {
    NULL,
    client_test_psa_set_rhandle_with_null_handle,
    NULL,
};

int32_t client_test_psa_set_rhandle_with_null_handle(caller_security_t caller __UNUSED)
{
   psa_handle_t       handle = 0;

   val->print(TEST, "Check 1: Test psa_set_rhandle with null msg handle\n", 0);

   handle = psa->connect(SERVER_UNSPECIFIED_VERSION_SID, SERVER_UNSPECIFIED_VERSION_VERSION);

   /* Expectation is server test should hang and control shouldn't have come here */
   val->print(ERROR, "\tConnection should failed but succeed\n", 0);

   (void)(handle);
   return VAL_STATUS_SPM_FAILED;
}
