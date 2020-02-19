/** @file
 * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
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

#include "val_framework.h"
#include "val_dispatcher.h"
#include "val_interfaces.h"
#include "val_peripherals.h"
#include "val_target.h"

extern val_api_t val_api;
extern psa_api_t psa_api;

/* gloabls */
addr_t          g_test_info_addr;
uint32_t        combine_test_binary_in_ram;
addr_t          combine_test_binary_addr;

#if !defined(TEST_COMBINE_ARCHIVE)
static const unsigned char elf_magic_header[ELF_IDENT] = {
    /* 0x7f, 'E', 'L', 'F' */
    0x7f, 0x45, 0x4c, 0x46,
    /* Only 32-bit objects  */
    0x01,
    /* Only LSB data. */
    0x01,
    /* Only ELF version 1. */
    0x01,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0
};

/**
    @brief    - This API will copy the length of data from addr to *data
    @param    - addr : address to be read
                data pointer : address to which data will be copied
                len  : length of data to be copy in bytes
    @return   - error status
**/
val_status_t val_mem_copy(addr_t addr, uint8_t *data, uint32_t len)
{
    if (combine_test_binary_in_ram)
    {
        memcpy((void*)data, (void *)addr, len);
        return VAL_STATUS_SUCCESS;
    }
    else
    {
        return val_spi_read(addr, data, len);
    }
}


/**
    @brief    - This function parses ELF header, entry address(test info addreess)
                and program headers. Copies the loadable segments to system memory.
    @return   - Returns Success/Failure
**/
int val_copy_elf(uint32_t saddr, uint32_t *info_addr)
{
    elf_header_t     test_elfh;
    elf_pheader_t    test_ph;
    int              i;

    if (0 != val_mem_copy(saddr, (uint8_t *)&test_elfh, sizeof(elf_header_t)))
    {
        val_print(PRINT_ERROR, "Error: read failure for Test ELF header\n", 0);
        return -1;
    }

    /* validate ELF header */
    if (0 != memcmp(&test_elfh.e_ident, &elf_magic_header, ELF_IDENT))
    {
        val_print(PRINT_ERROR, "Fail: Test ELF header validation\n", 0);
        return -1;
    }

    for (i = 0; i < test_elfh.e_phnum; i++)
    {
        /* Read the program header */
        if (0 != val_mem_copy((saddr + test_elfh.e_phoff + (sizeof(elf_pheader_t)*i)),
                            (uint8_t *)&test_ph, sizeof(elf_pheader_t)))
        {
            val_print(PRINT_ERROR, "Error: reading Test program header\n", 0);
            return -1;
        }

        /* Load the program to physical RAM */
        if (0 != val_mem_copy((saddr + test_ph.p_offset),
                            (uint8_t *)test_ph.p_paddr, test_ph.p_filesz))
        {
            val_print(PRINT_ERROR, "Error: reading Test program header\n", 0);
            return -1;
        }
    }

    *info_addr = test_elfh.e_entry;
    return 0;
}
#endif

