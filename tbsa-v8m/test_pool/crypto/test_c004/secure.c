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
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_CRYPTO_BASE, 4),
                  CREATE_TEST_TITLE("Static key must be in an immutable structure"),
                  CREATE_REF_TAG("R070_TBSA_KEY"),
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

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t status;
    uint32_t      data, expected_fuse_type, i;
    uint32_t      key[32];
    uint32_t      rd_data[32], wr_data[32] = {0xFFFFFFFF};
    key_desc_t    *key_info_static;

    g_val = val;
    status = val->crypto_set_base_addr(SECURE_PROGRAMMABLE);
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    status = val->crypto_get_key_info(&key_info_static, STATIC, 0);
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        return;
    }

    expected_fuse_type = FUSE_BULK | FUSE_LOCKABLE;

    if ((key_info_static->type & expected_fuse_type) != expected_fuse_type) {
        val->print(PRINT_ERROR, "\n        Fuse type in which static key stored is non-compliant", 0);
        val->print(PRINT_ERROR, "\n        Fuse type %x", key_info_static->type);
        val->print(PRINT_ERROR, "\n        Expected Fuse type %x", expected_fuse_type);
        val->err_check_set(TEST_CHECKPOINT_3, TBSA_STATUS_ERROR);
        return;
    }

    if ((key_info_static->state & FUSE_OPEN) == FUSE_OPEN) {
        status = val->fuse_ops(FUSE_READ, key_info_static->addr, key, key_info_static->size);
        if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
            return;
        }

        data = 0;
        /* Check that the static key is non-zero*/
        for(i = 0; i < key_info_static->size; i++)
            data += key[i];

        if (!data) {
            val->print(PRINT_ERROR, "\n        Incorrect Static Key", 0);
            val->err_check_set(TEST_CHECKPOINT_5, TBSA_STATUS_ERROR);
            return;
        }

        status = val->fuse_ops(FUSE_WRITE, key_info_static->addr, wr_data, key_info_static->size);
        if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
            return;
        }

        status = val->fuse_ops(FUSE_READ, key_info_static->addr, rd_data, key_info_static->size);
        if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
            return;
        }

        for (i=0; i<key_info_static->size; i++) {
            if (key[i] != rd_data[i]) {
                val->print(PRINT_ERROR, "\n        Able to modify static key", 0);
                val->err_check_set(TEST_CHECKPOINT_8, TBSA_STATUS_ERROR);
                return;
            }
        }
    } else {
       val->print(PRINT_INFO, "\n        Static key is not open", 0);
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
}
