/** @file
 * Copyright (c) 2018-2025, Arm Limited or its affiliates. All rights reserved.
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
#include "val_interfaces.h"
#include "val_dispatcher.h"
#include "val_peripherals.h"
#include "pal_interfaces_ns.h"

extern val_api_t val_api;
extern psa_api_t psa_api;

#ifdef IPC
/**
 * @brief Connect to given sid
   @param  -sid : RoT service id
   @param  -version : version of RoT service
   @param  -handle - return connection handle
 * @return val_status_t
 */
val_status_t val_ipc_connect(uint32_t sid, uint32_t version, psa_handle_t *handle)
{
    *handle = psa_connect(sid, version);

    if (*handle > 0)
        return VAL_STATUS_SUCCESS;

    return VAL_STATUS_CONNECTION_FAILED;
}

/**
 * @brief Call a connected Root of Trust Service.@n
 * The caller must provide an array of ::psa_invec_t structures as the input payload.
 *
 * @param  handle   Handle for the connection.
 * @param  type     Request type
 * @param  in_vec   Array of psa_invec structures.
 * @param  in_len   Number of psa_invec structures in in_vec.
 * @param out_vec  Array of psa_outvec structures for optional Root of Trust Service response.
 * @param  out_len  Number of psa_outvec structures in out_vec.
 * @return val_status_t
 */
val_status_t val_ipc_call(psa_handle_t handle,
                          int32_t type,
                          const psa_invec *in_vec,
                          size_t in_len,
                          psa_outvec *out_vec,
                          size_t out_len)
{
    psa_status_t call_status = PSA_SUCCESS;

    call_status = psa_call(handle, type, in_vec, in_len, out_vec, out_len);

    if (call_status != PSA_SUCCESS)
    {
        return VAL_STATUS_CALL_FAILED;
    }

    return VAL_STATUS_SUCCESS;
}

/**
 * @brief Close a connection to a Root of Trust Service.
 * Sends the PSA_IPC_DISCONNECT message to the Root of Trust Service so it can clean up resources.
 *
 * @param handle Handle for the connection.
 * @return void
 */
void val_ipc_close(psa_handle_t handle)
{
    psa_close(handle);
}
#endif

