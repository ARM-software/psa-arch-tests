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

#include "test_i073.h"

#if STATELESS_ROT == 1

const client_test_t test_i073_client_tests_list[] = {
    NULL,
    client_test_nspe_read_app_rot_stack,
    client_test_nspe_write_app_rot_stack,
    NULL,
};

static int32_t get_secure_partition_address(addr_t *addr)
{
   /* Get App-RoT address */
   psa_outvec outvec[1] = { {addr, sizeof(addr_t)} };
  if (psa->call(SERVER_UNSPECIFIED_VERSION_HANDLE, PSA_IPC_CALL, NULL, 0, outvec, 1) != PSA_SUCCESS)
   {
	   val->print(ERROR, "\tmsg request failed\n", 0);
       return VAL_STATUS_CALL_FAILED;
   }

   val->print(DBG, "\tNSPE: Accessing address 0x%x\n", *addr);

   return VAL_STATUS_SUCCESS;
}

int32_t client_test_nspe_read_app_rot_stack(caller_security_t caller __UNUSED)
{
   addr_t   app_rot_addr;
   uint32_t data = 0x1234;

   val->print(TEST, "Check 1: Test NSPE reading APP-RoT stack\n", 0);

   if (VAL_ERROR(get_secure_partition_address(&app_rot_addr)))
       return VAL_STATUS_ERROR;

   /* Setting boot.state before test check */
   if (val->set_boot_flag(BOOT_EXPECTED_REENTER_TEST))
   {
       val->print(ERROR, "\tFailed to set boot flag before check\n", 0);
       return VAL_STATUS_ERROR;
   }

   /* Read Application RoT stack address.
    * This should generate internal fault or ignore the read.
    */
   data = *(uint32_t *)app_rot_addr;

   /* Did read ignore? */
   if (data == 0x1234)
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

int32_t client_test_nspe_write_app_rot_stack(caller_security_t caller __UNUSED)
{
   addr_t   app_rot_addr;
   uint32_t data = 0x1234;

   val->print(TEST, "Check 2: Test NSPE writing APP-RoT stack\n", 0);

   if (VAL_ERROR(get_secure_partition_address(&app_rot_addr)))
       return VAL_STATUS_ERROR;

   /* Setting boot.state before test check */
   if (val->set_boot_flag(BOOT_EXPECTED_ON_SECOND_CHECK))
   {
       val->print(ERROR, "\tFailed to set boot flag before check\n", 0);
       return VAL_STATUS_ERROR;
   }

   /* Write Application RoT stack address.
    * This should generate internal fault or ignore the write.
    */
   *(uint32_t *)app_rot_addr = (uint32_t)data;

   return VAL_STATUS_SUCCESS;
}

#else

const client_test_t test_i073_client_tests_list[] = {
    NULL,
    client_test_nspe_read_app_rot_stack,
    client_test_nspe_write_app_rot_stack,
    NULL,
};

static int32_t get_secure_partition_address(addr_t *addr)
{
   psa_handle_t            handle = 0;

   handle = psa->connect(SERVER_UNSPECIFIED_VERSION_SID, SERVER_UNSPECIFIED_VERSION_VERSION);
   if (!PSA_HANDLE_IS_VALID(handle))
   {
       val->print(ERROR, "\tConnection failed\n", 0);
       return VAL_STATUS_INVALID_HANDLE;
   }

   /* Get App-RoT address */
   psa_outvec outvec[1] = {{addr, sizeof(addr_t)} };
   if (psa->call(handle, PSA_IPC_CALL, NULL, 0, outvec, 1) != PSA_SUCCESS)
   {
       val->print(ERROR, "\tmsg request failed\n", 0);
       return VAL_STATUS_CALL_FAILED;
   }

   val->print(DBG, "\tNSPE: Accessing address 0x%x\n", *addr);

   psa->close(handle);

   return VAL_STATUS_SUCCESS;
}

int32_t client_test_nspe_read_app_rot_stack(caller_security_t caller __UNUSED)
{
   addr_t   app_rot_addr;
   uint32_t data = 0x1234;

   val->print(TEST, "Check 1: Test NSPE reading APP-RoT stack\n", 0);

   if (VAL_ERROR(get_secure_partition_address(&app_rot_addr)))
       return VAL_STATUS_ERROR;

   /* Setting boot.state before test check */
   if (val->set_boot_flag(BOOT_EXPECTED_REENTER_TEST))
   {
       val->print(ERROR, "\tFailed to set boot flag before check\n", 0);
       return VAL_STATUS_ERROR;
   }

   /* Read Application RoT stack address.
    * This should generate internal fault or ignore the read.
    */
   data = *(uint32_t *)app_rot_addr;

   /* Did read ignore? */
   if (data == 0x1234)
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

int32_t client_test_nspe_write_app_rot_stack(caller_security_t caller __UNUSED)
{
   addr_t   app_rot_addr;
   uint32_t data = 0x1234;

   val->print(TEST, "Check 2: Test NSPE writing APP-RoT stack\n", 0);

   if (VAL_ERROR(get_secure_partition_address(&app_rot_addr)))
       return VAL_STATUS_ERROR;

   /* Setting boot.state before test check */
   if (val->set_boot_flag(BOOT_EXPECTED_ON_SECOND_CHECK))
   {
       val->print(ERROR, "\tFailed to set boot flag before check\n", 0);
       return VAL_STATUS_ERROR;
   }

   /* Write Application RoT stack address.
    * This should generate internal fault or ignore the write.
    */
   *(uint32_t *)app_rot_addr = (uint32_t)data;

   /* Handshake with server to decide write status */
   if ((psa->connect(SERVER_UNSPECIFIED_VERSION_SID, 1)) > 0)
   {
       val->print(ERROR, "\tExpected connection to fail but succeed\n", 0);
       return VAL_STATUS_INVALID_HANDLE;
   }

   return VAL_STATUS_SUCCESS;
}

#endif