/**
    @brief        - This function reads the test ELFs from RAM or secondary storage and loads into
                    system memory
    @param        - test_id      : Returns the current test ID
                  - test_id_prev : Previous test ID.
    @return       - Error code
**/
val_status_t val_test_load(test_id_t *test_id, test_id_t test_id_prev)
{
#if !defined(TEST_COMBINE_ARCHIVE)
    test_header_t   test_header;
    addr_t          flash_addr = combine_test_binary_addr;

    /*
     * The combined Test ELF binary:
     *
     * ----------------------
     * | Custom Test Header*|
     * |--------------------|
     * | Test-1 Image       |
     * ----------------------
     * | Custom Test Header*|
     * ----------------------
     * | Test-2 Image       |
     * |--------------------|
     * | Custom Test Header*|
     *          :
     *          :
     * ----------------------
     * | END Marker         |
     * ----------------------
     *
     */

    if (test_id_prev != VAL_INVALID_TEST_ID)
    {
        /* Jump to last test run + 1 */
        do
        {
            if (val_mem_copy(flash_addr, (uint8_t *)&test_header, sizeof(test_header_t)))
            {
                val_print(PRINT_ERROR, "Error: reading Test program header\n", 0);
                return VAL_STATUS_LOAD_ERROR;
            }

            if (test_header.start_marker == VAL_TEST_END_MARKER)
            {
                val_print(PRINT_DEBUG, "\n\nNo more valid tests found. Exiting..", 0);
                *test_id = VAL_INVALID_TEST_ID;
                return VAL_STATUS_SUCCESS;
            }

            if (test_header.start_marker != VAL_TEST_START_MARKER)
            {
                flash_addr += 0x4;
                continue;
            }

            if ((test_header.start_marker == VAL_TEST_START_MARKER)
                    && (test_header.test_id == test_id_prev))
            {
                flash_addr += (sizeof(test_header_t) + test_header.elf_size);
                break;
            }

            flash_addr += (sizeof(test_header_t) + test_header.elf_size);
        } while(1);
    }

    if (val_mem_copy(flash_addr, (uint8_t *)&test_header, sizeof(test_header_t)))
    {
        val_print(PRINT_ERROR, "\n\nError: reading custom Test header", 0);
        return VAL_STATUS_LOAD_ERROR;
    }

    if (test_header.start_marker == VAL_TEST_END_MARKER)
    {
        val_print(PRINT_DEBUG, "\n\nNo more valid tests found. Exiting.", 0);
        *test_id = VAL_INVALID_TEST_ID;
        return VAL_STATUS_SUCCESS;
    }

    if (test_header.start_marker != VAL_TEST_START_MARKER)
    {
        val_print(PRINT_ERROR, "\n\nError: No valid test binary found. Exiting.", 0);
        *test_id = VAL_INVALID_TEST_ID;
        return VAL_STATUS_LOAD_ERROR;
    }

    flash_addr += sizeof(test_header_t);
    if (val_copy_elf(flash_addr, &g_test_info_addr))
    {
        val_print(PRINT_ERROR, "Error: loading Test program\n", 0);
        return VAL_STATUS_LOAD_ERROR;
    }

    *test_id = test_header.test_id;
    return VAL_STATUS_SUCCESS;

#else /* TEST_COMBINE_ARCHIVE */

    int             i;
    val_test_info_t test_list[] = {
#include "test_entry_list.inc"
                                  {VAL_INVALID_TEST_ID, NULL}
                                  };

    for (i = 0; i < (sizeof(test_list)/sizeof(test_list[0])); i++)
    {
        if (test_id_prev == VAL_INVALID_TEST_ID)
        {
            *test_id = test_list[i].test_id;
            g_test_info_addr = (addr_t) test_list[i].entry_addr;
            return VAL_STATUS_SUCCESS;
        }
        else if (test_id_prev == test_list[i].test_id)
        {
            *test_id = test_list[i+1].test_id;
            g_test_info_addr = (addr_t) test_list[i+1].entry_addr;
            return VAL_STATUS_SUCCESS;
        }
        else if (test_list[i].test_id == VAL_INVALID_TEST_ID)
        {
            val_print(PRINT_DEBUG, "\n\nNo more valid tests found. Exiting.", 0);
            *test_id = VAL_INVALID_TEST_ID;
            return VAL_STATUS_SUCCESS;
        }
    }

    *test_id = VAL_INVALID_TEST_ID;
    val_print(PRINT_ERROR, "\n\nError: No more valid tests found. Exiting.", 0);
    return VAL_STATUS_LOAD_ERROR;
#endif /* TEST_COMBINE_ARCHIVE */
}

/**
    @brief    - This function reads the function pointer addresses for
                test_entry
    @param    - paddr    : Returns the Test function address
    @return   - Returns val_status_t
**/
val_status_t val_get_test_entry_addr(addr_t *paddr)
{
#if !defined(TEST_COMBINE_ARCHIVE)
    *paddr = (addr_t)(((val_test_info_t *)g_test_info_addr)->entry_addr);
#else
    *paddr = g_test_info_addr;
#endif
    return VAL_STATUS_SUCCESS;
}