/**
    @brief    - This function executes given list of tests from non-secure sequentially
                This covers non-secure to secure IPC API scenario
    @param    - test_num   : Test_num
    @param    - tests_list : list of tests to be executed
    @param    - server_hs  : Initiate a server handshake
    @return   - val_status_t
**/
val_status_t val_execute_non_secure_tests(uint32_t test_num, const client_test_t *tests_list,
                                          bool_t server_hs)
{
    val_status_t          status = VAL_STATUS_SUCCESS;
    val_status_t          test_status = VAL_STATUS_SUCCESS;
    boot_t                boot;
    uint32_t              i = 1;
#ifdef IPC
    psa_handle_t          handle;
    test_info_ipc_t       test_info;

    test_info.test_num = test_num;
#else
   (void)test_num;
#endif

    status = val_get_boot_flag(&boot.state);
    if (VAL_ERROR(status))
    {
       val_set_status(RESULT_FAIL(status));
       return status;
    }

    if (boot.state == BOOT_NOT_EXPECTED || boot.state == BOOT_EXPECTED_REENTER_TEST
        || boot.state == BOOT_EXPECTED_CONT_TEST_EXEC
        || boot.state == BOOT_EXPECTED_ON_SECOND_CHECK)
    {
        while (tests_list[i] != NULL)
        {
            /*
             * Reboot have been expected by test in previous ns run,
             * consider previous run pass and jump to second test function
             * of the same test if available.
             */

            if ((boot.state ==  BOOT_EXPECTED_REENTER_TEST) && (i == 1))
            {
                val_print(DBG, "[Check 1] PASSED\n", 0);
                i++;
                continue;
            }

            if ((boot.state ==  BOOT_EXPECTED_ON_SECOND_CHECK) && (i == 1))
            {
                val_print(DBG, "[Check 2] PASSED\n", 0);
                i = i + 2 ;
                continue;
            }

            if (boot.state != BOOT_EXPECTED_CONT_TEST_EXEC)
            {
                status = val_set_boot_flag(BOOT_NOT_EXPECTED);
                if (VAL_ERROR(status))
                {
                    return status;
                }
            }

            if (i == 1)
                val_print(TEST, "[Info] Executing tests from non-secure\n", 0);
#ifdef IPC
            if (server_hs == TRUE)
            {
                /* Handshake with server tests */
                test_info.block_num = i;
#if STATELESS_ROT == 1
                status = val_execute_secure_test_func(&handle, test_info,
                                                                SERVER_TEST_DISPATCHER_HANDLE);
                handle = (int32_t)SERVER_TEST_DISPATCHER_HANDLE;
#else
                status = val_execute_secure_test_func(&handle, test_info,
                                                SERVER_TEST_DISPATCHER_SID);
#endif
                if (VAL_ERROR(status))
                {
                    val_set_status(RESULT_FAIL(status));
                    val_print(DBG, "[Check %d] START\n", i);
                    return status;
                }
                else
                {
                    val_print(DBG, "[Check %d] START\n", i);
                }
            }
#endif
            /* keep track of the test block numbers, helps when the panic happened */
            status = val_set_test_data(NVM_TEST_DATA2, i);
            /* Execute client tests */
            test_status = tests_list[i](CALLER_NONSECURE);
#ifdef IPC
            if (server_hs == TRUE)
            {
                /* Retrive Server test status */
                status = val_get_secure_test_result(&handle);
            }
#endif
            status = test_status ? test_status:status;
            if (IS_TEST_SKIP(status))
            {
                val_set_status(status);
                if (server_hs == TRUE)
                    val_print(DBG, "[Check %d] SKIPPED\n", i);
                return status;
            }
            else if (VAL_ERROR(status))
            {
                val_set_status(RESULT_FAIL(status));
                if (server_hs == TRUE)
                    val_print(DBG, "[Check %d] FAILED\n", i);

                return status;
            }
            else
            {
                if (server_hs == TRUE)
                    val_print(DBG, "[Check %d] PASSED\n", i);
            }

            i++;
        }
   }
   else
   {
       /* If we are here means, we are in second run of this test */
       status = VAL_STATUS_SUCCESS;
       if (boot.state != BOOT_EXPECTED_S)
       {
            val_print(DBG, "[Check 1] PASSED\n", 0);
       }
   }
   return status;
}

