/** @file
 * Copyright (c) 2019, Arm Limited or its affiliates. All rights reserved.
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

#include "test_i050.h"

client_test_t test_i050_client_tests_list[] = {
    NULL,
    client_test_psa_call_with_invalid_invec_base,
    NULL,
};

int32_t client_test_psa_call_with_invalid_invec_base(security_t caller)
{
   int32_t                 status = VAL_STATUS_SUCCESS;
   psa_handle_t            handle = 0;
   psa_status_t            status_of_call;
   boot_state_t            boot_state;
   miscellaneous_desc_t    *misc_desc;
   memory_desc_t           *memory_desc;
   addr_t                  *invalid_base = NULL;

   val->print(PRINT_TEST,
            "[Check1] Test psa_call with invalid psa_invec.base\n", 0);

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

   handle = psa->connect(SERVER_UNSPECIFED_MINOR_V_SID, 1);

   if (handle < 0)
   {
       val->print(PRINT_ERROR, "\tConnection failed\n", 0);
       return VAL_STATUS_INVALID_HANDLE;
   }

   /* Selection of invalid invec pointer:
    * if caller == NONSECURE
    *    // PSA RoT pointer
    *    invalid_base = driver_mmio_region_base;
    * else
    *    if (ISOLATION_LEVEL > 1)
    *       invalid_base = driver_mmio_region_base;
    *    else
    *       invalid_base = NULL;
    */

   status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY,
                                  MEMORY_DRIVER_PARTITION_MMIO, 0),
                                  (uint8_t **)&memory_desc,
                                  (uint32_t *)sizeof(memory_desc_t));
   if (val->err_check_set(TEST_CHECKPOINT_NUM(101), status))
   {
       psa->close(handle);
       return status;
   }

   if (caller == NONSECURE)
       invalid_base = (addr_t *) memory_desc->start;
   else
   {
       status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MISCELLANEOUS,
                                   MISCELLANEOUS_DUT, 0),
                                  (uint8_t **)&misc_desc,
                                  (uint32_t *)sizeof(miscellaneous_desc_t));

       if (val->err_check_set(TEST_CHECKPOINT_NUM(102), status))
       {
           psa->close(handle);
           return status;
       }

       if (misc_desc->implemented_psa_firmware_isolation_level > LEVEL1)
           invalid_base = (addr_t *) memory_desc->start;
   }

   /* Setting boot.state before test check */
   boot_state = (caller == NONSECURE) ? BOOT_EXPECTED_NS : BOOT_EXPECTED_S;
   if (val->set_boot_flag(boot_state))
   {
       val->print(PRINT_ERROR, "\tFailed to set boot flag before check\n", 0);
       return VAL_STATUS_ERROR;
   }

   psa_invec invec[1] = {{invalid_base, sizeof(addr_t)}};

   /* Test check- psa_call with invalid address for psa_invec.base */
   status_of_call =  psa->call(handle, invec, 1, NULL, 0);

   /* Expectation is psa_call should hang and control shouldn't have come here */
   val->print(PRINT_ERROR, "\tpsa_call should failed but successed\n", 0);

   /* Resetting boot.state to catch unwanted reboot */
   if (val->set_boot_flag(BOOT_EXPECTED_BUT_FAILED))
   {
       val->print(PRINT_ERROR, "\tFailed to set boot flag after check\n", 0);
       return VAL_STATUS_ERROR;
   }

   status = VAL_STATUS_SPM_FAILED;
   psa->close(handle);
   (void)(status_of_call);
   return status;
}
