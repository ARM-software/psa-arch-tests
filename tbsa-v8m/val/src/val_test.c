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

#include "val_infra.h"
#include "val_test.h"
#include "val_interfaces.h"
#include "val_peripherals.h"
#include "pal_interfaces.h"

/* gloabls */
addr_t          g_s_test_info_addr;
addr_t          g_ns_test_info_addr;
extern uint32_t g_test_binary_src_addr;
extern uint32_t g_test_binary_in_ram;

/**
    @brief        - This function reads the test ELFs from RAM or secondary storage and loads into
                    system memory
    @param        - test_id      : Returns the current test ID
                  - test_id_prev : Previous test ID.
    @return       - Updates the test_id
**/
tbsa_status_t
val_test_load(test_id_t *test_id, test_id_t test_id_prev)
{
    tbsa_test_header_t test_header;
    addr_t             sflash_addr = g_test_binary_src_addr;

    /*
     * The combined Test ELF binary:
     *
     * ------------------------
     * |TBSA_TEST_START_MARKER|
     * ------------------------
     * | Custom Test Header*  |
     * ------------------------
     * | Secure Image         |
     * |----------------------|
     * | Non-Secure Image     |
     * ------------------------
     * | Custom Test Header*  |
     * ------------------------
     * | Secure Image         |
     * |----------------------|
     * | Non-Secure Image     |
     *          :
     *          :
     * ------------------------
     * | TBSA_TEST_END_MARKER |
     * ------------------------
     *
     */
    if (test_id_prev != TBSA_TEST_INVALID) {
        /* Reading TBSA header */
        do
        {
            if (val_spi_read(sflash_addr, (uint8_t *)&test_header, sizeof(tbsa_test_header_t))) {
                val_print(PRINT_INFO, "\n\nError: reading custom Test header", 0);
                return TBSA_STATUS_LOAD_ERROR;
            }

            if (test_header.start_marker == TBSA_TEST_END_MARKER) {
                val_print(PRINT_ERROR, "\n\nNo more valid tests found. Exiting.", 0);
                *test_id = TBSA_TEST_INVALID;
                return TBSA_STATUS_SUCCESS;
            }

            if (test_header.start_marker != TBSA_TEST_START_MARKER) {
                sflash_addr += 0x4;
                continue;
            }

            if ((test_header.start_marker == TBSA_TEST_START_MARKER) && (test_header.test_id == test_id_prev)) {
                sflash_addr += (sizeof(tbsa_test_header_t) + test_header.s_elf_size + test_header.ns_elf_size);
                break;
            }

            sflash_addr += (sizeof(tbsa_test_header_t) + test_header.s_elf_size + test_header.ns_elf_size);
        }while(1);
    }

    if (val_spi_read(sflash_addr, (uint8_t *)&test_header, sizeof(tbsa_test_header_t))) {
        val_print(PRINT_INFO, "\n\nError: reading custom Test header", 0);
        return TBSA_STATUS_LOAD_ERROR;
    }

    if (test_header.start_marker == TBSA_TEST_END_MARKER) {
        val_print(PRINT_ERROR, "\n\nNo more valid tests found. Exiting.", 0);
        *test_id = TBSA_TEST_INVALID;
        return TBSA_STATUS_SUCCESS;
    }

    if (test_header.start_marker != TBSA_TEST_START_MARKER) {
        val_print(PRINT_ERROR, "\n\nNo valid test binary found. Exiting.", 0);
        *test_id = TBSA_TEST_INVALID;
        return TBSA_STATUS_LOAD_ERROR;
    }

    /* load S image */
    sflash_addr += sizeof(tbsa_test_header_t);
    if (pal_test_load(sflash_addr, &g_s_test_info_addr, g_test_binary_in_ram)) {
        val_print(PRINT_INFO, "\nError: loading Test ELF", 0);
        return TBSA_STATUS_LOAD_ERROR;
    }

    /* load NS image */
    sflash_addr += test_header.s_elf_size;
    if (pal_test_load(sflash_addr, &g_ns_test_info_addr, g_test_binary_in_ram)) {
        val_print(PRINT_INFO, "\nError: loading Test ELF", 0);
        return TBSA_STATUS_LOAD_ERROR;
    }

    *test_id = test_header.test_id;

    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - This function reads the function pointer addresses for
                entry/payload/exit test functions
    @param    - test_id  : Current test ID
              - info_type: Test function type
              - paddr    : Returns the Test function address
    @return   - Returns tbsa_status_t
**/
tbsa_status_t
val_test_get_info(test_id_t test_id, tbsa_test_fn_type_t info_type, addr_t *paddr)
{
    switch (info_type)
    {
        case ENTRY_FUNC_HOOK_S:
            *paddr = (addr_t)(((tbsa_test_info_t *)g_s_test_info_addr)->test_fun[0]);
            break;

        case PAYLOAD_FUNC_S:
            *paddr = (addr_t)(((tbsa_test_info_t *)g_s_test_info_addr)->test_fun[1]);
            break;

        case EXIT_FUNC_HOOK_S:
            *paddr = (addr_t)(((tbsa_test_info_t *)g_s_test_info_addr)->test_fun[2]);
            break;

        case ENTRY_FUNC_HOOK_NS:
            *paddr = (addr_t)(((tbsa_test_info_t *)g_ns_test_info_addr)->test_fun[0]);
            break;

        case PAYLOAD_FUNC_NS:
            *paddr = (addr_t)(((tbsa_test_info_t *)g_ns_test_info_addr)->test_fun[1]);
            break;

        case EXIT_FUNC_HOOK_NS:
            *paddr = (addr_t)(((tbsa_test_info_t *)g_ns_test_info_addr)->test_fun[2]);
            break;
    }
    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Initialize bss for a given test
    @param    - init     : Start and end labels for bss section
    @return   - tbsa_status_t
**/
tbsa_status_t
val_test_initialize(tbsa_test_init_t *init)
{
    /* zero bss section */
    memset(init->bss_start, 0, TBSA_REGION_SIZE(init->bss_start, init->bss_end));

    return TBSA_STATUS_SUCCESS;
}