#ifdef IPC
/**
    @brief    - This function is used to switch to client_partition.c
                where client tests will be executed to cover secure to secure
                IPC scenario.
    @param    - test_num  : Test_num
    @return   - val_status_t
**/
val_status_t val_switch_to_secure_client(uint32_t test_num)
{
    val_status_t          status = VAL_STATUS_SUCCESS;
    boot_t                boot;
    psa_handle_t          handle;
    test_info_ipc_t       test_info;

    test_info.test_num = test_num;
    test_info.block_num = 1;

    status = val_get_boot_flag(&boot.state);
    if (VAL_ERROR(status))
    {
       goto exit;
    }

    if (boot.state != BOOT_EXPECTED_S)
    {
       /*
        * Reboot have been expected by test in previous s run,
        * consider previous run pass and jump to second test function
        * of the same test if available.
        */
       if (boot.state ==  BOOT_EXPECTED_REENTER_TEST)
       {
            test_info.block_num++;
            val_print(DBG, "[Check 1] PASSED\n", 0);
       }

       if (boot.state ==  BOOT_EXPECTED_ON_SECOND_CHECK)
       {
            test_info.block_num = test_info.block_num + 2;
            val_print(DBG, "[Check 2] PASSED\n", 0);
       }
       status = val_set_boot_flag(BOOT_NOT_EXPECTED);
       if (VAL_ERROR(status))
       {
   	   goto exit;
       }

       /* switch to secure client */
   #if STATELESS_ROT == 1
      status = val_execute_secure_test_func(&handle, test_info, CLIENT_TEST_DISPATCHER_HANDLE);
      handle = (int32_t)CLIENT_TEST_DISPATCHER_HANDLE;
   #else
      status = val_execute_secure_test_func(&handle, test_info, CLIENT_TEST_DISPATCHER_SID);
   #endif
      if (VAL_ERROR(status))
      {
   	   goto exit;
      }

      /* Retrive secure client test status */
      status = val_get_secure_test_result(&handle);
      if (IS_TEST_SKIP(status))
      {
        val_set_status(status);
   		return status;
      }
      if (VAL_ERROR(status))
      {
   	   goto exit;
      }
      return status;
    }
    else if (boot.state == BOOT_EXPECTED_S)
    {
	   int32_t test_data = 0;
	   status = val_get_test_data(NVM_TEST_DATA1, &test_data);
	   if (VAL_ERROR(status))
	   {
		   return VAL_STATUS_ERROR;
	   }
	   test_info.block_num = test_data + 1;
	   val_print(DBG, "[Check %d] PASSED\n", test_data);
	   status = val_set_boot_flag(BOOT_NOT_EXPECTED);
	   if (VAL_ERROR(status))
	   {
		   goto exit;
	   }


	   /* switch to secure client */
	#if STATELESS_ROT == 1
	   status = val_execute_secure_test_func(&handle, test_info, CLIENT_TEST_DISPATCHER_HANDLE);
	   handle = (int32_t)CLIENT_TEST_DISPATCHER_HANDLE;
	#else
	   status = val_execute_secure_test_func(&handle, test_info, CLIENT_TEST_DISPATCHER_SID);
	#endif
	   if (VAL_ERROR(status))
	   {
		   goto exit;
	   }

	   /* Retrive secure client test status */
	   status = val_get_secure_test_result(&handle);
	   if (IS_TEST_SKIP(status))
	   {
			val_set_status(status);
			return status;
	   }
	   if (VAL_ERROR(status))
	   {
		   goto exit;
	   }
	   return status;
    }
    else
    {
        /* If we are here means, we are in third run of this test */
        val_print(DBG, "[Check 1] PASSED\n", 0);
        return VAL_STATUS_SUCCESS;
    }

exit:
   val_set_status(RESULT_FAIL(status));
   return status;
}

/**
    @brief    - This function is used to handshake between:
                - nonsecure client fn to server test fn
                - secure client fn and server test fn
                - nonsecure client fn to secure client test fn
    @param    - handle     : handle returned while connecting given sid
    @param    - test_info  : Test_num and block_num to be executed
    @param    - sid        : RoT service to be connected. Partition dispatcher sid
    @return   - val_status_t
**/
val_status_t val_execute_secure_test_func(__attribute__((unused)) psa_handle_t *handle,
		                                           test_info_ipc_t test_info, uint32_t sid)
{
    uint32_t        test_data;
    val_status_t    status = VAL_STATUS_SUCCESS;
    psa_status_t    status_of_call = PSA_SUCCESS;
#if STATELESS_ROT == 1
    test_data = ((uint32_t)(test_info.test_num) | ((uint32_t)(test_info.block_num) << BLOCK_NUM_POS)
                        | ((uint32_t)(TEST_EXECUTE_FUNC) << ACTION_POS));
    psa_invec data[1] = { {&test_data, sizeof(test_data)} };

    status_of_call = psa_call(sid, 0, data, 1, NULL, 0);
    if (status_of_call != PSA_SUCCESS)
    {
    	status = VAL_STATUS_CALL_FAILED;
        val_print(ERROR, "Call to dispatch SF failed. Status=%x\n", status_of_call);
    }
    return status;
#else
    *handle = psa_connect(sid, 1);
    if (*handle > 0)
    {
        test_data = ((uint32_t)(test_info.test_num) |
                    ((uint32_t)(test_info.block_num) << BLOCK_NUM_POS)
                    | ((uint32_t)(TEST_EXECUTE_FUNC) << ACTION_POS));
        psa_invec data[1] = {{&test_data, sizeof(test_data)} };

        status_of_call = psa_call(*handle, 0, data, 1, NULL, 0);
        if (status_of_call != PSA_SUCCESS)
        {
            status = VAL_STATUS_CALL_FAILED;
            val_print(ERROR, "Call to dispatch SF failed. Status=%x\n", status_of_call);
            psa_close(*handle);
        }
    }
    else
    {
        val_print(ERROR, "Could not connect SID. Handle=%x\n", *handle);
        status = VAL_STATUS_CONNECTION_FAILED;
    }

    return status;
#endif
}

