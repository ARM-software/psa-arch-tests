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

int32_t server_test_psa_call_with_iovec_more_than_max_limit();

server_test_t test_i026_server_tests_list[] = {
    NULL,
    server_test_psa_call_with_iovec_more_than_max_limit,
    NULL,
};

int32_t server_test_psa_call_with_iovec_more_than_max_limit()
{
    int32_t         status = VAL_STATUS_SUCCESS;
    psa_msg_t       msg = {0};

    status = val_process_connect_request(SERVER_UNSPECIFED_MINOR_V_SIG, &msg);
    if (val_err_check_set(TEST_CHECKPOINT_NUM(201), status))
    {
        psa_reply(msg.handle, PSA_CONNECTION_REFUSED);
        return status;
    }

    psa_reply(msg.handle, PSA_SUCCESS);

    val_process_call_request(SERVER_UNSPECIFED_MINOR_V_SIG, &msg);

    /* Control shouldn't have come here */
    val_print(PRINT_ERROR, "\tControl shouldn't have reached here\n", 0);
    psa_reply(msg.handle, -2);

    val_process_disconnect_request(SERVER_UNSPECIFED_MINOR_V_SIG, &msg);
    psa_reply(msg.handle, PSA_SUCCESS);
    return VAL_STATUS_SPM_FAILED;
}
