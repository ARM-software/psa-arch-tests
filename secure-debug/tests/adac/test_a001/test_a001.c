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

#include "test_a001.h"

#define TEST_NUM  VAL_CREATE_TEST_ID(VAL_SECURE_DEBUG_BASE, 1)
#define TEST_DESC "Testing ADAC Protocol Host API| UT: psa_challenge\n"
TEST_PUBLISH(TEST_NUM, test_entry);

void test_entry(val_api_t *val_api)
{
    int32_t   status = VAL_STATUS_SUCCESS;
    val_api_t *val = NULL;

    val = val_api;

    /* test init */
    val->test_init(TEST_NUM, TEST_DESC);
    val_adac_host_init();

    uint8_t challenge1[CHALLENGE_SIZE], challenge2[CHALLENGE_SIZE], i;

    request_packet_t *request;
    response_packet_t *response;

    if (PSA_SUCCESS != val_issue_command(SDP_AUTH_START_CMD, request, NULL, 0))
        goto test_fail_exit;

    response = val_await_response();
    if (PSA_SUCCESS != val_parse_response(SDP_AUTH_START_CMD, response))
        goto test_fail_exit;

    psa_auth_challenge_t *challenge = (psa_auth_challenge_t *) response->data;

    *challenge1 = (uint8_t)(challenge->challenge_vector);
    response_packet_release(response);

    if (PSA_SUCCESS != val_issue_command(SDP_AUTH_START_CMD, request, NULL, 0))
        goto test_fail_exit;

    response = val_await_response();
    if (PSA_SUCCESS != val_parse_response(SDP_AUTH_START_CMD, response))
        goto test_fail_exit;

    challenge = (psa_auth_challenge_t *) response->data;

    *challenge2 = (uint8_t)(challenge->challenge_vector);
    response_packet_release(response);

    for (i = 0; i < CHALLENGE_SIZE; i++) {
        if (challenge1[i] != challenge2[i])
            break;
    }

    if (i == CHALLENGE_SIZE) {
	    val->print(PRINT_ERROR, "Challenge response obtained is not unique\n", 0);
        goto test_fail_exit;
    } else {
	    val->print(PRINT_INFO, "Challenge response obtained is unique\n", 0);
        goto test_end;
    }

test_fail_exit:
	val_set_status(RESULT_FAIL(VAL_STATUS_TEST_FAILED));
test_end:
    val->test_exit();
}

