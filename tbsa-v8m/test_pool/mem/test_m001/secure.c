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
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_SECURE_RAM_BASE, 1),
                  CREATE_TEST_TITLE("Secure RAM access from Trusted world only"),
                  CREATE_REF_TAG("R160/R170/R180_TBSA_INFRA"),
                  entry_hook,
                  test_payload,
                  exit_hook);

tbsa_val_api_t *g_val;
bool_t         secure_range_found = FALSE;

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

    /* Clear the status register for the next fault */
    g_val->mem_reg_write(SFSR, 0xFF);
    g_val->mem_reg_write(SFAR, 0x0);

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
                             .bss_end        = &__tbsa_test_bss_end__,
                            };

    val->test_initialize(&init);

    g_val = val;
    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void
test_payload(tbsa_val_api_t *val)
{
    uint32_t                data=0, mem_num=0, instance=0, prot_unit_num=0;
    memory_hdr_t            *mem;
    memory_desc_t           *mem_desc;
    protection_units_hdr_t  *prot_units;
    protection_units_desc_t *prot_unit_desc;
    tbsa_status_t           status;
    addr_t                  addr;
    bool_t                  mpc_found = FALSE;

    secure_range_found = FALSE;

    /* Read all the memory ranges given to us and ensure at least 1 range is secure */
    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, 0, 0),
              (uint8_t **)&mem, (uint32_t *)sizeof(memory_hdr_t));
    if (val->err_check_set(TEST_CHECKPOINT_1, status))
        return;

    /* SRAM */
    while (mem_num < mem->num) {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_SRAM, instance),
                                        (uint8_t **)&mem_desc, (uint32_t *)sizeof(memory_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_2, status))
            return;

        if (mem_desc->attribute == MEM_SECURE) {
            addr               = mem_desc->end;
            secure_range_found = TRUE;
            break;
        }
        instance++;
        mem_num++;
    }

    /* FLASH */
    mem_num  = 0;
    instance = 0;
    while (mem_num < mem->num) {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_FLASH, instance),
                                        (uint8_t **)&mem_desc, (uint32_t *)sizeof(memory_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_3, status))
            return;

        if (mem_desc->attribute == MEM_SECURE) {
            addr               = mem_desc->end;
            secure_range_found = TRUE;
            break;
        }

        instance++;
        mem_num++;
    }
    (void)addr;

    if (secure_range_found != TRUE) {
        val->err_check_set(TEST_CHECKPOINT_4, TBSA_STATUS_NOT_FOUND);
        return;
    }

    /* Get MPC configuration */
    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_PROTECTION_UNITS, 0, 0),
									(uint8_t **)&prot_units, (uint32_t *)sizeof(protection_units_hdr_t));
    if (val->err_check_set(TEST_CHECKPOINT_5, status))
        return;

    instance = 0;
    while (prot_unit_num < prot_units->num) {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_PROTECTION_UNITS, PROTECTION_UNITS_MPC, instance),
                                        (uint8_t **)&prot_unit_desc, (uint32_t *)sizeof(protection_units_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_6, status))
            return;

        if (prot_unit_desc->attribute == SECURE_PROGRAMMABLE) {

            mpc_found = TRUE;

			status = val->mpc_configure_security_attribute(0,prot_unit_desc->start,prot_unit_desc->end, MEM_NONSECURE);
            if (val->err_check_set(TEST_CHECKPOINT_7, status))
                return;

			val->mem_write(((uint32_t*) (prot_unit_desc->start)), WORD, 0x12345678);

			status = val->mem_read(((uint32_t*) (prot_unit_desc->start)), WORD,&data);
            if (val->err_check_set(TEST_CHECKPOINT_8, status))
                return;

			if (data != 0x12345678) {
				val->err_check_set(TEST_CHECKPOINT_9,TBSA_STATUS_INCORRECT_VALUE);
                break;
			}
			setup_ns_env();

        }
        /* Revert back to original security state */
        if (mpc_found) {
			status = val->mpc_configure_security_attribute(0,prot_unit_desc->start,prot_unit_desc->end, MEM_SECURE);
            if (val->err_check_set(TEST_CHECKPOINT_A, status))
                return;
        }
        prot_unit_num++;
        instance++;
    }


    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void
exit_hook(tbsa_val_api_t *val)
{
}
