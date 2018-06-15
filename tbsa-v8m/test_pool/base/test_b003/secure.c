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

#define PATTERN1	0x73737373
#define PATTERN2	0x56565656

/**
  Publish these functions to the external world as associated to this test ID
**/
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_BASE_BASE, 3),
                  CREATE_TEST_TITLE("Check scrub of shared storage when reallocated from Trusted to Non-trusted"),
                  CREATE_REF_TAG("R100_TBSA_INFRA"),
                  entry_hook,
                  test_payload,
                  exit_hook);

memory_desc_t *memory_desc;

void entry_hook(tbsa_val_api_t *val)
{
    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t status;
    uint32_t      instance         = 0;
    bool_t        sram_block_found = FALSE;
    uint32_t      sram_num;
    bool_t        sram_num_init    = FALSE;

    do {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_SRAM, instance),
                                        (uint8_t **)&memory_desc,
                                        (uint32_t *)sizeof(memory_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
            return;
        }

        if (!sram_num_init) {
            sram_num = memory_desc->cfg_type.size >> 24;
            sram_num_init = TRUE;
        }
        if (memory_desc->attribute == MEM_CONFIGURABLE) {
            /* We found a configurable SRAM block */
            sram_block_found = TRUE;

            /* Ensure the configurable block is secure */
            if(!val->is_secure_address(memory_desc->start)) {
                /* Configure a given block of memory as secure one */
                status = val->mpc_configure_security_attribute(0, memory_desc->start, memory_desc->end, MEM_SECURE);
                if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
                    return;
                }
            }
            /* Writing a known pattern to trusted shared volatile storage */
            *(uint32_t*)(memory_desc->start)                                 = PATTERN1;
            *(uint32_t*)(memory_desc->end - (memory_desc->end % 0x4) - 0x4 ) = PATTERN2;

            /* Configure given secure block to non-secure */
            status = val->mpc_configure_security_attribute(0, memory_desc->start, memory_desc->end, MEM_NONSECURE);
            if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
                return;
            }

            break;
        } else {
            instance++;
        }
        /* Moving to next instance of SRAM block*/
        sram_num--;
    } while(sram_num);

    if (sram_block_found) {
        val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
    } else {
        val->err_check_set(TEST_CHECKPOINT_4, TBSA_STATUS_NOT_FOUND);
    }
}

void exit_hook(tbsa_val_api_t *val)
{
}
