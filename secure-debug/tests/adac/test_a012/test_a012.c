/** @file
 * Copyright (c) 2023 Arm Limited or its affiliates. All rights reserved.
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
#include <adac_util.h>
#include "val_interfaces.h"

#include "test_a012.h"

#define TEST_NUM  VAL_CREATE_TEST_ID(VAL_SECURE_DEBUG_BASE, 12)
#define TEST_DESC "Testing ADAC Protocol Host API| UT: psa_lcs_change_command\n"
TEST_PUBLISH(TEST_NUM, test_entry);

void test_entry(val_api_t *val_api)
{
    psa_status_t ret;
    val_api_t *val = val_api;

    /* test init */
    val->test_init(TEST_NUM, TEST_DESC);
    if (!IS_TEST_START(val->get_status()))
    {
        goto test_end;
    }
    psa_adac_host_init();

    request_packet_t *request;
    response_packet_t *response;

    psa_tlv_t *tlv = NULL, *lifecycle_tlv = NULL, **tlv_seq = NULL;
    uint8_t *tlv_ptr;
    size_t i, j, tlv_size;

    // Send discovery packet with no requested type ID.
    ret = psa_adac_issue_command(ADAC_DISCOVERY_CMD, request, NULL, 0);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(1), VAL_STATUS_WRITE_FAILED);
        goto test_end;
    }

    response = psa_adac_await_response();
    if (response == NULL) {
        val->err_check_set(TEST_CHECKPOINT_NUM(2), VAL_STATUS_CONNECTION_FAILED);
        goto test_end;
    }

    // Variable to collect the reference to each TLV in the response sequence.
    tlv_seq = (psa_tlv_t **)malloc((response->data_count) * sizeof(psa_tlv_t *));
    if (tlv_seq == NULL) {
        val->err_check_set(TEST_CHECKPOINT_NUM(3), VAL_STATUS_INSUFFICIENT_SIZE);
        goto test_end;
    }

    // Parse the TLV sequence and obtain tlv with type_id as psa_lifecycle.
    for (i = 0, j = 0; (i + 4) < (response->data_count * 4); j++) {
        tlv = (psa_tlv_t *) (((uint8_t *)response->data) + i);

        if (tlv->type_id == PSA_LIFECYCLE) {
            tlv_seq[j] = tlv;
            tlv_ptr = (uint8_t *)(tlv);
            tlv_size = sizeof(psa_tlv_t) + ROUND_TO_WORD(tlv->length_in_bytes);
        }
        i += sizeof(psa_tlv_t) + ROUND_TO_WORD(tlv->length_in_bytes);
    }

    response_packet_release(response);

    // Send ADAC LCS command with empty TLV list
    ret = psa_adac_issue_command(ADAC_LCS_CHANGE_CMD, request, NULL, 0);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(4), VAL_STATUS_WRITE_FAILED);
        goto test_end;
    }

    response = psa_adac_await_response();
    ret = psa_adac_parse_response(ADAC_LCS_CHANGE_CMD, response);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(2), VAL_STATUS_READ_FAILED);
        goto test_end;
    }

    // Target must recognize LCS command
    if (response->status == ADAC_INVALID_COMMAND) {
        val->err_check_set(TEST_CHECKPOINT_NUM(3), VAL_STATUS_INVALID);
        goto test_end;
    }

    if ((response->status != ADAC_UNSUPPORTED) && (response->status != ADAC_INVALID_PARAMETERS)) {
        val->err_check_set(TEST_CHECKPOINT_NUM(3), VAL_STATUS_INVALID);
        goto test_end;
    }
    response_packet_release(response);

    // Send ADAC LCS command with TLV sequence
    ret = psa_adac_issue_command(ADAC_LCS_CHANGE_CMD, request, tlv_ptr, tlv_size);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(4), VAL_STATUS_WRITE_FAILED);
        goto test_end;
    }

    response = psa_adac_await_response();
    ret = psa_adac_parse_response(ADAC_LCS_CHANGE_CMD, response);
    if (ret != PSA_SUCCESS) {
        val->err_check_set(TEST_CHECKPOINT_NUM(2), VAL_STATUS_READ_FAILED);
        goto test_end;
    }

    // Target must recognize LCS command
    if (response->status == ADAC_INVALID_COMMAND) {
        val->err_check_set(TEST_CHECKPOINT_NUM(3), VAL_STATUS_INVALID);
        goto test_end;
    }
    response_packet_release(response);

test_end:
    if (tlv_seq != NULL)
        free(tlv_seq);

    val->test_exit();
}
