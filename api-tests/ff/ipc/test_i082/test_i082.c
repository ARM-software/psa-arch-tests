/** @file
 * Copyright (c) 2019-2025, Arm Limited or its affiliates. All rights reserved.
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

#ifdef NONSECURE_TEST_BUILD
#include "val_interfaces.h"
#else
#include "val_client_defs.h"
#include "val_service_defs.h"
#endif

#include "test_i082.h"

#define DATA_VALUE_ORG 0x11
#define DATA_VALUE     0x12
#define BUFFER_SIZE    0x4

#if STATELESS_ROT == 1

const client_test_t test_i082_client_tests_list[] = {
    NULL,
    client_test_app_rot_read_psa_rot_heap,
    client_test_app_rot_write_psa_rot_heap,
    NULL,
};

#ifdef SP_HEAP_MEM_SUPP
static int32_t get_secure_partition_address(psa_handle_t *handle,
                                            addr_t *addr,
                                            driver_test_fn_id_t test_fn_id)
{
   /* Execute driver function related to TEST_ISOLATION_PSA_ROT_DATA_RD */
      psa_invec invec[1] = { {&test_fn_id, sizeof(test_fn_id)} };
      psa_outvec outvec[1] = { {addr, sizeof(addr_t)} };
      if (psa->call(DRIVER_TEST_HANDLE, PSA_IPC_CALL, invec, 1, outvec, 1) != PSA_SUCCESS)
      {
          val->print(ERROR, "\tmsg request failed\n", 0);
          return VAL_STATUS_CALL_FAILED;
      }

      return VAL_STATUS_SUCCESS;
}

static int32_t get_driver_status(psa_handle_t *handle)
{
   if (psa->call(*handle, PSA_IPC_CALL, NULL, 0, NULL, 0) != PSA_SUCCESS)
   {
       return VAL_STATUS_CALL_FAILED;
   }
   return VAL_STATUS_SUCCESS;
}

int32_t client_test_app_rot_read_psa_rot_heap(caller_security_t caller __UNUSED)
{
   addr_t       psa_rot_addr;
   uint8_t      data = DATA_VALUE;
   psa_handle_t handle = 0;

   val->print(TEST, "Check 1: Test APP-RoT reading PSA-RoT heap\n", 0);

   if (VAL_ERROR(get_secure_partition_address(&handle,
                                              &psa_rot_addr,
                                              TEST_ISOLATION_PSA_ROT_HEAP_RD)))
       return VAL_STATUS_ERROR;

   /* Setting boot.state before test check */
   if (val->set_boot_flag(BOOT_EXPECTED_REENTER_TEST))
   {
       val->print(ERROR, "\tFailed to set boot flag before check\n", 0);
       return VAL_STATUS_ERROR;
   }

   /* Read PSA RoT heap address.
    * This should generate internal fault or ignore the read.
    */
   data = *(uint8_t *)psa_rot_addr;

   /* Did read ignore? */
   if (data == DATA_VALUE)
        return VAL_STATUS_SUCCESS;

   val->print(ERROR, "\tExpected read to fault but it didn't\n", 0);

   /* Resetting boot.state to catch unwanted reboot */
   if (val->set_boot_flag(BOOT_EXPECTED_BUT_FAILED))
   {
       val->print(ERROR, "\tFailed to set boot flag after check\n", 0);
       return VAL_STATUS_ERROR;
   }

   return VAL_STATUS_SPM_FAILED;
}

int32_t client_test_app_rot_write_psa_rot_heap(caller_security_t caller __UNUSED)
{
   addr_t       psa_rot_addr;
   uint8_t      data = DATA_VALUE;
   psa_handle_t handle = 0;

   val->print(TEST, "Check 2: Test APP-RoT writing PSA-RoT heap\n", 0);

   if (VAL_ERROR(get_secure_partition_address(&handle,
                                              &psa_rot_addr,
                                              TEST_ISOLATION_PSA_ROT_HEAP_WR)))
       return VAL_STATUS_ERROR;

   /* Write PSA RoT heap address.
    * This should generate internal fault or ignore the write.
    */
   *(uint8_t *)psa_rot_addr = (uint8_t)data;

   /* Handshake with driver to decide write status */
   if (VAL_ERROR(get_driver_status(&handle)))
   {
       return VAL_STATUS_DRIVER_FN_FAILED;
   }
   return VAL_STATUS_SUCCESS;
}
#else
int32_t client_test_app_rot_read_psa_rot_heap(caller_security_t caller __UNUSED)
{
   val->print(TEST, "Check 1: Test APP-RoT reading PSA-RoT heap\n", 0);
   val->print(ERROR, "\tSkipping test as heap memory not supported\n", 0);
   return RESULT_SKIP(VAL_STATUS_HEAP_NOT_AVAILABLE);
}

int32_t client_test_app_rot_write_psa_rot_heap(caller_security_t caller __UNUSED)
{
   val->print(TEST, "Check 2: Test APP-RoT writing PSA-RoT heap\n", 0);
   val->print(ERROR, "\tSkipping test as heap memory not supported\n", 0);
   return RESULT_SKIP(VAL_STATUS_HEAP_NOT_AVAILABLE);
}
#endif

