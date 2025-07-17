/** @file
 * Copyright (c) 2018-2025, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 * Copyright 2023 NXP
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

extern val_api_t val_api;
extern psa_api_t psa_api;

/* gloabls */
addr_t          g_test_info_addr;

/**
    @brief        - This function prints PSA_{SUITE}_API_VERSION_MAJOR
                    PSA_{SUITE}_API_VERSION_MINOR details.
    @param        - None
    @return       - None
**/
__attribute__((unused)) static void val_print_api_version(void)
{
#ifdef CRYPTO
    val_print(ALWAYS, " %d.", PSA_CRYPTO_API_VERSION_MAJOR);
    val_print(ALWAYS, "%d", PSA_CRYPTO_API_VERSION_MINOR);
#endif
#ifdef INTERNAL_TRUSTED_STORAGE
    val_print(ALWAYS, " %d.", PSA_ITS_API_VERSION_MAJOR);
    val_print(ALWAYS, "%d", PSA_ITS_API_VERSION_MINOR);
#endif
#ifdef PROTECTED_STORAGE
    val_print(ALWAYS, " %d.", PSA_PS_API_VERSION_MAJOR);
    val_print(ALWAYS, "%d", PSA_PS_API_VERSION_MINOR);
#endif
#ifdef STORAGE
    val_print(ALWAYS, " ITS %d.", PSA_ITS_API_VERSION_MAJOR);
    val_print(ALWAYS, "%d", PSA_ITS_API_VERSION_MINOR);
    val_print(ALWAYS, " and PS %d.", PSA_PS_API_VERSION_MAJOR);
    val_print(ALWAYS, "%d", PSA_PS_API_VERSION_MINOR);
#endif
#ifdef INITIAL_ATTESTATION
    val_print(ALWAYS, " %d.", PSA_INITIAL_ATTEST_API_VERSION_MAJOR);
    val_print(ALWAYS, "%d", PSA_INITIAL_ATTEST_API_VERSION_MINOR);
#endif
}

static val_test_info_t g_test_list[] = {
#include "test_entry_list.inc"
                                  {VAL_INVALID_TEST_ID, NULL}
                              };

const uint32_t total_tests = sizeof(g_test_list)/sizeof(g_test_list[0]) - 1;

/**
    @brief        - This function returns the IDs list of available tests
    @param        - test_id_list : Buffer allocated by caller
                  - size : Size of test_id_list
    @return       - If test_id_list is NULL, the required size of test_id_list
                    else if size is too small -1
                    else the actual size of test_id_list
**/
size_t val_get_test_list(uint32_t *test_id_list, size_t size)
{

    val_test_info_t *test_info = &g_test_list[0];
    size_t test_list_size = sizeof(g_test_list)/sizeof(g_test_list[0]) - 1;

    if (!test_id_list)
        return test_list_size;

    if (size < test_list_size)
        return -1;

    test_list_size = 0;

    for (; test_info->test_id != VAL_INVALID_TEST_ID; test_info++)
    {
        *test_id_list++ = test_info->test_id;
        test_list_size++;
    }

    return test_list_size;
}

/**
    @brief        - This function reads the test ELFs from RAM or secondary storage and loads into
                    system memory
    @param        - test_id      : Returns the current test ID
                  - test_id_prev : Previous test ID.
    @return       - Error code
**/
val_status_t val_test_load(test_id_t *test_id, test_id_t test_id_prev)
{
    val_test_info_t *test_info = &g_test_list[0];

    if (test_id_prev != VAL_INVALID_TEST_ID)
    {
        for (; test_info->test_id != VAL_INVALID_TEST_ID; test_info++)
        {
            if (test_info->test_id == test_id_prev)
            {
                test_info++;
                break;
            }
        }
    }

    for (; test_info->test_id != VAL_INVALID_TEST_ID; test_info++)
    {
        if (pal_is_test_enabled(test_info->test_id))
        {
            *test_id = test_info->test_id;
            g_test_info_addr = (addr_t) test_info->entry_addr;
            return VAL_STATUS_SUCCESS;
        }
    }

    val_print(DBG, "\n\nNo more valid tests found. Exiting.", 0);
    *test_id = VAL_INVALID_TEST_ID;
    return VAL_STATUS_SUCCESS;
}

