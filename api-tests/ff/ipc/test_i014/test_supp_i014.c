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

#include "val/common/val_client_defs.h"
#include "val/spe/val_partition_common.h"

int32_t server_test_psa_get_called_twice(void);

server_test_t test_i014_server_tests_list[] = {
    NULL,
    server_test_psa_get_called_twice,
    NULL,
};

int32_t server_test_psa_get_called_twice(void)
{
    psa_msg_t   msg = {0};
    int32_t     status = VAL_STATUS_SUCCESS;

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

wait:
    if ((psa_wait(PSA_WAIT_ANY, PSA_BLOCK)) & SERVER_UNSPECIFED_MINOR_V_SIG)
    {
        /* First psa_get call */
        if (psa_get(SERVER_UNSPECIFED_MINOR_V_SIG, &msg) != PSA_SUCCESS)
        {
            goto wait;
        }
        /* Debug print */
        val_err_check_set(TEST_CHECKPOINT_NUM(201), status);

        /* Setting boot.state before test check */
        if (val_set_boot_flag(BOOT_EXPECTED_NS))
        {
            val_print(PRINT_ERROR, "\tFailed to set boot flag before check\n", 0);
            /* Unblock client */
            psa_reply(msg.handle, PSA_CONNECTION_REFUSED);
            return VAL_STATUS_ERROR;
        }

        /* Second psa_get call. This should not return */
        psa_get(SERVER_UNSPECIFED_MINOR_V_SIG, &msg);

        /* shouldn't have reached here */
        /* Resetting boot.state to catch unwanted reboot */
        if (val_set_boot_flag(BOOT_EXPECTED_BUT_FAILED))
        {
            val_print(PRINT_ERROR, "\tFailed to set boot flag after check\n", 0);
        }
        psa_reply(msg.handle, PSA_CONNECTION_REFUSED);
    }

    status = VAL_STATUS_SPM_FAILED;
    val_err_check_set(TEST_CHECKPOINT_NUM(202), status);
    return status;
}
