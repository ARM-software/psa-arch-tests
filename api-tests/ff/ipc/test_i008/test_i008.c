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
#include "val_client_defs.h"
#include "val_partition_common.h"
#endif

#include "test_i008.h"

client_test_t test_i008_client_tests_list[] = {
    NULL,
    client_test_secure_access_only_connection,
    NULL,
};

int32_t client_test_secure_access_only_connection(security_t caller)
{
   int32_t            status = VAL_STATUS_SUCCESS;
   psa_handle_t       handle = 0;

   val->print(PRINT_TEST, "[Check1] Test secure access only connection\n", 0);

   /* Test is targeting fatal error condition and it will expect an error recovery(reboot)
    * to happen. To decide, a reboot happened was intended as per test scenario or it happended
    * due to other reasons, test is setting a boot signature into non-volatile memory before and
    * after targeted test check. After a reboot, these boot signatures are being read by the
    * VAL APIs to decide test status.
    *
    * Note: If SPM is not capable of rebooting (just hangs or power down) in fatal error condition,
    * a watchdog timer enabled by val_test_init can reboot the system on timeout event.
    * If programmed timeout value isn't sufficient for your system, it can be reconfigured using
    * timeout entries available in target.cfg.
    */

   /* Setting boot.state before test check for NS*/
   if(caller == NONSECURE)
   {
       status = val->set_boot_flag(BOOT_EXPECTED_NS);
   }
   if(VAL_ERROR(status))
   {
       val->print(PRINT_ERROR, "\tFailed to set boot flag before check\n", 0);
       return status;
   }
   /* Below psa_connect should not return for call from nspe and
    * should succeed for call from spe
    */
   handle = psa->connect(SERVER_SECURE_CONNECT_ONLY_SID, 1);

   if(caller == NONSECURE)
   {
        /* Shouldn't have reached here for NS run*/
        val->print(PRINT_ERROR, "\tSecure access only connection test failed for NS run\n", 0);

        /* Resetting boot.state to catch unwanted reboot */
        if (val->set_boot_flag(BOOT_EXPECTED_BUT_FAILED))
        {
            val->print(PRINT_ERROR, "\tFailed to set boot flag after check\n", 0);
            return VAL_STATUS_ERROR;
        }
        status = VAL_STATUS_SPM_FAILED;
   }

   /* Should return positive handle for SPE connection */
   if(handle > 0)
   {
      psa->close(handle);
   }
   else
   {
        val->print(PRINT_ERROR, "\tSecure access only connection failed\n", 0);
        status = VAL_STATUS_SPM_FAILED;
   }
   return status;
}