/**
    @brief    - This function reads the function pointer addresses for
                test_entry
    @param    - paddr    : Returns the Test function address
    @return   - Returns val_status_t
**/
val_status_t val_get_test_entry_addr(addr_t *paddr)
{
    *paddr = g_test_info_addr;
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
char *val_get_comp_name(test_id_t test_id)
{
    switch (VAL_GET_COMP_NUM(test_id))
    {
        case VAL_FF_BASE:
            return "IPC Suite";
        case VAL_CRYPTO_BASE:
            return "Crypto Suite";
        case VAL_STORAGE_BASE:
            return "Storage Suite";
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
    boot_t               boot;
    regre_report_t       test_count;
    uint32_t             test_result;

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
            val_set_status(RESULT_ERROR(VAL_STATUS_ERROR));
            status = val_nvm_read(VAL_NVM_OFFSET(NVM_CUR_TEST_NUM_INDEX),
                                    &test_id, sizeof(test_id_t));
            if (VAL_ERROR(status))
            {
                val_print(ERROR, "\n\tNVMEM read error", 0);
                return status;
            }
        }
        /* Did last run test hang and system reset due to watchdog timeout but
           boot.state was set to BOOT_EXPECTED_BUT_FAILED ? If yes, set the test status
           to FAIL and go to next test. This condition will hit when test was expecting
           re-boot on particular scenario but it didn't happen and system re-booted due
           to other reason. */
        else if (boot.state == BOOT_EXPECTED_BUT_FAILED)
        {
            val_set_status(RESULT_FAIL(VAL_STATUS_BOOT_EXPECTED_BUT_FAILED));
            status = val_nvm_read(VAL_NVM_OFFSET(NVM_CUR_TEST_NUM_INDEX),
                                    &test_id, sizeof(test_id_t));
            if (VAL_ERROR(status))
            {
                val_print(ERROR, "\n\tNVMEM read error", 0);
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

            status = val_nvm_write(VAL_NVM_OFFSET(NVM_CUR_TEST_NUM_INDEX),
                                     &test_id, sizeof(test_id_t));
            if (VAL_ERROR(status))
            {
                val_print(ERROR, "\n\tNVMEM write error", 0);
                return status;
            }

            if (VAL_GET_COMP_NUM(test_id_prev) != VAL_GET_COMP_NUM(test_id))
            {
                val_get_comp_name(test_id);
              //  val_print(ALWAYS, "\nRunning.. ", 0);
                //val_print(ALWAYS, val_get_comp_name(test_id), 0);
			//	val_print_api_version();
#ifdef TESTS_COVERAGE
                val_print(ALWAYS, "\nNOTE : Known failing tests are excluded from this \
build. For PSA functional API certification, all tests must be run.\n", 0);
#endif
                val_print(ALWAYS, "\n******************************************\n", 0);
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

#ifdef BESPOKE_PRINT_NS
        test_result = val_report_status_bespoke();
#else
        test_result = val_report_status();
#endif

        /* Reset boot.state to UNKNOWN before launching next test */
        status = val_set_boot_flag(BOOT_UNKNOWN);
        if (VAL_ERROR(status))
        {
            return status;
        }

        /* Prepare suite summary data structure */
        status = (val_nvm_read(VAL_NVM_OFFSET(NVM_TOTAL_PASS_INDEX),
                        &test_count.total_pass, sizeof(uint32_t)) ||
                  val_nvm_read(VAL_NVM_OFFSET(NVM_TOTAL_FAIL_INDEX),
                        &test_count.total_fail, sizeof(uint32_t))  ||
                  val_nvm_read(VAL_NVM_OFFSET(NVM_TOTAL_SKIP_INDEX),
                        &test_count.total_skip, sizeof(uint32_t))  ||
                  val_nvm_read(VAL_NVM_OFFSET(NVM_TOTAL_ERROR_INDEX),
                        &test_count.total_error, sizeof(uint32_t)));

        if (VAL_ERROR(status))
        {
            val_print(ERROR, "\n\tNVMEM read error", 0);
            return status;
        }

        val_update_regression_report(test_result, &test_count);

        status = (val_nvm_write(VAL_NVM_OFFSET(NVM_TOTAL_PASS_INDEX),
                        &test_count.total_pass, sizeof(uint32_t)) ||
                  val_nvm_write(VAL_NVM_OFFSET(NVM_TOTAL_FAIL_INDEX),
                       &test_count.total_fail, sizeof(uint32_t))  ||
                  val_nvm_write(VAL_NVM_OFFSET(NVM_TOTAL_SKIP_INDEX),
                       &test_count.total_skip, sizeof(uint32_t))  ||
                  val_nvm_write(VAL_NVM_OFFSET(NVM_TOTAL_ERROR_INDEX),
                       &test_count.total_error, sizeof(uint32_t)));

        if (VAL_ERROR(status))
        {
            val_print(ERROR, "\n\tNVMEM write error", 0);
            return status;
        }

        test_id_prev = test_id;
        status = val_nvm_write(VAL_NVM_OFFSET(NVM_PREVIOUS_TEST_ID),
                                 &test_id, sizeof(test_id_t));
        if (VAL_ERROR(status))
        {
            val_print(ERROR, "\n\tNVMEM write error", 0);
            return status;
        }

   } while (1);

   status = (val_nvm_read(VAL_NVM_OFFSET(NVM_TOTAL_PASS_INDEX),
                    &test_count.total_pass, sizeof(uint32_t)) ||
             val_nvm_read(VAL_NVM_OFFSET(NVM_TOTAL_FAIL_INDEX),
                    &test_count.total_fail, sizeof(uint32_t))  ||
             val_nvm_read(VAL_NVM_OFFSET(NVM_TOTAL_SKIP_INDEX),
                    &test_count.total_skip, sizeof(uint32_t))  ||
             val_nvm_read(VAL_NVM_OFFSET(NVM_TOTAL_ERROR_INDEX),
                    &test_count.total_error, sizeof(uint32_t)));

   if (VAL_ERROR(status))
   {
       val_print(ERROR, "\n\tNVMEM read error", 0);
       return status;
   }

#ifdef BESPOKE_PRINT_NS
   val_print(ALWAYS, "\n************ ", 0);
   val_print(ALWAYS, val_get_comp_name(test_id_prev), 0);
   val_print(ALWAYS, " Report ************\n", 0);
   val_print(ALWAYS, "TOTAL TESTS     : %d\n", (uint32_t)test_count.total_pass +
            test_count.total_fail + test_count.total_skip + test_count.total_error);
   val_print(ALWAYS, "TOTAL PASSED    : %d\n", test_count.total_pass);
   val_print(ALWAYS, "TOTAL SIM ERROR : %d\n", test_count.total_error);
   val_print(ALWAYS, "TOTAL FAILED    : %d\n", test_count.total_fail);
   val_print(ALWAYS, "TOTAL SKIPPED   : %d\n", test_count.total_skip);
   val_print(ALWAYS, "******************************************\n", 0);
#else
    val_print_regression_report(&test_count);
#endif

   return (test_count.total_fail > 0) ? VAL_STATUS_TEST_FAILED : VAL_STATUS_SUCCESS;
}
