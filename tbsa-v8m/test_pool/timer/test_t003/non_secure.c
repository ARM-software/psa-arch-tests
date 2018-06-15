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

/**
  Publish these functions to the external world as associated to this test ID
**/
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_TRUSTED_TIMERS_BASE, 3),
                  CREATE_TEST_TITLE("Trusted and Non-trusted world operation to TRTC"),
                  CREATE_REF_TAG("R130/R140/R150_TBSA_TIME"),
                  entry_hook,
                  test_payload,
                  exit_hook);

soc_peripheral_hdr_t  *soc_per;
soc_peripheral_desc_t *soc_per_desc;

void
entry_hook(tbsa_val_api_t *val)
{
    tbsa_test_init_t init = {
                             .bss_start      = &__tbsa_test_bss_start__,
                             .bss_end        = &__tbsa_test_bss_end__
                            };
    val->test_initialize(&init);
    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    uint32_t      per_num  = 0;
    uint32_t      instance = 0;
    uint32_t      data;
    tbsa_status_t status;

    /* Get total number of peripherals available */
    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, 0, 0),
                                    (uint8_t **)&soc_per,
                                    (uint32_t *)sizeof(soc_peripheral_hdr_t));
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        goto clean_up;
    }

    /* Check we have at least one TRTC */
    while (per_num < soc_per->num) {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, SOC_PERIPHERAL_RTC, instance),
                                        (uint8_t **)&soc_per_desc,
                                        (uint32_t *)sizeof(soc_peripheral_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
            goto clean_up;
        }

        if (soc_per_desc->attribute != SECURE_PROGRAMMABLE) {
            per_num++;
            continue;
        }

        /* Ensure TRTC access from non-trusted world triggers secure fault exception */
        val->set_status(RESULT_PENDING(status));

        /* Trying to read the TRTC base address, expect secure fault? */
        val_mem_read_wide((uint32_t *)(soc_per_desc->base + soc_per_desc->offset), &data);

        /* wait here till pending status is cleared by secure fault */
        while (IS_TEST_PENDING(val->get_status()));

        val->set_status(RESULT_PENDING(status));

        /* Trying to read the clock source base address for a given TRTC, expect secure fault? */
        val_mem_read_wide((uint32_t *)soc_per_desc->clk_src, &data);

        /* wait here till pending status is cleared by secure fault */
        while (IS_TEST_PENDING(val->get_status()));

        /* successfully verified, break now */
        break;
    }
clean_up:
    /* Restoring default Handler */
    status = val->interrupt_restore_handler(EXCP_NUM_SF);
    if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
        return;
    }
}

void exit_hook(tbsa_val_api_t *val)
{
}
