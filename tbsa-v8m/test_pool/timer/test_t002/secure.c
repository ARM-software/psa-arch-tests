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

#include "val_test_common.h"

#define WDOG_TIMEOUT_VALUE_IN_US        (1 * 1000 * 1000)   /* 1sec : 1 * 1000 * 1000 */
#define TIMEOUT_VALUE                   0xFFFFFFFF

/**
  Publish these functions to the external world as associated to this test ID
**/
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_TRUSTED_TIMERS_BASE, 2),
                  CREATE_TEST_TITLE("Trusted and Non-trusted world operation to trusted watchdog timer"),
                  CREATE_REF_TAG("R060/R070/R080/R100/R110_TBSA_TIME"),
                  entry_hook,
                  test_payload,
                  exit_hook);

tbsa_val_api_t        *g_val;
soc_peripheral_hdr_t  *soc_per;
soc_peripheral_desc_t *soc_per_desc;
memory_desc_t         *memory_desc;
clocks_desc_t         *clocks_desc;
bool_t                trusted_wd_timer_found;

void
secure_fault_esr (unsigned long *sf_args)
{
    if (IS_TEST_PENDING(g_val->get_status())) {
        g_val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
    } else {
        g_val->set_status(RESULT_FAIL(TBSA_STATUS_INVALID));
    }

    /* Updating the return address in the stack frame in order to avoid periodic fault */
    sf_args[6] = sf_args[6] + 4;
}

__attribute__((naked))
void
SF_Handler(void)
{
    asm volatile("mrs r0, control_ns \n"
                 "mov r1, #0x2       \n"
                 "and r0, r1         \n"
                 "cmp r0, r1         \n"
                 "beq _psp_ns        \n"
                 "mrs r0, msp_ns     \n"
                 "b secure_fault_esr \n"
                 "_psp_ns:           \n"
                 "mrs r0, psp_ns     \n"
                 "b secure_fault_esr \n");
}

void
setup_ns_env(void)
{
    tbsa_status_t status;

    /* Installing Trusted Fault Handler for NS test */
    status = g_val->interrupt_setup_handler(EXCP_NUM_SF, 0, SF_Handler);
    g_val->err_check_set(TEST_CHECKPOINT_9, status);
}

void
entry_hook(tbsa_val_api_t *val)
{
    tbsa_test_init_t init = {
                             .bss_start      = &__tbsa_test_bss_start__,
                             .bss_end        = &__tbsa_test_bss_end__
                            };

    val->test_initialize(&init);

    g_val = val;
    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void
test_payload(tbsa_val_api_t *val)
{
    uint32_t      instance = 0;
    uint32_t      per_num  = 0;
    tbsa_status_t status;
    boot_t        boot;
    uint32_t      timeout = TIMEOUT_VALUE;

    trusted_wd_timer_found = FALSE;

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_NVRAM, 0),
                                    (uint8_t **)&memory_desc,
                                    (uint32_t *)sizeof(memory_desc_t));
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    /* Read the non-volatile falg and check for the pattern which was updated in the previous ran */
    status = val->nvram_read(memory_desc->start, TBSA_NVRAM_OFFSET(NV_WDOG), &boot, sizeof(boot_t));
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        return;
    }

    if (boot.state == WARM_BOOT_REQUESTED) {
        /* There was watchdog reset previously */
        val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
        setup_ns_env();
    } else {
        /*
         * Ensure there exist at least one trusted watchdog timer
         */
        /* Get total number of peripherals available */
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, 0, 0),
                                        (uint8_t **)&soc_per,
                                        (uint32_t *)sizeof(soc_peripheral_hdr_t));
        if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
            return;
        }

        while (per_num < soc_per->num) {
            status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, SOC_PERIPHERAL_WATCHDOG, instance),
                                            (uint8_t **)&soc_per_desc,
                                            (uint32_t *)sizeof(soc_peripheral_desc_t));
            if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
                return;
            }

            if (soc_per_desc->attribute != SECURE_PROGRAMMABLE) {
                per_num++;
                continue;
            }

            /* So we found at least one secure watchdog timer */
            trusted_wd_timer_found = TRUE;

            /*
             * After a system restart, 'trusted watchdog timers' must be started automatically
             */
            /* Check whether trusted watchdog timer is enabled ? */
            status = val->is_wd_timer_enabled(soc_per_desc->base);
            if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
                return;
            }
            /* check for all the available trusted watchdog timers */
            instance++;
        }

        if (!trusted_wd_timer_found) {
            /* We did not find any trusted watchdog */
            val->err_check_set(TEST_CHECKPOINT_6, status);
            val->set_status(RESULT_FAIL(status));
            return;
        }

        /*
         * - Ensure trusted watchdog timer modification from trusted access
         * - A Trusted watchdog timer must be able to trigger a reset of the SoC
         * - A Trusted watchdog timer must implement a flag that indicates the occurance of a
         *   timeout event that causes a Warm reset, to allow post reset software to distinguish
         *   this from a powerup cold boot.
         */

        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_CLOCKS, CLOCKS_SYS_FREQ, 0),
                                        (uint8_t **)&clocks_desc,
                                        (uint32_t *)sizeof(clocks_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
            return;
        }

        per_num  = 0;
        instance = 0;
        while (per_num < soc_per->num) {
            boot.state = WARM_BOOT_REQUESTED;
            status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_WDOG), &boot, sizeof(boot_t));
            if (val->err_check_set(TEST_CHECKPOINT_8, status)) {
                return;
            }

            status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, SOC_PERIPHERAL_WATCHDOG, instance),
                                            (uint8_t **)&soc_per_desc,
                                            (uint32_t *)sizeof(soc_peripheral_desc_t));

            if (soc_per_desc->attribute != SECURE_PROGRAMMABLE) {
                per_num++;
                continue;
            }

            /* Initialise watchdog */
            status = val->wd_timer_init(soc_per_desc->base, WDOG_TIMEOUT_VALUE_IN_US, ((clocks_desc->sys_freq)/1000000));

            /* Enable watchdog Timer */
            status = val->wd_timer_enable(soc_per_desc->base);

            /* Expecting watchdog timeout which triggers reset of the system but wait here till guard timeout occurs */
            while(timeout--);

            /* If we reached here means, the watchdog reset did not happen as expected so fail the test */
            val->set_status(RESULT_FAIL(status));
            return;
        }
    }
}

void
exit_hook(tbsa_val_api_t *val)
{
}
