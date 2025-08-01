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

#include "val_client_defs.h"
#include "val_service_defs.h"

#define val CONCAT(val, _server_sp)
#define psa CONCAT(psa, _server_sp)
extern val_api_t *val;
extern psa_api_t *psa;

#if STATELESS_ROT == 1

int32_t server_test_psa_read_with_invalid_buffer_addr(void);

const server_test_t test_i055_server_tests_list[] = {
    NULL,
    server_test_psa_read_with_invalid_buffer_addr,
    NULL,
};

int32_t server_test_psa_read_with_invalid_buffer_addr(void)
{
    int32_t                 status = VAL_STATUS_SUCCESS;
    psa_msg_t               msg = {0};
    void                    *buffer = NULL;

   /*
    * This test checks for the PROGRAMMER ERROR condition for the PSA API. API's respond to
    * PROGRAMMER ERROR could be either to return appropriate status code or panic the caller.
    * When a Secure Partition panics, the SPE cannot continue normal execution, as defined
    * in this specification. The behavior of the SPM following a Secure Partition panic is
    * IMPLEMENTATION DEFINED- Arm recommends that the SPM causes the system to restart in
    * this situation. Refer PSA-FF for more information on panic.
    * For the cases where, SPM cannot capable to reboot the system (just hangs or power down),
    * a watchdog timer set by val_test_init can reboot the system on timeout event. This will
    * tests continuity and able to jump to next tests. Therefore, each test who checks for
    * PROGRAMMER ERROR condition, expects system to get reset either by SPM or watchdog set by
    * the test harness function.
    *
    * If programmed timeout value isn't sufficient for your system, it can be reconfigured using
    * timeout entries available in pal_config.h.
    *
    * To decide, a reboot happened as intended by test scenario or it happended
    * due to other reasons, test is setting a boot signature into non-volatile memory before and
    * after targeted test check. After a reboot, these boot signatures are being read by the
    * VAL APIs to decide test status.
    */

    /*
     * Selection of invalid buffer addr:
     *
     *  if (ISOLATION_LEVEL > 1)
     *       buffer = driver_mmio_region_base;
     *  else
     *       buffer = NULL;
     */

    if (PLATFORM_PSA_ISOLATION_LEVEL > LEVEL1)
    {
        buffer = (void *) PLATFORM_DRIVER_PARTITION_MMIO_START;
    }

    /* Serve psa_call */
    status = val->process_call_request(SERVER_UNSPECIFIED_VERSION_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(202), status))
    {
        psa->reply(msg.handle, -2);
    }
    else
    {
        /* Setting boot.state before test check */
        status = val->set_boot_flag(BOOT_EXPECTED_NS);
        if (val->err_check_set(TEST_CHECKPOINT_NUM(203), status))
        {
            val->print(ERROR, "\tFailed to set boot flag before check\n", 0);
            psa->reply(msg.handle, -3);
        }
        else
        {
            /* Test check- psa_read with invalid buffer addr, call should panic */
            psa->read(msg.handle, 0, (void *)buffer, msg.in_size[0]);

            /* shouldn't have reached here */
            val->print(ERROR,
                "\tpsa_read with invalid buffer should failed but succeed\n", 0);

            /* Resetting boot.state to catch unwanted reboot */
            if (val->set_boot_flag(BOOT_EXPECTED_BUT_FAILED))
            {
                val->print(ERROR, "\tFailed to set boot flag after check\n", 0);
            }

            status = VAL_STATUS_SPM_FAILED;
            psa->reply(msg.handle, -4);
        }
    }

    val->err_check_set(TEST_CHECKPOINT_NUM(204), status);
    return status;
}

#else

int32_t server_test_psa_read_with_invalid_buffer_addr(void);

const server_test_t test_i055_server_tests_list[] = {
    NULL,
    server_test_psa_read_with_invalid_buffer_addr,
    NULL,
};

int32_t server_test_psa_read_with_invalid_buffer_addr(void)
{
    int32_t                 status = VAL_STATUS_SUCCESS;
    psa_msg_t               msg = {0};
    void                    *buffer = NULL;

   /*
    * This test checks for the PROGRAMMER ERROR condition for the PSA API. API's respond to
    * PROGRAMMER ERROR could be either to return appropriate status code or panic the caller.
    * When a Secure Partition panics, the SPE cannot continue normal execution, as defined
    * in this specification. The behavior of the SPM following a Secure Partition panic is
    * IMPLEMENTATION DEFINED- Arm recommends that the SPM causes the system to restart in
    * this situation. Refer PSA-FF for more information on panic.
    * For the cases where, SPM cannot capable to reboot the system (just hangs or power down),
    * a watchdog timer set by val_test_init can reboot the system on timeout event. This will
    * tests continuity and able to jump to next tests. Therefore, each test who checks for
    * PROGRAMMER ERROR condition, expects system to get reset either by SPM or watchdog set by
    * the test harness function.
    *
    * If programmed timeout value isn't sufficient for your system, it can be reconfigured using
    * timeout entries available in pal_config.h.
    *
    * To decide, a reboot happened as intended by test scenario or it happended
    * due to other reasons, test is setting a boot signature into non-volatile memory before and
    * after targeted test check. After a reboot, these boot signatures are being read by the
    * VAL APIs to decide test status.
    */

    status = val->process_connect_request(SERVER_UNSPECIFIED_VERSION_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(201), status))
    {
        psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
        return status;
    }

    /*
     * Selection of invalid buffer addr:
     *
     *  if (ISOLATION_LEVEL > 1)
     *       buffer = driver_mmio_region_base;
     *  else
     *       buffer = NULL;
     */

    if (PLATFORM_PSA_ISOLATION_LEVEL > LEVEL1)
    {
        buffer = (void *) PLATFORM_DRIVER_PARTITION_MMIO_START;
    }

    /* Accept the connection */
    psa->reply(msg.handle, PSA_SUCCESS);

    /* Serve psa_call */
    status = val->process_call_request(SERVER_UNSPECIFIED_VERSION_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(202), status))
    {
        psa->reply(msg.handle, -2);
    }
    else
    {
        /* Setting boot.state before test check */
        status = val->set_boot_flag(BOOT_EXPECTED_NS);
        if (val->err_check_set(TEST_CHECKPOINT_NUM(203), status))
        {
            val->print(ERROR, "\tFailed to set boot flag before check\n", 0);
            psa->reply(msg.handle, -3);
        }
        else
        {
            /* Test check- psa_read with invalid buffer addr, call should panic */
            psa->read(msg.handle, 0, (void *)buffer, msg.in_size[0]);

            /* shouldn't have reached here */
            val->print(ERROR,
                "\tpsa_read with invalid buffer should failed but succeed\n", 0);

            /* Resetting boot.state to catch unwanted reboot */
            if (val->set_boot_flag(BOOT_EXPECTED_BUT_FAILED))
            {
                val->print(ERROR, "\tFailed to set boot flag after check\n", 0);
            }

            status = VAL_STATUS_SPM_FAILED;
            psa->reply(msg.handle, -4);
        }
    }

    val->err_check_set(TEST_CHECKPOINT_NUM(204), status);
    status = ((val->process_disconnect_request(SERVER_UNSPECIFIED_VERSION_SIGNAL, &msg))
               ? VAL_STATUS_ERROR : status);
    psa->reply(msg.handle, PSA_SUCCESS);
    return status;
}

#endif
