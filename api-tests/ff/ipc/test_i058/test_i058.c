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

#ifdef NONSECURE_TEST_BUILD
#include "val_interfaces.h"
#include "val_target.h"
#else
#include "val_client_defs.h"
#include "val_service_defs.h"
#endif

#include "test_i058.h"

const client_test_t test_i058_client_tests_list[] = {
    NULL,
    client_test_psa_doorbell_signal,
    NULL,
};

int32_t client_test_psa_doorbell_signal(caller_security_t caller __UNUSED)
{
   int32_t            status = VAL_STATUS_SUCCESS;
   psa_handle_t       handle = 0;
#ifndef NONSECURE_TEST_BUILD
   psa_signal_t       signals = 0;
#endif

   val->print(PRINT_TEST,
            "[Check 1] Test PSA_DOORBELL signal\n", 0);

   handle = psa->connect(SERVER_UNSPECIFED_VERSION_SID, SERVER_UNSPECIFED_VERSION_VERSION);
   if (!PSA_HANDLE_IS_VALID(handle))
   {
       val->print(PRINT_ERROR, "\tConnection failed\n", 0);
       return VAL_STATUS_INVALID_HANDLE;
   }

#ifndef NONSECURE_TEST_BUILD
   /* Wait for doorball notification */
   signals = psa_wait(PSA_DOORBELL, PSA_BLOCK);

   /* Is this doorbell signal? */
   if ((signals & PSA_DOORBELL) == 0)
   {
       status = VAL_STATUS_INVALID_HANDLE;
       val->print(PRINT_ERROR, "\tpsa_wait didn't receive doorbell signal\n", 0);
   }

   /*
    * Wait for doorball notification again to check -
    * Doorbell should remain asserted until psa_clear is called.
    */
   signals = psa_wait(PSA_DOORBELL, PSA_BLOCK);

   /* Is this doorbell signal? */
   if ((signals & PSA_DOORBELL) == 0)
   {
       status = VAL_STATUS_INVALID_HANDLE;
       val->print(PRINT_ERROR, "\tDoorbell signal cleared without calling psa_clear\n", 0);
   }

   /* Clear the doorbell signal */
   psa_clear();

   /* Is doorbell signal cleared? */
   signals = psa_wait(PSA_DOORBELL, PSA_POLL);
   if ((signals & PSA_DOORBELL) != 0)
   {
       status = VAL_STATUS_INVALID_HANDLE;
       val->print(PRINT_ERROR, "\tpsa_clear didn't clear doorbell signal\n", 0);
   }
#endif

   psa->close(handle);
   return status;
}