/**
    @brief    - This function is used to retrive the status of previously connected test function
                using val_execute_secure_test_func
    @param    - handle     : handle of server function. Handle of Partition dispatcher sid
    @return   - The status of test functions
**/
val_status_t val_get_secure_test_result(psa_handle_t *handle)
{
    uint32_t        test_data;
    val_status_t    status = VAL_STATUS_SUCCESS;
    psa_status_t    status_of_call = PSA_SUCCESS;

    test_data = (TEST_RETURN_RESULT << ACTION_POS);

    psa_outvec resp = {&status, sizeof(status)};
    psa_invec data[1] = {{&test_data, sizeof(test_data)} };

    status_of_call = psa_call(*handle, 0, data, 1, &resp, 1);
    if (status_of_call != PSA_SUCCESS)
    {
        status = VAL_STATUS_CALL_FAILED;
        val_print(ERROR, "Call to dispatch SF failed. Status=%x\n", status_of_call);
    }
#if STATELESS_ROT != 1
    psa_close(*handle);
#endif
    return status;
}
#endif

/**
 *   @brief    Parses input status for a given test and
 *             outputs appropriate information on the console
 *   @return   Test state
**/
uint32_t val_report_status_bespoke(void)
{
    uint32_t status, state, status_code;

    status = val_get_status();

    state = (status >> TEST_STATE_SHIFT) & TEST_STATE_MASK;
    status_code = status & TEST_STATUS_CODE_MASK;

    switch (state)
    {
        case TEST_START:
            state = TEST_FAIL;
            val_print(ALWAYS, "\nResult=Failed (Error Code=0x%x)\n",
                                            VAL_STATUS_INIT_FAILED);
            break;

        case TEST_END:
        case TEST_PASS:
            state = TEST_PASS;
            val_print(ALWAYS, "\nResult=Passed\n", 0);
            break;

        case TEST_FAIL:
            state = TEST_FAIL;
            val_print(ALWAYS, "\nResult=Failed (Error code=0x%x)\n",
                status_code);
            break;

        case TEST_SKIP:
            state = TEST_SKIP;
            val_print(ALWAYS, "\nResult=Skipped (Skip code=0x%x)\n",
                status_code);
            break;

        case TEST_ERROR:
            state = TEST_ERROR;
            val_print(ALWAYS, "\nResult=Sim Error (Error code=0x%x)\n",
                status_code);
            break;

        default:
            state = TEST_FAIL;
            val_print(ALWAYS, "\nResult=Failed (Error Code=0x%x)\n",
                status_code);
            break;
    }

    val_print(ALWAYS, "***********************************\n", 0);
    return state;
}

/*
    @brief           - This function checks if the input status argument is an error.
                       On error, we print the checkpoint value and set the status.
    @param           - checkpoint     : Test debug checkpoint
                     - val_status_t   : Test status
    @return          - returns the input status back to the program.
*/

val_status_t val_err_check_set(uint32_t checkpoint, val_status_t status)
{
    if (VAL_ERROR(status))
    {
        val_print(ERROR, "\tCheckpoint %d : ", checkpoint);
        val_print(ERROR, "Error Code=0x%x \n", status);
        val_set_status(RESULT_FAIL(status));
    }
    else
    {
        status = (val_get_status() & TEST_STATUS_CODE_MASK);
        if (VAL_ERROR(status))
        {
            val_print(ERROR, "\tCheckpoint %d : ", checkpoint);
            val_print(ERROR, "Error Code=0x%x \n", status);
        }
        else
        {
            val_print(DBG, "\tCheckpoint %d \n", checkpoint);
        }
    }
    return status;
}

