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

#define TIMEOUT_VALUE    0xFFFFFFFF

/**
  Publish these functions to the external world as associated to this test ID
**/
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_SECURE_RAM_BASE, 1),
                  CREATE_TEST_TITLE("Secure RAM access from Trusted world only"),
                  CREATE_REF_TAG("R160/R170/R180_TBSA_INFRA"),
                  entry_hook,
                  test_payload,
                  exit_hook);

tbsa_val_api_t *g_val;
uint32_t        shcsr;

void entry_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;

    tbsa_test_init_t init = {
                             .bss_start      = &__tbsa_test_bss_start__,
                             .bss_end        = &__tbsa_test_bss_end__,
                            };

    val->test_initialize(&init);

    g_val = val;

    /* Disabling SecureFault, UsageFault, BusFault, MemFault temporarily */
    status = val->mem_reg_read(SHCSR, &shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    status = val->mem_reg_write(SHCSR, (shcsr & ~0xF0000));
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        return;
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    uint32_t                data=0, instance=0, prot_unit_num=0;
    protection_units_hdr_t  *prot_units;
    protection_units_desc_t *prot_unit_desc;
    tbsa_status_t           status;
	uint32_t                timeout = TIMEOUT_VALUE;
    bool_t                  mpc_found = FALSE;
    bool_t                  timeout_flag = FALSE;



    /* Get MPC configuration */
    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_PROTECTION_UNITS, 0, 0),
									(uint8_t **)&prot_units, (uint32_t *)sizeof(protection_units_hdr_t));
    if (val->err_check_set(TEST_CHECKPOINT_3, status))
        return;

    instance = 0;
    while (prot_unit_num < prot_units->num) {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_PROTECTION_UNITS, PROTECTION_UNITS_MPC, instance),
                                        (uint8_t **)&prot_unit_desc, (uint32_t *)sizeof(protection_units_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_4, status))
            return;



        if (prot_unit_desc->attribute == SECURE_PROGRAMMABLE) {

            mpc_found = TRUE;

			val->set_status(RESULT_PENDING(status));

			/* Try to access MPC from Non-Secure */
			val_mem_read_wide((uint32_t *)prot_unit_desc->device_base, &data);

            /* Wait here till pending status is cleared by secure fault or timeout occurs, whichever comes early */
            while (IS_TEST_PENDING(val->get_status()) && (--timeout));

            if(!timeout) {
                val->err_check_set(TEST_CHECKPOINT_5, TBSA_STATUS_TIMEOUT);
                timeout_flag = TRUE;
            }

            /* Restoring default Handler */
            status = val->interrupt_restore_handler(EXCP_NUM_HF);
            if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
                return;
            }

            /* Perform a write and read back to make sure that non-secure access went through correctly */
			val_mem_write(((uint32_t*) (prot_unit_desc->start)), WORD, 0xAABBCCDD);

			status = val_mem_read_wide(((uint32_t*) (prot_unit_desc->start)), &data);

            if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
                return;
            }

			if (data != 0xAABBCCDD) {
				val->err_check_set(TEST_CHECKPOINT_8,TBSA_STATUS_INCORRECT_VALUE);
                return;
			}

        }
        prot_unit_num++;
        instance++;
    }

    if (mpc_found && (timeout_flag != TRUE)) {

        val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
    }

}

void exit_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;

    /* Restoring faults */
    status = val->mem_reg_write(SHCSR, shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_9, status)) {
        return;
    }
}
