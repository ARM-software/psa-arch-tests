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
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_CRYPTO_BASE, 5),
                  CREATE_TEST_TITLE("A fuse should only transit to programmed state"),
                  CREATE_REF_TAG("R020_TBSA_FUSE"),
                  entry_hook,
                  test_payload,
                  exit_hook);


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
    tbsa_status_t   status;
    fuse_desc_t     *fuse_desc;
    uint32_t        rd_data1 = 0, rd_data2 = 0;
    uint32_t        wr_data1 = 0x0000FFFF, wr_data2 = 0x0000F0F0;

    status = val->crypto_set_base_addr(SECURE_PROGRAMMABLE);
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    status = val->get_fuse_info((fuse_desc_t **)&fuse_desc, FUSE_OPEN, 0);
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        return;
    }

    status = val->fuse_ops(FUSE_WRITE, fuse_desc->addr, &wr_data1, fuse_desc->size);
    if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
        return;
    }

    status = val->fuse_ops(FUSE_READ, fuse_desc->addr, &rd_data1, fuse_desc->size);
    if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
        return;
    }

    if (rd_data1 != wr_data1)
    {
        val->err_check_set(TEST_CHECKPOINT_5, TBSA_STATUS_WRITE_ERROR);
        return;
    }

    status = val->fuse_ops(FUSE_WRITE, fuse_desc->addr, &wr_data2, fuse_desc->size);
    if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
        return;
    }

    status = val->fuse_ops(FUSE_READ, fuse_desc->addr, &rd_data2, fuse_desc->size);
    if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
        return;
    }

    if (rd_data2 != rd_data1)
    {
        val->print(PRINT_ERROR, "\n        Able to modify programmable bits", 0);
        val->err_check_set(TEST_CHECKPOINT_8, TBSA_STATUS_ERROR);
        return;
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));

}

void exit_hook(tbsa_val_api_t *val)
{
}

