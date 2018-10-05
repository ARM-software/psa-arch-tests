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

#include "val/common/val_client_defs.h"
#include "val/spe/val_partition_common.h"

int32_t server_test_psa_drop_connection(void);

server_test_t test_i027_server_tests_list[] = {
    NULL,
    server_test_psa_drop_connection,
    NULL,
};

int32_t server_test_psa_drop_connection(void)
{
    int32_t         status = VAL_STATUS_SUCCESS;
    psa_msg_t       msg = {0};

    status = val_process_connect_request(SERVER_CONNECTION_DROP_SIG, &msg);
    if (val_err_check_set(TEST_CHECKPOINT_NUM(201), status))
    {
        psa_reply(msg.handle, PSA_CONNECTION_REFUSED);
        return status;
    }

    psa_reply(msg.handle, PSA_SUCCESS);

    status = val_process_call_request(SERVER_CONNECTION_DROP_SIG, &msg);
    if (val_err_check_set(TEST_CHECKPOINT_NUM(202), status))
    {
        psa_reply(msg.handle, -2);
        goto exit;
    }
    else
    {
        psa_reply(msg.handle, PSA_DROP_CONNECTION);
    }

exit:
    /* SPM must deliver a PSA_IPC_DISCONNECT message for the connection to the RoT Service
       directly after receipt of the PSA_DROP_CONNECTION completion to allow connection resources
       within the RoT Service to be released */
    status = val_process_disconnect_request(SERVER_CONNECTION_DROP_SIG, &msg);
    if (val_err_check_set(TEST_CHECKPOINT_NUM(203), status))
    {
        val_print(PRINT_ERROR, "\tDisconnect request failed\n", 0);
    }
    psa_reply(msg.handle, PSA_SUCCESS);
    return status;
}
