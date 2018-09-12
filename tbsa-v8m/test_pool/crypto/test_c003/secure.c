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

/*  Publish these functions to the external world as associated to this test ID */
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_CRYPTO_BASE, 3),
                  CREATE_TEST_TITLE("HUK should be in Confidential-Lockable-Bulk fuses, accessible only to TW"),
                  CREATE_REF_TAG("R220/R240_TBSA_KEY"),
                  entry_hook,
                  test_payload,
                  exit_hook);


tbsa_val_api_t *g_val;

void entry_hook(tbsa_val_api_t *val)
{
    tbsa_test_init_t init = {
                             .bss_start      = &__tbsa_test_bss_start__,
                             .bss_end        = &__tbsa_test_bss_end__
                            };

    val->test_initialize(&init);

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void hard_fault_esr (unsigned long *sf_args)
{
    g_val->print(PRINT_INFO, "\nSecureFault triggered when HUK was accessed from"
                 "non-Trusted world", 0);

    /* Updating the return address in the stack frame in order to avoid periodic fault */
    sf_args[6] = sf_args[6] + 4;
}

__attribute__((naked))
void HF_Handler(void)
{
    asm volatile("mrs r0, control_ns \n"
                 "mov r1, #0x2       \n"
                 "and r0, r1         \n"
                 "cmp r0, r1         \n"
                 "beq _psp_ns        \n"
                 "mrs r0, msp_ns     \n"
                 "b hard_fault_esr \n"
                 "_psp_ns:           \n"
                 "mrs r0, psp_ns     \n"
                 "b hard_fault_esr \n");
}

tbsa_status_t setup_ns_env(void)
{
    return g_val->interrupt_setup_handler(EXCP_NUM_HF, 0, HF_Handler);
}

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t status;
    uint32_t      data, expected_fuse_type, i;
    uint32_t      key[32];
    key_desc_t    *key_info_huk;

    g_val = val;
    status = val->crypto_set_base_addr(SECURE_PROGRAMMABLE);
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    status = val->crypto_get_key_info((key_desc_t **)&key_info_huk, HUK, 0);
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        return;
    }

    expected_fuse_type = FUSE_CONFIDENTIAL | FUSE_BULK | FUSE_LOCKABLE;

    if ((key_info_huk->type & expected_fuse_type) != expected_fuse_type) {
        val->print(PRINT_ERROR, "\n        Fuse type in which HUK stored is non-compliant", 0);
        val->print(PRINT_ERROR, "\n        Fuse type %x", key_info_huk->type);
        val->print(PRINT_ERROR, "\n        Expected Fuse type %x", expected_fuse_type);
        val->err_check_set(TEST_CHECKPOINT_4, TBSA_STATUS_ERROR);
        return;
    }

    if ((key_info_huk->state & FUSE_OPEN) == FUSE_OPEN) {
        status = val->fuse_ops(FUSE_READ, key_info_huk->addr, key, key_info_huk->size);
        if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
            return;
        }

        data = 0;
        /* Check that if HUK is non-zero*/
        for(i = 0; i < key_info_huk->size; i++)
            data += key[i];

        if (!data) {
            val->print(PRINT_ERROR, "\n        Incorrect HUK", 0);
            val->err_check_set(TEST_CHECKPOINT_6, TBSA_STATUS_ERROR);
            return;
        }
    } else {
        val->print(PRINT_ERROR, "\n        HUK is not open", 0);
        val->set_status(RESULT_SKIP(1));
        return;
    }

    status = setup_ns_env();
    if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
        return;
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
}

