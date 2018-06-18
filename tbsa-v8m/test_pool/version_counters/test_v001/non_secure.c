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
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_VERSION_COUNTERS_BASE, 1),
                  CREATE_TEST_TITLE("Check version counter functionality"),
                  CREATE_REF_TAG("R010/R020/R030/R040/R050/R060_TBSA_COUNT"),
                  entry_hook,
                  test_payload,
                  exit_hook);

memory_desc_t         *memory_desc;
miscellaneous_hdr_t   *misc;
miscellaneous_desc_t  *misc_desc;

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
    uint32_t      instance = 0;
    uint32_t      fw_ver_count;
    boot_t        boot;

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_NVRAM, 0),
                                    (uint8_t **)&memory_desc,
                                    (uint32_t *)sizeof(memory_desc_t));
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        goto cleanup;
    }

    status = val->nvram_read(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
    if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
        goto cleanup;
    }

    if (boot.cb != COLD_BOOT_REQUESTED) {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MISCELLANEOUS, MISCELLANEOUS_VER_COUNT, instance),
                                        (uint8_t **)&misc_desc,
                                        (uint32_t *)sizeof(miscellaneous_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
            goto cleanup;
        }

        boot.cb = COLD_BOOT_REQUESTED;
        status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
        if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
            goto cleanup;
        }
        val->system_reset(COLD_RESET);
        /* shouldn't come here */
        if (val->err_check_set(TEST_CHECKPOINT_6, TBSA_STATUS_ERROR)) {
            goto cleanup;
        }
    } else if (boot.cb == COLD_BOOT_REQUESTED) {
        boot.cb = BOOT_UNKNOWN;
        status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
        if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
            goto cleanup;
        }
    }

    while (TRUE) {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MISCELLANEOUS, MISCELLANEOUS_VER_COUNT, instance),
                                        (uint8_t **)&misc_desc,
                                        (uint32_t *)sizeof(miscellaneous_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_8, status)) {
            goto cleanup;
        }

        if ((misc_desc->fw_ver_type == TRUSTED) || (misc_desc->fw_ver_type == NON_TRUSTED)) {
            if (misc_desc->fw_ver_type == TRUSTED) {
                if (misc_desc->fw_ver_cnt_max < 63) {
                    val->print(PRINT_DEBUG, "\nTrusted firmware version counter max should be >= %d", 63);
                    val->err_check_set(TEST_CHECKPOINT_9, TBSA_STATUS_INCORRECT_VALUE);
                    goto cleanup;
                }
            } else {
                if (misc_desc->fw_ver_cnt_max < 255) {
                    val->print(PRINT_DEBUG, "\nNon-trusted firmware version counter max should be >= %d", 255);
                    val->err_check_set(TEST_CHECKPOINT_A, TBSA_STATUS_INCORRECT_VALUE);
                    goto cleanup;
                }
            }

            /* Reading the current firmware version counter */
            fw_ver_count = val->firmware_version_read(instance);

            /* Updating the firmware version count(non-trusted mode), secure-fault is expected! */
            val->set_status(RESULT_PENDING(status));
            val_firmware_version_update(instance, fw_ver_count);
            while (IS_TEST_PENDING(val->get_status()));


            /* Updating the firmware version count(trsuted mode) */
            status = val->firmware_version_update(instance, ++fw_ver_count);
            if (val->err_check_set(TEST_CHECKPOINT_B, status)) {
                goto cleanup;
            }

            /* Updating firmware version(lower than previous counter), expecting failure ! */
            status = val->firmware_version_update(instance, --fw_ver_count);
            if (status == TBSA_STATUS_SUCCESS) {
                val->err_check_set(TEST_CHECKPOINT_C, TBSA_STATUS_INVALID);
                goto cleanup;
            }

            /* which confirms we cannot decrement firmware version counter */

            /* When a version counter reaches its maximum value, it must not roll over,
             * and no further changes must be possible
             */
            status = val->firmware_version_update(instance, misc_desc->fw_ver_cnt_max);
            if (val->err_check_set(TEST_CHECKPOINT_D, TBSA_STATUS_INVALID)) {
                goto cleanup;
            }
            status = val->firmware_version_update(instance, (misc_desc->fw_ver_cnt_max + 1));
            if (status == TBSA_STATUS_SUCCESS) {
                val->err_check_set(TEST_CHECKPOINT_E, TBSA_STATUS_INVALID);
                goto cleanup;
            }

            /* Reading trusted firmware version counter */
            if(misc_desc->fw_ver_cnt_max != val->firmware_version_read(instance)) {
                val->err_check_set(TEST_CHECKPOINT_F, TBSA_STATUS_INVALID);
                goto cleanup;
            }
            /* confirms roll over didn't happen */
        }
        /* repeat test for remaining available version counters */
        instance++;
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));

cleanup:
    /* Restoring default Handler */
    status = val->interrupt_restore_handler(EXCP_NUM_SF);
    if (val->err_check_set(TEST_CHECKPOINT_10, status)) {
        return;
    }
}

void exit_hook(tbsa_val_api_t *val)
{
}
