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

int32_t server_test_psa_read_with_invalid_buffer_addr(void);

server_test_t test_i055_server_tests_list[] = {
    NULL,
    server_test_psa_read_with_invalid_buffer_addr,
    NULL,
};

int32_t server_test_psa_read_with_invalid_buffer_addr(void)
{
    int32_t                 status = VAL_STATUS_SUCCESS;
    psa_msg_t               msg = {0};
    void                    *buffer = NULL;
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

    status = val_process_connect_request(SERVER_UNSPECIFED_MINOR_V_SIG, &msg);
    if (val_err_check_set(TEST_CHECKPOINT_NUM(201), status))
    {
        psa_reply(msg.handle, PSA_CONNECTION_REFUSED);
        return status;
    }


    /* Selection of invalid buffer addr:
     *  if (ISOLATION_LEVEL > 1)
     *       buffer = driver_mmio_region_base;
     *  else
     *       buffer = NULL;
     */

    status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MISCELLANEOUS,
                                   MISCELLANEOUS_DUT, 0),
                                  (uint8_t **)&misc_desc,
                                  (uint32_t *)sizeof(miscellaneous_desc_t));
    if (val_err_check_set(TEST_CHECKPOINT_NUM(202), status))
    {
        psa_reply(msg.handle, PSA_CONNECTION_REFUSED);
        return status;
    }

    if (misc_desc->implemented_psa_firmware_isolation_level > LEVEL1)
    {
        status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY,
                                      MEMORY_DRIVER_PARTITION_MMIO, 0),
                                      (uint8_t **)&memory_desc,
                                      (uint32_t *)sizeof(memory_desc_t));
        if (val_err_check_set(TEST_CHECKPOINT_NUM(203), status))
        {
            psa_reply(msg.handle, PSA_CONNECTION_REFUSED);
            return status;
        }

        buffer = (void *) memory_desc->start;
    }

    /* Accept the connection */
    psa_reply(msg.handle, PSA_SUCCESS);

    /* Server psa_call */
    status = val_process_call_request(SERVER_UNSPECIFED_MINOR_V_SIG, &msg);
    if (val_err_check_set(TEST_CHECKPOINT_NUM(204), status))
    {
        psa_reply(msg.handle, -2);
    }
    else
    {
        /* Setting boot.state before test check */
        status = val_set_boot_flag(BOOT_EXPECTED_NS);
        if(val_err_check_set(TEST_CHECKPOINT_NUM(205), status))
        {
            val_print(PRINT_ERROR, "\tFailed to set boot flag before check\n", 0);
            psa_reply(msg.handle, -3);
        }
        else
        {
            /* Test check- psa_read with invalid buffer addr */
            psa_read(msg.handle, 0, (void *)buffer, msg.in_size[0]);

            /* shouldn't have reached here */
            val_print(PRINT_ERROR,
                "\tpsa_read with invalid buffer should failed but successed\n", 0);

            /* Resetting boot.state to catch unwanted reboot */
            if (val_set_boot_flag(BOOT_EXPECTED_BUT_FAILED))
            {
                val_print(PRINT_ERROR, "\tFailed to set boot flag after check\n", 0);
            }

            status = VAL_STATUS_SPM_FAILED;
            psa_reply(msg.handle, -4);
        }
    }

    val_err_check_set(TEST_CHECKPOINT_NUM(206), status);
    status = ((val_process_disconnect_request(SERVER_UNSPECIFED_MINOR_V_SIG, &msg))
               ? VAL_STATUS_ERROR : status);
    psa_reply(msg.handle, PSA_SUCCESS);
    return status;
}
