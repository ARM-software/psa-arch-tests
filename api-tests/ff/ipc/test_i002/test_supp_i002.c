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

#include "val/common/val_client_defs.h"
#include "val/spe/val_partition_common.h"

#define CONNECT_NUM 2

int32_t server_test_connection_busy_and_reject(void);
int32_t server_test_accept_and_close_connect(void);
int32_t server_test_connect_with_allowed_minor_version_policy(void);
int32_t server_test_psa_call_with_allowed_status_code(void);
int32_t server_test_identity(void);
int32_t server_test_spm_concurrent_connect_limit(void);
int32_t server_test_psa_block_behave(void);
int32_t server_test_psa_poll_behave(void);

server_test_t test_i002_server_tests_list[] = {
    NULL,
    server_test_connection_busy_and_reject,
    server_test_accept_and_close_connect,
    server_test_connect_with_allowed_minor_version_policy,
    server_test_psa_call_with_allowed_status_code,
    server_test_identity,
    server_test_spm_concurrent_connect_limit,
    server_test_psa_block_behave,
    server_test_psa_poll_behave,
    NULL,
};

int32_t server_test_connection_busy_and_reject(void)
{
    int32_t     status = VAL_STATUS_SUCCESS;
    psa_msg_t   msg = {0};

    /* Checks performed:
     * psa_wait()- Returns > 0 when at least one signal is asserted
     * check delivery of PSA_IPC_CONNECT when psa_connect called.
     * And msg.handle must be positive.
    */
    status = val_process_connect_request(SERVER_UNSPECIFED_MINOR_V_SIG, &msg);
    if (val_err_check_set(TEST_CHECKPOINT_NUM(201), status))
    {
        return status;
    }

    /* Rejecting connection to check behaviour of PSA_CONNECTION_BUSY */
    psa_reply(msg.handle, PSA_CONNECTION_BUSY);

    status = val_process_connect_request(SERVER_UNSPECIFED_MINOR_V_SIG, &msg);
    if (val_err_check_set(TEST_CHECKPOINT_NUM(202), status))
    {
        return status;
    }

    /* Rejecting connection to check behaviour of PSA_CONNECTION_REFUSED */
    psa_reply(msg.handle, PSA_CONNECTION_REFUSED);
    return status;
}

int32_t server_test_accept_and_close_connect(void)
{
    int32_t     status = VAL_STATUS_SUCCESS;
    psa_msg_t   msg = {0};

    status = val_process_connect_request(SERVER_UNSPECIFED_MINOR_V_SIG, &msg);
    if (val_err_check_set(TEST_CHECKPOINT_NUM(203), status))
    {
        /* Reject the connection if processing of connect request has failed */
        psa_reply(msg.handle, PSA_CONNECTION_REFUSED);
        return status;
    }

    /* Accepting connection to check behaviour of PSA_SUCCESS */
    psa_reply(msg.handle, PSA_SUCCESS);

    /* Checking delivery of PSA_IPC_DISCONNECT when psa_close called
     * msg.handle must be positive
     */
    status = val_process_disconnect_request(SERVER_UNSPECIFED_MINOR_V_SIG, &msg);
    if (val_err_check_set(TEST_CHECKPOINT_NUM(204), status))
    {
        return status;
    }

    /* Sanity check - if the message type is PSA_IPC_DISCONNECT then the status code is ignored.*/
    psa_reply(msg.handle, PSA_CONNECTION_REFUSED);
    /* Debug print for sanity check */
    val_err_check_set(TEST_CHECKPOINT_NUM(205), status);
    return status;
}

int32_t server_test_connect_with_allowed_minor_version_policy(void)
{
    int32_t         status = VAL_STATUS_SUCCESS;
    psa_msg_t       msg = {0};
    int             i = 0;
    psa_signal_t    signal[4] = {SERVER_UNSPECIFED_MINOR_V_SIG,
                                 SERVER_STRICT_MINOR_VERSION_SIG,
                                 SERVER_RELAX_MINOR_VERSION_SIG,
                                 SERVER_RELAX_MINOR_VERSION_SIG};

    for (i = 0; i < 4; i++)
    {
        status = ((val_process_connect_request(signal[i], &msg))
                                    ? VAL_STATUS_ERROR : status);
        if (val_err_check_set(TEST_CHECKPOINT_NUM(206), status))
        {
            psa_reply(msg.handle, PSA_CONNECTION_REFUSED);
            return status;
        }

        psa_reply(msg.handle, PSA_SUCCESS);

        status = ((val_process_disconnect_request(signal[i], &msg))
                                        ? VAL_STATUS_ERROR : status);
        val_err_check_set(TEST_CHECKPOINT_NUM(207), status);
        psa_reply(msg.handle, PSA_SUCCESS);
    }
    return status;
}

