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

uint32_t              shcsr;

void entry_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;

    tbsa_test_init_t init = {
                             .bss_start      = &__tbsa_test_bss_start__,
                             .bss_end        = &__tbsa_test_bss_end__
                            };

    val->test_initialize(&init);

    /* Disabling SecureFault, UsageFault, BusFault, MemFault temporarily */
    status = val->mem_reg_read(SHCSR, &shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_8, status)) {
        return;
    }

    status = val->mem_reg_write(SHCSR, (shcsr & ~0xF0000));
    if (val->err_check_set(TEST_CHECKPOINT_9, status)) {
        return;
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t status;
    uint32_t      i;
    uint32_t      key[32] = {0}, key_ns[32] = {0};
    key_desc_t    *key_info_huk;

    status = val->crypto_set_base_addr(SECURE_PROGRAMMABLE);
    if (val->err_check_set(TEST_CHECKPOINT_A, status)) {
        return;
    }

    status = val->crypto_get_key_info((key_desc_t **)&key_info_huk, HUK, 0);
    if (val->err_check_set(TEST_CHECKPOINT_B, status)) {
        return;
    }

    if ((key_info_huk->state & FUSE_OPEN) == FUSE_OPEN) {
        status = val->fuse_ops(FUSE_READ, key_info_huk->addr, key, key_info_huk->size);
        if (val->err_check_set(TEST_CHECKPOINT_C, status)) {
            return;
        }

        status = val_crypto_set_base_addr(SECURE_PROGRAMMABLE);
        if (val->err_check_set(TEST_CHECKPOINT_D, status)) {
            return;
        }

       /* Setup secure fault handler is setup, in secure version of the test to recover from
           exception caused by accessing Trusted only accessible key.*/
        status = val_fuse_ops(FUSE_READ, key_info_huk->addr, key_ns, key_info_huk->size);
        if (val->err_check_set(TEST_CHECKPOINT_E, status)) {
            return;
        }

        for(i = 0; i < key_info_huk->size; i++) {
            if (key[i] == key_ns[i]) {
                val->print(PRINT_ERROR, "\n        HUK was accessible by non-Trusted code", 0);
                val->err_check_set(TEST_CHECKPOINT_F, TBSA_STATUS_ERROR);
                return;
            }
        }
    } else {
        val->print(PRINT_INFO, "\n        HUK is not open", 0);
        val->set_status(RESULT_SKIP(1));
        return;
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;

    /* Restoring faults */
    status = val->mem_reg_write(SHCSR, shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_10, status)) {
        return;
    }

    val->interrupt_restore_handler(EXCP_NUM_HF);
}