/**
  @brief  This API prints the test number, description and
          sets the test state to TEST_START on successful execution.
  @param  test_num       :unique number identifying this test
  @param  desc           :brief description of the test
  @param  test_bitfield  :Addition test info such as
                           - test isolation level requirement
                           - Watchdog timeout type
  @return void
**/

void val_test_init(uint32_t test_num, uint32_t comp_num, char8_t *desc, uint32_t test_bitfield)
{
   val_status_t         status = VAL_STATUS_SUCCESS;

   val_test_status_buffer_ts *status_buffer =
                              (val_test_status_buffer_ts *)PLATFORM_SHARED_REGION_BASE;
   char *suite;

   switch (comp_num)
    {
        case VAL_FF_BASE:
            suite = "IPC";
            break;
        case VAL_CRYPTO_BASE:
            suite = "Crypto";
            break;
        case VAL_STORAGE_BASE:
            suite = "Storage";
            break;
        case VAL_INITIAL_ATTESTATION_BASE:
            suite = "Attestation";
            break;
        default:
            suite = "Unknown";
            break;
    }

   /*global init*/
   status_buffer->state        = TEST_FAIL;
   status_buffer->status_code  = VAL_STATUS_INVALID;

   val_print(ALWAYS, "\rSuite=", 0);
   val_print(ALWAYS, suite, 0);
   val_print(ALWAYS, " : Test=", 0);
   val_print(ALWAYS, desc, 0);
   val_print(ALWAYS, "Num=%d\n", test_num);

   /* common skip logic */
   if (PLATFORM_PSA_ISOLATION_LEVEL < GET_TEST_ISOLATION_LEVEL(test_bitfield))
   {
       val_set_status(RESULT_SKIP(VAL_STATUS_ISOLATION_LEVEL_NOT_SUPP));
       val_print(ALWAYS, "\tSkipping test. Required isolation level is not supported\n", 0);
       return;
   }

#ifdef WATCHDOG_AVAILABLE
   /* Initialise watchdog */
   status = val_wd_timer_init(GET_WD_TIMOUT_TYPE(test_bitfield));
   if (VAL_ERROR(status))
   {
       val_print(ERROR, "\tval_wd_timer_init failed Error=0x%x\n", status);
       return;
   }

   /* Enable watchdog Timer */
   status = val_watchdog_enable();
   if (VAL_ERROR(status))
   {
       val_print(ERROR, "\tval_wd_timer_enable failed Error=0x%x\n", status);
       return;
   }
#endif

   val_set_status(RESULT_START(status));
   return;
}

/**
  @brief  This API sets the test state to TEST_PASS if test is successfuly passed.
  @param none
  @return none
**/

void val_test_exit(void)
{
    val_status_t         status = VAL_STATUS_SUCCESS;

#ifdef WATCHDOG_AVAILABLE
    status = val_watchdog_disable();
    if (VAL_ERROR(status))
    {
       val_print(ERROR, "\tval_wd_timer_disable failed Error=0x%x\n", status);
       val_set_status(RESULT_FAIL(status));
       return;
    }
#endif

    status = val_get_status();

    /* return if test skipped or failed */
    if (IS_TEST_FAIL(status) || IS_TEST_SKIP(status))
    {
        return;
    }
    else
    {
        val_set_status(RESULT_PASS(VAL_STATUS_SUCCESS));
    }
}