int32_t server_test_psa_call_with_allowed_status_code(void)
{
    int32_t         status = VAL_STATUS_SUCCESS;
    psa_msg_t       msg = {0};
    uint32_t        i = 0;
    psa_status_t    status_code[] = {PSA_SUCCESS, 1, 2, INT32_MAX, -1, -2, INT32_MIN+128};

    for (i = 0; i < (sizeof(status_code)/sizeof(status_code[0])); i++)
    {
        status = ((val_process_connect_request(SERVER_UNSPECIFED_MINOR_V_SIG, &msg))
                                        ? VAL_STATUS_ERROR : status);
        if (val_err_check_set(TEST_CHECKPOINT_NUM(208), status))
        {
            psa_reply(msg.handle, PSA_CONNECTION_REFUSED);
            return status;
        }
        psa_reply(msg.handle, PSA_SUCCESS);

        status = val_process_call_request(SERVER_UNSPECIFED_MINOR_V_SIG, &msg);
        if (val_err_check_set(TEST_CHECKPOINT_NUM(209), status))
        {
            /* Send status code other than status_code[] to indicate failure
             * in processing call request
             */
            psa_reply(msg.handle, -3);
        }
        else
        {
            /* Send various status code available in status_code[] */
            psa_reply(msg.handle, status_code[i]);
        }

        status = ((val_process_disconnect_request(SERVER_UNSPECIFED_MINOR_V_SIG, &msg))
                                        ? VAL_STATUS_ERROR : status);
        psa_reply(msg.handle, PSA_SUCCESS);
        val_err_check_set(TEST_CHECKPOINT_NUM(210), status);
    }
    return status;
}

int32_t server_test_identity(void)
{
    int32_t     status = VAL_STATUS_SUCCESS;
    psa_msg_t   msg = {0};
    int         id_at_connect = 0, id_at_call = 0;

    status = val_process_connect_request(SERVER_UNSPECIFED_MINOR_V_SIG, &msg);

    if (val_err_check_set(TEST_CHECKPOINT_NUM(211), status))
    {
        psa_reply(msg.handle, PSA_CONNECTION_REFUSED);
        return status;
    }

    /* Client ID during connect */
    id_at_connect = msg.client_id;
    psa_reply(msg.handle, PSA_SUCCESS);

    status = val_process_call_request(SERVER_UNSPECIFED_MINOR_V_SIG, &msg);
    if (val_err_check_set(TEST_CHECKPOINT_NUM(212), status))
    {
        psa_reply(msg.handle, -3);
    }
    else
    {
        /* Client ID during call */
        id_at_call = msg.client_id;
        psa_write(msg.handle, 0, &id_at_connect, msg.out_size[0]);
        psa_write(msg.handle, 1, &id_at_call, msg.out_size[1]);
        psa_reply(msg.handle, PSA_SUCCESS);
    }

    status = val_process_disconnect_request(SERVER_UNSPECIFED_MINOR_V_SIG, &msg);
    val_err_check_set(TEST_CHECKPOINT_NUM(213), status);
    /* Client ID during disconnect. It should be equal to id_at_call */
    if (msg.client_id != id_at_call)
    {
        val_print(PRINT_ERROR, "\tmsg.client_id failed for IPC_DISCONNECT", 0);
        status = VAL_STATUS_WRONG_IDENTITY;
    }

    /* A Partition must have an alphanumeric name for source code to directly refer
     * to a specific Partition. The Secure Partition ID can be referenced in
     * Secure Partition source code via the symbolic name specified as name attribute.
     * The symbol must have the value of the Secure Partition ID.
     * Using CLIENT_PARTITION define to cover this rule.
     */
    if ((msg.client_id > 0) && (msg.client_id != CLIENT_PARTITION))
    {
        val_print(PRINT_ERROR, "\tmsg.client_id failed for CLIENT_PARTITION", 0);
        status = VAL_STATUS_WRONG_IDENTITY;
    }
    psa_reply(msg.handle, PSA_SUCCESS);
    return status;
}

