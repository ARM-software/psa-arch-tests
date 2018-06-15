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
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_CRYPTO_BASE, 6),
                  CREATE_TEST_TITLE("A Trusted HK must not be directly accessible by any software."),
                  CREATE_REF_TAG("R140_TBSA_KEY"),
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
    tbsa_status_t status;
    uint32_t      i;
    uint32_t      data, key[32] = {0xdeaddead};
    key_desc_t    *key_info_trust;

    status = val->crypto_set_base_addr(SECURE_PROGRAMMABLE);
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    status = val->crypto_get_key_info((key_desc_t **)&key_info_trust, TRUST, 0);
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        return;
    }

    status = val->fuse_ops(FUSE_READ, key_info_trust->addr, key, key_info_trust->size);
    if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
        return;
    }

    data = 0;
    for(i = 0; i < key_info_trust->size; i++)
        data += key[i];

    /* Check that if Trusted key is zero*/
    if (data) {
        val->print(PRINT_ERROR, "\n        Trusted key is accessible", 0);
        val->err_check_set(TEST_CHECKPOINT_4, TBSA_STATUS_ERROR);
        return;
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
}

