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

#ifdef NONSECURE_TEST_BUILD
#include "val_interfaces.h"
#include "val_target.h"
#else
#include "val/common/val_client_defs.h"
#include "val/spe/val_partition_common.h"
#endif

#include "test_i027.h"

client_test_t test_i027_client_tests_list[] = {
    NULL,
    client_test_psa_drop_connection,
    NULL,
};

int32_t client_test_psa_drop_connection(security_t caller)
{
   int32_t            status = VAL_STATUS_SUCCESS;
   psa_handle_t       handle = 0;
   psa_status_t       status_of_call;
   boot_state_t       boot_state;

   val->print(PRINT_TEST,
            "[Check1] Test PSA_DROP_CONNECTION\n", 0);

   /* Test algorithm:
    *
    * RoT service will terminate the connection by completing the message with a call
    * to psa_reply() using the status code PSA_DROP_CONNECTION.
    *
    * The SPM can implement the required PSA_DROP_CONNECTION behavior in different ways:
    *
    * 1. Return PSA_DROP_CONNECTION to the client from the faulty psa_call() and mark the
    *    SPM connection object so that all future calls to psa_call() on this connection are
    *    immediately failed with PSA_DROP_CONNECTION. And SPM must deliver a PSA_IPC_DISCONNECT
    *    message for the connection to the RoT Service directly after receipt of the
    *    PSA_DROP_CONNECTION completion to allow connection resources within the RoT Service
    *    to be released.
    *
    * 2. Terminate the client task and restart if appropriate for the system
    *
    *
    * If SPM implementation happens to be to first case, the test will expect psa_call to return
    * PSA_DROP_CONNECTION for every future call to psa_call on the connection
    *
    * If SPM implementation happens to be second case, test will expect does not return behaviour
    * for executed psa_call API
   */

   handle = psa->connect(SERVER_CONNECTION_DROP_SID, 1);
   if (handle < 0)
   {
       val->print(PRINT_ERROR, "\tConnection failed\n", 0);
       return VAL_STATUS_INVALID_HANDLE;
   }

   /* Setting boot flag to BOOT_EXPECTED_* to help error recovery if SPM behaviour
    * matches with 2nd case
    */
   boot_state = (caller == NONSECURE) ? BOOT_EXPECTED_NS : BOOT_EXPECTED_S;
   if (val->set_boot_flag(boot_state))
   {
       val->print(PRINT_ERROR, "\tFailed to set boot flag before check\n", 0);
       return VAL_STATUS_ERROR;
   }

   status_of_call =  psa->call(handle, NULL, 0, NULL, 0);

   /* Resetting boot.state to catch unwanted reboot */
   status = val->set_boot_flag(BOOT_NOT_EXPECTED);
   if (VAL_ERROR(status))
   {
       val->print(PRINT_ERROR, "\tFailed to set boot flag after check\n", 0);
       goto exit;
   }

   /* Case 1 implementation */
   if (status_of_call == PSA_DROP_CONNECTION)
   {
        val->print(PRINT_DEBUG, "\tRecieved PSA_DROP_CONNECTION\n", 0);

        status_of_call =  psa->call(handle, NULL, 0, NULL, 0);
        if (status_of_call != PSA_DROP_CONNECTION)
        {
            status = VAL_STATUS_SPM_FAILED;
            val->print(PRINT_ERROR,
                "\tCall should have returned PSA_DROP_CONNECTION. Status = 0x%x\n", status_of_call);
        }
   }
   /* If implementation is case 2, control shouldn't have come here*/
   else
   {
        /* psa_call should hang and control shouldn't have come here */
        status = VAL_STATUS_SPM_FAILED;
        val->print(PRINT_ERROR, "\tCall should failed but successed\n", 0);
   }

exit:
   psa->close(handle);
   return status;
}