/**
    @brief    - Execute the function pointer which was given to us by the test
    @param    - void
**/
void val_execute_test_fn(void)
{
    test_fptr_t   fn_ptr;
    addr_t        addr;

    val_get_test_entry_addr(&addr);
    fn_ptr = (test_fptr_t)addr;
    fn_ptr(&val_api, &psa_api);
    return;
}

/*
    @brief    - Reads the pre-defined component name against given test_id
    @param    - test_id  : Current Test ID
    @return   - Component name
*/
char * val_get_comp_name(test_id_t test_id)
{
    switch (VAL_GET_COMP_NUM(test_id))
    {
        case VAL_FF_BASE:
            return "IPC Suite";
        case VAL_CRYPTO_BASE:
            return "Crypto Suite";
        case VAL_PROTECTED_STORAGE_BASE:
            return "Protected Storage Suite";
        case VAL_INTERNAL_TRUSTED_STORAGE_BASE:
            return "Internal Trusted Storage Suite";
        case VAL_INITIAL_ATTESTATION_BASE:
            return "Attestation Suite";
        default:
            return "Unknown Suite";
    }
}

/**
    @brief    - This function is responsible for setting up VAL infrastructure.
                Loads test one by one from combine binary and calls test_entry
                function of each test image.
    @return   - 0 if success Or error code for the failure.
**/
int32_t val_dispatcher(test_id_t test_id_prev)
{

    test_id_t            test_id;
    val_status_t         status;
    miscellaneous_desc_t *misc_desc;
    boot_t               boot;
    test_count_t         test_count;
    uint32_t             test_result;

    status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MISCELLANEOUS,
                                    MISCELLANEOUS_DUT, 0),
                                   (uint8_t **)&misc_desc,
                                   (uint32_t *)sizeof(miscellaneous_desc_t));
    if (VAL_ERROR(status))
    {
        val_print(PRINT_ERROR, "\n\ttarget config read failed", 0);
        return status;
    }

    combine_test_binary_addr   = misc_desc->ns_start_addr_of_combine_test_binary;
    combine_test_binary_in_ram = misc_desc->combine_test_binary_in_ram;
    do
    {
        status = val_get_boot_flag(&boot.state);
        if (VAL_ERROR(status))
        {
            return status;
        }

        /* Did last run test hang and system re-booted due to watchdog timeout and
           boot.state was set to BOOT_NOT_EXPECTED ? If yes, set the test status
           to SIM ERROR and go to next test. */
        if (boot.state == BOOT_NOT_EXPECTED)
        {
            val_set_status(RESULT_PENDING(VAL_STATUS_ERROR));
            status = val_nvmem_read(VAL_NVMEM_OFFSET(NV_TEST_ID_CURRENT),
                                    &test_id, sizeof(test_id_t));
            if (VAL_ERROR(status))
            {
                val_print(PRINT_ERROR, "\n\tNVMEM read error", 0);
                return status;
            }
        }
        /* Did last run test hang and system reset due to watchdog timeout but
           boot.state was set to BOOT_EXPECTED_BUT_FAILED ? If yes, set the test status
           to FAIL and go to next test. This condition will hit when test was expecting
           re-boot on perticular scenario but it didn't happen and system re-booted due
           to other reason. */
        else if (boot.state == BOOT_EXPECTED_BUT_FAILED)
        {
            val_set_status(RESULT_FAIL(VAL_STATUS_BOOT_EXPECTED_BUT_FAILED));
            status = val_nvmem_read(VAL_NVMEM_OFFSET(NV_TEST_ID_CURRENT),
                                    &test_id, sizeof(test_id_t));
            if (VAL_ERROR(status))
            {
                val_print(PRINT_ERROR, "\n\tNVMEM read error", 0);
                return status;
            }
        }
        else
        {
            status = val_test_load(&test_id, test_id_prev);

            if (VAL_ERROR(status))
            {
                return status;
            }
            else if (test_id == VAL_INVALID_TEST_ID)
            {
                break;
            }

            status = val_nvmem_write(VAL_NVMEM_OFFSET(NV_TEST_ID_CURRENT),
                                     &test_id, sizeof(test_id_t));
            if (VAL_ERROR(status))
            {
                val_print(PRINT_ERROR, "\n\tNVMEM write error", 0);
                return status;
            }

            if (VAL_GET_COMP_NUM(test_id_prev) != VAL_GET_COMP_NUM(test_id))
            {
                val_print(PRINT_ALWAYS, "\nRunning.. ", 0);
                val_print(PRINT_ALWAYS, val_get_comp_name(test_id), 0);
                val_print(PRINT_ALWAYS, "\n******************************************\n", 0);
            }

            if (boot.state == BOOT_UNKNOWN)
            {
                /* Set boot.state to BOOT_NOT_EXPECTED to catch unexpected test hang */
                status = val_set_boot_flag(BOOT_NOT_EXPECTED);
                if (VAL_ERROR(status))
                {
                    return status;
                }
            }
            val_execute_test_fn();
        }

        test_result = val_report_status();

        /* Reset boot.state to UNKNOWN before lunching next test */
        status = val_set_boot_flag(BOOT_UNKNOWN);
        if (VAL_ERROR(status))
        {
            return status;
        }

        /* Prepare suite summary data structure */
        status = val_nvmem_read(VAL_NVMEM_OFFSET(NV_TEST_CNT), &test_count, sizeof(test_count_t));
        if (VAL_ERROR(status))
        {
            val_print(PRINT_ERROR, "\n\tNVMEM read error", 0);
            return status;
        }

        switch (test_result)
        {
            case TEST_PASS:
                test_count.pass_cnt += 1;
                break;
            case TEST_FAIL:
                test_count.fail_cnt += 1;
                break;
            case TEST_SKIP:
                test_count.skip_cnt += 1;
                break;
            case TEST_PENDING:
                test_count.sim_error_cnt += 1;
                break;
        }

        status = val_nvmem_write(VAL_NVMEM_OFFSET(NV_TEST_CNT), &test_count, sizeof(test_count_t));
        if (VAL_ERROR(status))
        {
            val_print(PRINT_ERROR, "\n\tNVMEM write error", 0);
            return status;
        }

        test_id_prev = test_id;
        status = val_nvmem_write(VAL_NVMEM_OFFSET(NV_TEST_ID_PREVIOUS),
                                 &test_id, sizeof(test_id_t));
        if (VAL_ERROR(status))
        {
            val_print(PRINT_ERROR, "\n\tNVMEM write error", 0);
            return status;
        }

   } while(1);

   status = val_nvmem_read(VAL_NVMEM_OFFSET(NV_TEST_CNT), &test_count, sizeof(test_count_t));
   if (VAL_ERROR(status))
   {
       val_print(PRINT_ERROR, "\n\tNVMEM read error", 0);
       return status;
   }

   val_print(PRINT_ALWAYS, "\n************ ", 0);
   val_print(PRINT_ALWAYS, val_get_comp_name(test_id_prev), 0);
   val_print(PRINT_ALWAYS, " Report **********\n", 0);
   val_print(PRINT_ALWAYS, "TOTAL TESTS     : %d\n", test_count.pass_cnt + test_count.fail_cnt
            + test_count.skip_cnt + test_count.sim_error_cnt);
   val_print(PRINT_ALWAYS, "TOTAL PASSED    : %d\n", test_count.pass_cnt);
   val_print(PRINT_ALWAYS, "TOTAL SIM ERROR : %d\n", test_count.sim_error_cnt);
   val_print(PRINT_ALWAYS, "TOTAL FAILED    : %d\n", test_count.fail_cnt);
   val_print(PRINT_ALWAYS, "TOTAL SKIPPED   : %d\n", test_count.skip_cnt);
   val_print(PRINT_ALWAYS, "******************************************\n", 0);

   return (test_count.fail_cnt > 0) ? VAL_STATUS_TEST_FAILED : VAL_STATUS_SUCCESS;
}







