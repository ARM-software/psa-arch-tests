/** @file
 * Copyright (c) 2019, Arm Limited or its affiliates. All rights reserved.
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

#include "val/common/val_client_defs.h"
#include "val/spe/val_partition_common.h"

int32_t server_test_psa_wait_signal_mask(void);

server_test_t test_i063_server_tests_list[] = {
    NULL,
    server_test_psa_wait_signal_mask,
    NULL,
};

int32_t server_test_psa_wait_signal_mask(void)
{
    psa_signal_t    signals = 0;
    psa_msg_t       msg = {0};
    int             loop_cnt = 2;
    psa_signal_t    signal_mask = (SERVER_UNSPECIFED_MINOR_V_SIG | SERVER_RELAX_MINOR_VERSION_SIG);

    /* Debug print */
    val_err_check_set(TEST_CHECKPOINT_NUM(211), VAL_STATUS_SUCCESS);

    /* Notify client partition to make SERVER_SECURE_CONNECT_ONLY_SID connection request.
     * This connection request act as irritator to psa_wait(signal_mask) call and it is used
     * to cover the rule - Signals that are not in signal_mask should be ignored by psa_wait.
     * This means, during the following while loop, returned signal vaule should not be
     * SERVER_SECURE_CONNECT_ONLY_SIG as this signal is not part of signal_mask.
     */
    psa_notify(CLIENT_PARTITION);

    while (loop_cnt != 0)
    {
         signals = psa_wait(signal_mask, PSA_BLOCK);

         /* Rule - Returned signals value must be subset signals indicated in the signal_mask.
          *
          * This mean signal value should be either SERVER_UNSPECIFED_MINOR_V_SIG
          * or SERVER_RELAX_MINOR_VERSION_SIG.
          */
         if (((signals & signal_mask) == 0) &&
             ((signals | signal_mask) != signal_mask))
         {
             val_print(PRINT_ERROR,
                     "psa_wait-1 returned with invalid signal value = 0x%x\n", signals);
             return VAL_STATUS_ERROR;
         }
         else if (signals & SERVER_UNSPECIFED_MINOR_V_SIG)
         {
             if (psa_get(SERVER_UNSPECIFED_MINOR_V_SIG, &msg) != PSA_SUCCESS)
                 continue;

             loop_cnt--;
             psa_reply(msg.handle, PSA_CONNECTION_REFUSED);
         }
         else if (signals & SERVER_RELAX_MINOR_VERSION_SIG)
         {
             if (psa_get(SERVER_RELAX_MINOR_VERSION_SIG, &msg) != PSA_SUCCESS)
                 continue;

             loop_cnt--;
             psa_reply(msg.handle, PSA_CONNECTION_REFUSED);
         }
    }

    /* Debug print */
    val_err_check_set(TEST_CHECKPOINT_NUM(212), VAL_STATUS_SUCCESS);

wait:
    /* At the end, completes the starved connection
     * request of SERVER_SECURE_CONNECT_ONLY_SID.
     */
    signals = psa_wait(SERVER_SECURE_CONNECT_ONLY_SIG, PSA_BLOCK);
    if ((signals & SERVER_SECURE_CONNECT_ONLY_SIG) == 0)
    {
       val_print(PRINT_ERROR,
                "psa_wait-2 returned with invalid signal value = 0x%x\n", signals);
       return VAL_STATUS_ERROR;
    }

    if (psa_get(SERVER_SECURE_CONNECT_ONLY_SIG, &msg) != PSA_SUCCESS)
        goto wait;

    psa_reply(msg.handle, PSA_CONNECTION_REFUSED);
    return VAL_STATUS_SUCCESS;
}
