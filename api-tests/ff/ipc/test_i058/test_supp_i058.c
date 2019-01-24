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

int32_t server_test_psa_doorbell_signal(void);

server_test_t test_i058_server_tests_list[] = {
    NULL,
    server_test_psa_doorbell_signal,
    NULL,
};

int32_t server_test_psa_doorbell_signal(void)
{
    int32_t                 status = VAL_STATUS_SUCCESS;
    psa_msg_t               msg = {0};

    /* Serve psa_connect */
    status = val_process_connect_request(SERVER_UNSPECIFED_MINOR_V_SIG, &msg);
    if (val_err_check_set(TEST_CHECKPOINT_NUM(201), status))
    {
        psa_reply(msg.handle, PSA_CONNECTION_REFUSED);
        return status;
    }

    /* Accept the connection */
    psa_reply(msg.handle, PSA_SUCCESS);

    if (msg.client_id > 0)
    {
        /* Doorbell signal to client partititon */
        psa_notify(msg.client_id);
    }
    else
    {
        status = VAL_STATUS_SPM_FAILED;
        val_print(PRINT_ERROR, "Caller is from non-secure\n", 0);
    }

    /* Serve psa_close */
    status = ((val_process_disconnect_request(SERVER_UNSPECIFED_MINOR_V_SIG, &msg))
               ? VAL_STATUS_ERROR : status);
    val_err_check_set(TEST_CHECKPOINT_NUM(202), status);
    psa_reply(msg.handle, PSA_SUCCESS);

    return status;
}
