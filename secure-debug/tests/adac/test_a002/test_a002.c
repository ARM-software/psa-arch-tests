/** @file
 * Copyright (c) 2021, Arm Limited or its affiliates. All rights reserved.
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

#include <psa_adac.h>
#include <val_adac.h>
#include "val_interfaces.h"

#include "test_a002.h"

#define TEST_NUM  VAL_CREATE_TEST_ID(VAL_SECURE_DEBUG_BASE, 2)
#define TEST_DESC "Testing ADAC Protocol Host API| UT: psa_connect\n"
TEST_PUBLISH(TEST_NUM, test_entry);

void test_entry(val_api_t *val_api)
{
    int32_t   status = VAL_STATUS_SUCCESS;
    val_api_t *val = NULL;

    val = val_api;

    /* test init */
    val->test_init(TEST_NUM, TEST_DESC);
    val_adac_host_init();

    uint8_t *chain = NULL;
    size_t chain_size = 0;

    if (PSA_SUCCESS != val_load_certificate_chain(chain_file, &chain, &chain_size))
        goto test_fail_exit;

    psa_tlv_t *exts[MAX_EXTENSIONS];
    size_t exts_count = 0;
    uint8_t key_type;

    if (PSA_SUCCESS != val_infer_cryptosystem((uint32_t *) chain, chain_size, exts,
                                                           &exts_count, &key_type)) {
        goto test_fail_exit;
    }

    request_packet_t *request;

    if (PSA_SUCCESS != val_issue_command(SDP_DISCOVERY_CMD, request, NULL, 0))
        goto test_fail_exit;

    response_packet_t *response;

    response = val_await_response();
    if (PSA_SUCCESS != val_parse_response(SDP_DISCOVERY_CMD, response))
        goto test_fail_exit;

    if (!val_check_cryptosystem_support(response, key_type))
        goto test_fail_exit;

    response_packet_release(response);

    psa_key_handle_t handle;
    uint8_t *key = NULL;
    size_t key_size = 0;

    if (PSA_SUCCESS != val_get_private_key(key_file, &key_type, &handle, &key, &key_size))
        goto test_fail_exit;

    if (PSA_SUCCESS != val_issue_command(SDP_AUTH_START_CMD, request, NULL, 0))
        goto test_fail_exit;

    response = val_await_response();
    if (PSA_SUCCESS != val_parse_response(SDP_AUTH_START_CMD, response))
        goto test_fail_exit;

    psa_auth_challenge_t *challenge = (psa_auth_challenge_t *) response->data;

    uint8_t *token = NULL;
    size_t token_size = 0;

    if (PSA_SUCCESS != val_sign_token(challenge->challenge_vector,
                                      sizeof(challenge->challenge_vector),
                                      key_type, NULL, 0, &token, &token_size,
                                      handle, key, key_size)) {
        goto test_fail_exit;
    }
    response_packet_release(response);

    if (PSA_SUCCESS != val_send_certificate(exts, exts_count))
        goto test_fail_exit;

	val->print(PRINT_INFO, "Sending token\n", 0);
    if (PSA_SUCCESS != val_issue_command(SDP_AUTH_RESPONSE_CMD, request,
                                        (uint8_t *)token, token_size)) {
        goto test_fail_exit;
    }

	val->print(PRINT_INFO, "Receiving token_authentication response\n", 0);
    response = val_await_response();
    if (PSA_SUCCESS != val_parse_response(SDP_AUTH_RESPONSE_CMD, response))
        goto test_fail_exit;

    if (response->status == SDP_SUCCESS)
	    val->print(PRINT_INFO, "Target unlocked successfully\n", 0);
    else
        goto test_fail_exit;

    response_packet_release(response);
    goto test_end;

test_fail_exit:
	val_set_status(RESULT_FAIL(VAL_STATUS_TEST_FAILED));
test_end:
    val->test_exit();
}

