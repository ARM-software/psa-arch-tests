/** @file
 * Copyright (c) 2019-2025, Arm Limited or its affiliates. All rights reserved.
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

#include "test_i057.h"

#if STATELESS_ROT == 1

const client_test_t test_i057_client_tests_list[] = {
    NULL,
    client_test_psa_write_with_invalid_buffer_addr,
    NULL,
};

int32_t client_test_psa_write_with_invalid_buffer_addr(caller_security_t caller __UNUSED)
{
   int32_t            status = VAL_STATUS_SUCCESS;
   uint8_t            data = 0x11;
   psa_status_t       status_of_call;

   val->print(TEST,
            "Check 1: Test psa_write with invalid buffer addr\n", 0);

   psa_outvec outvec[1] = { {&data, sizeof(data)} };

   status_of_call =  psa->call(SERVER_UNSPECIFIED_VERSION_HANDLE, PSA_IPC_CALL, NULL, 0, outvec, 1);

   /* Expectation is server test should hang and control shouldn't have come here */
   val->print(ERROR, "\tCall should failed but succeed\n", 0);

   status = VAL_STATUS_SPM_FAILED;

   (void)(status_of_call);
   return status;
}

#else

const client_test_t test_i057_client_tests_list[] = {
    NULL,
    client_test_psa_write_with_invalid_buffer_addr,
    NULL,
};

int32_t client_test_psa_write_with_invalid_buffer_addr(caller_security_t caller __UNUSED)
{
   int32_t            status = VAL_STATUS_SUCCESS;
   uint8_t            data = 0x11;
   psa_handle_t       handle = 0;
   psa_status_t       status_of_call;

   val->print(TEST,
            "Check 1: Test psa_write with invalid buffer addr\n", 0);

   handle = psa->connect(SERVER_UNSPECIFIED_VERSION_SID, SERVER_UNSPECIFIED_VERSION_VERSION);
   if (!PSA_HANDLE_IS_VALID(handle))
   {
       val->print(ERROR, "\tConnection failed\n", 0);
       return VAL_STATUS_INVALID_HANDLE;
   }

   psa_outvec outvec[1] = {{&data, sizeof(data)} };

   status_of_call =  psa->call(handle, PSA_IPC_CALL, NULL, 0, outvec, 1);

   /* Expectation is server test should hang and control shouldn't have come here */
   val->print(ERROR, "\tCall should failed but succeed\n", 0);

   status = VAL_STATUS_SPM_FAILED;

   psa->close(handle);

   (void)(status_of_call);
   return status;
}

#endif