/**
    @brief    - This function returns the test ID of the last test that was run
    @param    - test_id address
    @return   - val_status_t
**/
val_status_t val_get_last_run_test_id(test_id_t *test_id)
{
    val_status_t    status;
    regre_report_t  test_count;
    boot_t          boot;
    int             intermediate_boot = 0;
    uint8_t         boot_state[] = {BOOT_NOT_EXPECTED,
                                    BOOT_EXPECTED_NS,
                                    BOOT_EXPECTED_S,
                                    BOOT_EXPECTED_BUT_FAILED,
                                    BOOT_EXPECTED_REENTER_TEST,
                                    BOOT_EXPECTED_CONT_TEST_EXEC,
                                    BOOT_EXPECTED_ON_SECOND_CHECK
                                    };

    status = val_get_boot_flag(&boot.state);
    if (VAL_ERROR(status))
    {
        return status;
    }

    val_print(INFO, "\n\tboot.state=0x%x", boot.state);

    intermediate_boot = is_reboot_run(boot.state, boot_state,
                                    sizeof(boot_state)/sizeof(boot_state[0]));

    if (!intermediate_boot)
    {
         /* First boot. Initiliase necessary data structure */
         status = val_set_boot_flag(BOOT_UNKNOWN);
         if (VAL_ERROR(status))
         {
             return status;
         }

         *test_id = VAL_INVALID_TEST_ID;
         status = val_nvm_write(VAL_NVM_OFFSET(NVM_PREVIOUS_TEST_ID),
                                  test_id, sizeof(test_id_t));
         if (VAL_ERROR(status))
         {
             val_print(ALWAYS, "\n\tNVMEM write error", 0);
             return status;
         }

         val_reset_regression_report(&test_count);
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
    }

    status = val_nvm_read(VAL_NVM_OFFSET(NVM_PREVIOUS_TEST_ID), test_id, sizeof(test_id_t));
    if (VAL_ERROR(status))
    {
        val_print(ERROR, "\n\tNVMEM read error", 0);
    }

    val_print(INFO, "In val_get_last_run_test_id, test_id=%x\n", *test_id);
    return status;
}

/**
    @brief    - This function sets the given boot.state value to corresponding
                boot NVMEM location
    @param    - state: boot_state_t
    @return   - val_status_t
**/
val_status_t val_set_boot_flag(boot_state_t state)
{
   boot_t           boot;
   val_status_t     status;

   boot.state = state;
   status = val_nvm_write(VAL_NVM_OFFSET(NVM_BOOT), &boot, sizeof(boot_t));
   if (VAL_ERROR(status))
   {
       val_print(ERROR, "\tval_nvmem_write failed. Error=0x%x\n", status);
       return status;
   }
   return status;
}

/**
    @brief    - This function returns boot.state value available in boot NVMEM location
    @param    - state address
    @return   - val_status_t
**/
val_status_t val_get_boot_flag(boot_state_t *state)
{
   boot_t           boot;
   val_status_t     status;

   status = val_nvm_read(VAL_NVM_OFFSET(NVM_BOOT), &boot, sizeof(boot_t));
   if (VAL_ERROR(status))
   {
       val_print(ERROR, "\tval_nvmem_read failed. Error=0x%x\n", status);
       return status;
   }
   *state = boot.state;
   return status;
}

/**
    @brief    - This function sets the test specific data
                NVMEM location
    @param    - nvm index
    @param    - nvm testdata
    @return   - val_status_t
**/
val_status_t val_set_test_data(int32_t nvm_index, int32_t test_data)
{
   val_status_t     status;

   status = val_nvm_write(VAL_NVM_OFFSET(nvm_index), &test_data, sizeof(int32_t));
   if (VAL_ERROR(status))
   {
       val_print(ERROR, "\tval_nvmem_write failed for test data. Error=0x%x\n", status);
       return status;
   }
   return status;
}

/**
    @brief    - This function gets the test specific data
                NVMEM location
    @param    - nvm index
    @param    - nvm testdata
    @return   - val_status_t
**/
val_status_t val_get_test_data(int32_t nvm_index, int32_t *test_data)
{
   val_status_t     status;

   status = val_nvm_read(VAL_NVM_OFFSET(nvm_index), test_data, sizeof(int32_t));
   if (VAL_ERROR(status))
   {
       val_print(ERROR, "\tval_nvmem_read failed for test data. Error=0x%x\n", status);
       return status;
   }
   return status;
}