int32_t server_test_spm_concurrent_connect_limit(void)
{
    psa_signal_t    signals = 0;
    psa_msg_t       msg = {0};
    int             count = 0;
    int32_t         status = VAL_STATUS_SUCCESS;

    while (1)
    {
       signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
       if ((signals & SERVER_UNSPECIFED_MINOR_V_SIG) == 0)
       {
          val_print(PRINT_ERROR,
                    "psa_wait returned with invalid signal value = 0x%x\n", signals);
          return VAL_STATUS_ERROR;
       }

       if (psa_get(SERVER_UNSPECIFED_MINOR_V_SIG, &msg) != PSA_SUCCESS)
           continue;

       switch(msg.type)
       {
           case PSA_IPC_CONNECT:
                /* serve bulk connect cases to reach connect limit */
                count++;
                psa_reply(msg.handle, PSA_SUCCESS);
                break;
           case PSA_IPC_DISCONNECT:
                /* serve bulk disconnect cases */
                count--;
                psa_reply(msg.handle, PSA_SUCCESS);
       }
       if (count == 0)
       {
           /* Closed all open connection. Come out of loop */
           break;
       }
    }
    return status;
}

int32_t server_test_psa_block_behave(void)
{
    psa_signal_t    signals = 0;
    psa_msg_t       msg = {0};
    int             i = 0;

    /* This is a sanity check - a successful handshaking between client and
     * server for requested connection represents check pass.
     */

    /* Debug print */
    val_err_check_set(TEST_CHECKPOINT_NUM(214), VAL_STATUS_SUCCESS);

    for (i = 0; i < CONNECT_NUM; i++)
    {
wait:
         /* PSA_BLOCK ored with 0xFF to check timeout[30:0]=RES is ignored by implementation */
         signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);

         /* When MODE is one(PSA_BLOCK), the psa_wait must return non-zero signal value */
         if ((signals & SERVER_UNSPECIFED_MINOR_V_SIG) == 0)
         {
             val_print(PRINT_ERROR,
                     "psa_wait returned with invalid signal value = 0x%x\n", signals);
             return VAL_STATUS_ERROR;
         }
         else
         {
             if (psa_get(SERVER_UNSPECIFED_MINOR_V_SIG, &msg) != PSA_SUCCESS)
             {
                 goto wait;
             }

             psa_reply(msg.handle, PSA_CONNECTION_REFUSED);
         }
    }

    /* Debug print */
    val_err_check_set(TEST_CHECKPOINT_NUM(215), VAL_STATUS_SUCCESS);

    return VAL_STATUS_SUCCESS;
}

int32_t server_test_psa_poll_behave(void)
{
    psa_signal_t    signals = 0, signals_temp = 0;
    psa_msg_t       msg = {0};
    int             count = 0;

    while (1)
    {
        /* Debug print */
        val_err_check_set(TEST_CHECKPOINT_NUM(216), VAL_STATUS_SUCCESS);

        /* Loop to receive client request */
        while (signals == 0)
        {
            signals = psa_wait(PSA_WAIT_ANY, PSA_POLL);
        }

        /* When MODE is zero(PSA_POLL), the psa_wait will return immediately with the current
         * signal state, which can be zero if no signals are active. Exepecting following call to
         * return immediately as none of client is making request.
         */
        signals_temp = psa_wait(PSA_WAIT_ANY, PSA_POLL);

        if (signals_temp == 0)
        {
            val_print(PRINT_ERROR,
                    "psa_wait returned with invalid signals_temp = 0x%x\n", signals_temp);
            return VAL_STATUS_ERROR;
        }
        else if ((signals & SERVER_UNSPECIFED_MINOR_V_SIG) == 0)
        {
            val_print(PRINT_ERROR,
                    "psa_wait returned with invalid signal value = 0x%x\n", signals);
            return VAL_STATUS_ERROR;
        }
        else
        {
            if (psa_get(SERVER_UNSPECIFED_MINOR_V_SIG, &msg) != PSA_SUCCESS)
                continue;
            psa_reply(msg.handle, PSA_CONNECTION_REFUSED);
            count++;
            signals = 0;
        }

        /* Come out of loop as we reached required connect limit*/
        if (count == CONNECT_NUM)
            break;
    }
    return VAL_STATUS_SUCCESS;
}
