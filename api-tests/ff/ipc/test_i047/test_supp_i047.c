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

#include "val/common/val_client_defs.h"
#include "val/spe/val_partition_common.h"


int32_t server_test_psa_get_with_invalid_msg_pointer(void);

server_test_t test_i047_server_tests_list[] = {
    NULL,
    server_test_psa_get_with_invalid_msg_pointer,
    NULL,
};

int32_t server_test_psa_get_with_invalid_msg_pointer(void)
{
   int32_t                 status = VAL_STATUS_SUCCESS;
   psa_signal_t            signals = 0;
   psa_msg_t               *invalid_msg = NULL;
   miscellaneous_desc_t    *misc_desc;
   memory_desc_t           *memory_desc;

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

    signals = psa_wait(SERVER_UNSPECIFED_MINOR_V_SIG, PSA_BLOCK);
    if ((signals & SERVER_UNSPECIFED_MINOR_V_SIG) == 0)
    {
       val_print(PRINT_ERROR,
                "psa_wait returned with invalid signal value = 0x%x\n", signals);
       return VAL_STATUS_ERROR;
    }

    /* Selection of invalid msg pointer:
     *  if (ISOLATION_LEVEL > 1)
     *       msg_pointer = driver_mmio_region_base;
     *  else
     *       msg_pointer = NULL;
     */

    status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MISCELLANEOUS,
                                   MISCELLANEOUS_DUT, 0),
                                  (uint8_t **)&misc_desc,
                                  (uint32_t *)sizeof(miscellaneous_desc_t));
    if (val_err_check_set(TEST_CHECKPOINT_NUM(201), status))
    {
        return status;
    }

    if (misc_desc->implemented_psa_firmware_isolation_level > LEVEL1)
    {
        status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY,
                                      MEMORY_DRIVER_PARTITION_MMIO, 0),
                                      (uint8_t **)&memory_desc,
                                      (uint32_t *)sizeof(memory_desc_t));
        if (val_err_check_set(TEST_CHECKPOINT_NUM(202), status))
        {
            return status;
        }

        invalid_msg = (psa_msg_t *) memory_desc->start;
    }

    /* Setting boot.state before test check */
    if (val_set_boot_flag(BOOT_EXPECTED_NS))
    {
        val_print(PRINT_ERROR, "\tFailed to set boot flag before check\n", 0);
        return VAL_STATUS_ERROR;
    }

    /* psa_get with invalid msg pointer */
    psa_get(SERVER_UNSPECIFED_MINOR_V_SIG, invalid_msg);

    /* shouldn't have reached here */
    val_print(PRINT_ERROR,
             "\tpsa_get with invalid msg pointer should failed but successed\n", 0);

    /* Resetting boot.state to catch unwanted reboot */
    if (val_set_boot_flag(BOOT_EXPECTED_BUT_FAILED))
    {
        val_print(PRINT_ERROR, "\tFailed to set boot flag after check\n", 0);
    }

    /* Reject the connection */
    psa_reply(invalid_msg->handle, PSA_CONNECTION_REFUSED);
    return VAL_STATUS_SPM_FAILED;
}