#else

const client_test_t test_i082_client_tests_list[] = {
    NULL,
    client_test_app_rot_read_psa_rot_heap,
    client_test_app_rot_write_psa_rot_heap,
    NULL,
};

#ifdef SP_HEAP_MEM_SUPP
static int32_t get_secure_partition_address(psa_handle_t *handle,
                                            addr_t *addr,
                                            driver_test_fn_id_t test_fn_id)
{
   *handle = psa->connect(DRIVER_TEST_SID, DRIVER_TEST_VERSION);
   if (!PSA_HANDLE_IS_VALID(*handle))
   {
       val->print(ERROR, "\tConnection failed\n", 0);
       return VAL_STATUS_INVALID_HANDLE;
   }

   /* Execute driver function related to TEST_ISOLATION_PSA_ROT_DATA_RD */
   psa_invec invec[1] = {{&test_fn_id, sizeof(test_fn_id)} };
   psa_outvec outvec[1] = {{addr, sizeof(addr_t)} };
   if (psa->call(*handle, PSA_IPC_CALL, invec, 1, outvec, 1) != PSA_SUCCESS)
   {
       val->print(ERROR, "\tmsg request failed\n", 0);
       return VAL_STATUS_CALL_FAILED;
   }

   return VAL_STATUS_SUCCESS;
}

static int32_t get_driver_status(psa_handle_t *handle)
{
   if (psa->call(*handle, PSA_IPC_CALL, NULL, 0, NULL, 0) != PSA_SUCCESS)
   {
       return VAL_STATUS_CALL_FAILED;
   }
   return VAL_STATUS_SUCCESS;
}

static void close_driver_fn(psa_handle_t *handle)
{
   psa->close(*handle);
}

int32_t client_test_app_rot_read_psa_rot_heap(caller_security_t caller __UNUSED)
{
   addr_t       psa_rot_addr;
   uint8_t      data = DATA_VALUE;
   psa_handle_t handle = 0;

   val->print(TEST, "Check 1: Test APP-RoT reading PSA-RoT heap\n", 0);

   if (VAL_ERROR(get_secure_partition_address(&handle,
                                              &psa_rot_addr,
                                              TEST_ISOLATION_PSA_ROT_HEAP_RD)))
    {
       return VAL_STATUS_ERROR;
    }

   close_driver_fn(&handle);

   /* Setting boot.state before test check */
   if (val->set_boot_flag(BOOT_EXPECTED_REENTER_TEST))
   {
       val->print(ERROR, "\tFailed to set boot flag before check\n", 0);
       return VAL_STATUS_ERROR;
   }

   /* Read PSA RoT heap address.
    * This should generate internal fault or ignore the read.
    */
   data = *(uint8_t *)psa_rot_addr;

   /* Did read ignore? */
   if (data == DATA_VALUE)
        return VAL_STATUS_SUCCESS;

   val->print(ERROR, "\tExpected read to fault but it didn't\n", 0);

   /* Resetting boot.state to catch unwanted reboot */
   if (val->set_boot_flag(BOOT_EXPECTED_BUT_FAILED))
   {
       val->print(ERROR, "\tFailed to set boot flag after check\n", 0);
       return VAL_STATUS_ERROR;
   }

   return VAL_STATUS_SPM_FAILED;
}

int32_t client_test_app_rot_write_psa_rot_heap(caller_security_t caller __UNUSED)
{
   addr_t       psa_rot_addr;
   uint8_t      data = DATA_VALUE;
   psa_handle_t handle = 0;

   val->print(TEST, "Check 2: Test APP-RoT writing PSA-RoT heap\n", 0);

   if (VAL_ERROR(get_secure_partition_address(&handle,
                                              &psa_rot_addr,
                                              TEST_ISOLATION_PSA_ROT_HEAP_WR)))
       return VAL_STATUS_ERROR;

   /* Write PSA RoT heap address.
    * This should generate internal fault or ignore the write.
    */
   *(uint8_t *)psa_rot_addr = (uint8_t)data;

   /* Handshake with driver to decide write status */
   if (VAL_ERROR(get_driver_status(&handle)))
   {
       close_driver_fn(&handle);
       return VAL_STATUS_DRIVER_FN_FAILED;
   }

   close_driver_fn(&handle);
   return VAL_STATUS_SUCCESS;
}
#else
int32_t client_test_app_rot_read_psa_rot_heap(caller_security_t caller __UNUSED)
{
   val->print(TEST, "Check 1: Test APP-RoT reading PSA-RoT heap\n", 0);
   val->print(ERROR, "\tSkipping test as heap memory not supported\n", 0);
   return RESULT_SKIP(VAL_STATUS_HEAP_NOT_AVAILABLE);
}

int32_t client_test_app_rot_write_psa_rot_heap(caller_security_t caller __UNUSED)
{
   val->print(TEST, "Check 2: Test APP-RoT writing PSA-RoT heap\n", 0);
   val->print(ERROR, "\tSkipping test as heap memory not supported\n", 0);
   return RESULT_SKIP(VAL_STATUS_HEAP_NOT_AVAILABLE);
}
#endif

#endif
