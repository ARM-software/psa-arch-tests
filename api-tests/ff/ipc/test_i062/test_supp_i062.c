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

/* signal bit-31 should be unassigned to server partition */
#define INVALID_SIGNAL_MASK 0x80000000

int32_t server_test_psa_wait_with_unassigned_signal(void);

const server_test_t test_i062_server_tests_list[] = {
    NULL,
    server_test_psa_wait_with_unassigned_signal,
    NULL,
};

int32_t server_test_psa_wait_with_unassigned_signal(void)
{

    val->print(TEST, "[Check 1] Test psa_wait with unassigned signal\n", 0);

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

    /* Setting boot.state before test check */
    if (val->set_boot_flag(BOOT_EXPECTED_NS))
    {
        val->print(ERROR, "\tFailed to set boot flag before check\n", 0);
        return VAL_STATUS_ERROR;
    }

    /* psa_wait with signal mask that doesn't include any assigned signal, call should panic */
    psa->wait(INVALID_SIGNAL_MASK, PSA_POLL);

    /* shouldn't have reached here */
    val->print(ERROR, "\tpsa_wait() check failed for unassigned signal\n", 0);

    /* Resetting boot.state to catch unwanted reboot */
    if (val->set_boot_flag(BOOT_EXPECTED_BUT_FAILED))
    {
        val->print(ERROR, "\tFailed to set boot flag after check\n", 0);
        return VAL_STATUS_ERROR;
    }

    return VAL_STATUS_SPM_FAILED;
}
