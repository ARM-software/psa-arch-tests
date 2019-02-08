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

#include "pal_attestation_eat.h"

uint32_t  mandatory_claims = 0, mandaroty_sw_components = 0;
bool_t    sw_component_present = 0;

static int get_items_in_map(QCBORDecodeContext *decode_context,
                            struct items_to_get_t *items)
{
    int                     item_index;
    QCBORItem               item;
    struct items_to_get_t  *item_iterator;

    for (item_iterator = items; item_iterator->label != 0; item_iterator++)
    {
        item_iterator->item.uDataType = QCBOR_TYPE_NONE;
    }

    QCBORDecode_GetNext(decode_context, &item);
    if (item.uDataType != QCBOR_TYPE_MAP)
    {
        return PAL_ATTEST_ERROR;
    }

    for (item_index = item.val.uCount; item_index != 0; item_index--)
    {
        if (QCBORDecode_GetNext(decode_context, &item) != QCBOR_SUCCESS)
        {
            return PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
        }
        if (item.uLabelType != QCBOR_TYPE_INT64)
        {
            continue;
        }

        for (item_iterator = items; item_iterator->label != 0; item_iterator++)
        {
            if (item.label.int64 == item_iterator->label)
            {
                item_iterator->item = item;
            }
        }
    }

    return PAL_ATTEST_SUCCESS;
}

static int get_item_in_map(QCBORDecodeContext *decode_context,
                           int32_t label,
                           QCBORItem *item)
{
    struct items_to_get_t   one_item[2];
    int                     return_value;

    one_item[0].label = label;
    one_item[1].label = 0;

    return_value = get_items_in_map(decode_context, one_item);
    if (return_value) {
        goto Done;
    }

    if (one_item[0].item.uDataType == QCBOR_TYPE_NONE) {
        return_value = PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
        goto Done;
    }

    *item = one_item[0].item;

Done:
    return return_value;
}

static int parse_unprotected_headers(QCBORDecodeContext *decode_context,
                                     struct useful_buf_c *kid,
                                     bool *short_circut)
{
    struct items_to_get_t   items[3];
    int                     return_value;

    items[0].label = COSE_HEADER_PARAM_KID;
    items[1].label = T_COSE_SHORT_CIRCUIT_LABEL;
    items[2].label = 0;

    return_value = get_items_in_map(decode_context, items);
    if (return_value)
    {
        goto Done;
    }

    *short_circut = false;
    if (items[1].item.uDataType == QCBOR_TYPE_TRUE)
    {
        *short_circut = true;
    }

    if (items[0].item.uDataType != QCBOR_TYPE_BYTE_STRING)
    {
        return_value = PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
        goto Done;
    }

    *kid = items[0].item.val.string;

Done:
    return return_value;
}

static int parse_protected_headers(struct useful_buf_c protected_headers,
                                   int32_t *algorithm_id)
{
    QCBORDecodeContext  decode_context;
    QCBORItem           item;

    QCBORDecode_Init(&decode_context, protected_headers, 0);

    if (get_item_in_map(&decode_context, COSE_HEADER_PARAM_ALG, &item))
    {
        return PAL_ATTEST_ERROR;
    }

    if (QCBORDecode_Finish(&decode_context))
    {
        return PAL_ATTEST_ERROR;
    }

    if (item.uDataType != QCBOR_TYPE_INT64 || item.val.int64 > INT32_MAX)
    {
        return PAL_ATTEST_ERROR;
    }

    *algorithm_id = (int32_t)item.val.int64;

    return PAL_ATTEST_SUCCESS;
}

/**
    @brief    - This API will verify the claims
    @param    - decode_context      : The buffer containing the challenge
                item                : context for decoding the data items
                completed_challenge : Buffer containing the challenge
    @return   - error status
**/
static int parse_claims(QCBORDecodeContext *decode_context, QCBORItem item,
                                   struct useful_buf_c completed_challenge)
{
    int i, count = 0;
    int status = PAL_ATTEST_SUCCESS;

    /* Parse each claim and validate their data type */
    while (status == PAL_ATTEST_SUCCESS)
    {
        status = QCBORDecode_GetNext(decode_context, &item);
        if (status != PAL_ATTEST_SUCCESS)
            break;

        mandatory_claims |= 1 << (EAT_CBOR_ARM_RANGE_BASE - item.label.int64);
        if (item.uLabelType == QCBOR_TYPE_INT64)
        {
            if (item.label.int64 == EAT_CBOR_ARM_LABEL_NONCE)
            {
                if (item.uDataType == QCBOR_TYPE_BYTE_STRING)
                {
                    /* Given challenge vs challenge in token */
                    if (UsefulBuf_Compare(item.val.string, completed_challenge))
                        return PAL_ATTEST_TOKEN_CHALLENGE_MISMATCH;
                }
                else
                    return PAL_ATTEST_TOKEN_NOT_SUPPORTED;
            }
            else if (item.label.int64 == EAT_CBOR_ARM_LABEL_BOOT_SEED ||
                     item.label.int64 == EAT_CBOR_ARM_LABEL_IMPLEMENTATION_ID ||
                     item.label.int64 == EAT_CBOR_ARM_LABEL_UEID)
            {
                if (item.uDataType != QCBOR_TYPE_BYTE_STRING)
                    return PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
            }
            else if (item.label.int64 == EAT_CBOR_ARM_LABEL_ORIGINATION ||
                     item.label.int64 == EAT_CBOR_ARM_LABEL_PROFILE_DEFINITION ||
                     item.label.int64 == EAT_CBOR_ARM_LABEL_HW_VERSION)
            {
                if (item.uDataType != QCBOR_TYPE_TEXT_STRING)
                    return PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
            }
            else if (item.label.int64 == EAT_CBOR_ARM_LABEL_CLIENT_ID ||
                     item.label.int64 == EAT_CBOR_ARM_LABEL_SECURITY_LIFECYCLE)
            {
                if (item.uDataType != QCBOR_TYPE_INT64)
                    return PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
            }
            else if (item.label.int64 == EAT_CBOR_ARM_LABEL_SW_COMPONENTS)
            {
                if (item.uDataType != QCBOR_TYPE_ARRAY)
                    return PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;

                sw_component_present = 1;
                status = QCBORDecode_GetNext(decode_context, &item);
                if (status != PAL_ATTEST_SUCCESS)
                    continue;

                count = item.val.uCount;
                for (i = 0; i < count; i++)
                {
                    mandaroty_sw_components |= 1 << item.label.int64;

                    if (item.label.int64 == EAT_CBOR_SW_COMPONENT_MEASUREMENT)
                    {
                         if (item.uDataType != QCBOR_TYPE_BYTE_STRING)
                            return PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
                    }
                    else if (item.label.int64 == EAT_CBOR_SW_COMPONENT_MEASUREMENT_DESC)
                    {
                        if (item.uDataType != QCBOR_TYPE_TEXT_STRING)
                            return PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
                    }
                    else if (item.label.int64 == EAT_CBOR_SW_COMPONENT_VERSION)
                    {
                        if (item.uDataType != QCBOR_TYPE_TEXT_STRING)
                            return PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
                    }
                    else if (item.label.int64 == EAT_CBOR_SW_COMPONENT_SIGNER_ID)
                    {
                        if (item.uDataType != QCBOR_TYPE_BYTE_STRING)
                            return PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
                    }
                    else if (item.label.int64 == EAT_CBOR_SW_COMPONENT_EPOCH)
                    {
                        if (item.uDataType != QCBOR_TYPE_INT64)
                            return PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
                    }
                    else if (item.label.int64 == EAT_CBOR_SW_COMPONENT_TYPE)
                    {
                        if (item.uDataType != QCBOR_TYPE_TEXT_STRING)
                            return PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
                    }

                    status = QCBORDecode_GetNext(decode_context, &item);
                    if (status != PAL_ATTEST_SUCCESS)
                        return PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
                }

            }
        }
        else
        {
            /* ToDo: Add other claim types */
        }
    }

    if (status == QCBOR_ERR_HIT_END)
        return PAL_ATTEST_SUCCESS;
    else
        return PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
}

/**
    @brief    - This API will verify the attestation token
    @param    - challenge       : The buffer containing the challenge
                challenge_size  : Size of the challenge buffer
                token           : The buffer containing the attestation token
                token_size      : Size of the token buffer
    @return   - error status
**/
int32_t pal_initial_attest_verify_token(uint8_t *challenge, uint32_t challenge_size,
                                        uint8_t *token, uint32_t token_size)
{
    int                 status = PAL_ATTEST_SUCCESS;
    bool                short_circuit;
    int32_t             cose_algorithm_id;
    QCBORItem           item;
    QCBORDecodeContext  decode_context;
    struct useful_buf_c completed_challenge;
    struct useful_buf_c completed_token;
    struct useful_buf_c payload;
    struct useful_buf_c protected_headers;
    struct useful_buf_c signature;
    struct useful_buf_c kid;

    /* Construct the token buffer for validation */
    completed_token.ptr = token;
    completed_token.len = token_size;

    /* Construct the challenge buffer for validation */
    completed_challenge.ptr = challenge;
    completed_challenge.len = challenge_size;

/*
    -------------------------
    |  CBOR Array Type      |
    -------------------------
    |  Protected Headers    |
    -------------------------
    |  Unprotected Headers  |
    -------------------------
    |  Payload              |
    -------------------------
    |  Signature            |
    -------------------------
*/

    /* Initialize the decorder */
    QCBORDecode_Init(&decode_context, completed_token, QCBOR_DECODE_MODE_NORMAL);

    /* Get the Header */
    QCBORDecode_GetNext(&decode_context, &item);

    /* Check the CBOR Array type. Check if the count is 4.
     * Only COSE_SIGN1 is supported now.
     */
    if (item.uDataType != QCBOR_TYPE_ARRAY || item.val.uCount != 4 ||
       !QCBORDecode_IsTagged(&decode_context, &item, CBOR_TAG_COSE_SIGN1))
        return PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;

    /* Get the next headers */
    QCBORDecode_GetNext(&decode_context, &item);
    if (item.uDataType != QCBOR_TYPE_BYTE_STRING)
        return PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;

    protected_headers = item.val.string;

    /* Parse the protected headers and check the data type and value*/
    status = parse_protected_headers(protected_headers, &cose_algorithm_id);
    if (status != PAL_ATTEST_SUCCESS)
        return status;

    /* Parse the unprotected headers and check the data type and value */
    short_circuit = false;
    status = parse_unprotected_headers(&decode_context, &kid, &short_circuit);
    if (status != PAL_ATTEST_SUCCESS)
        return status;

    /* Get the payload */
    QCBORDecode_GetNext(&decode_context, &item);
    if (item.uDataType != QCBOR_TYPE_BYTE_STRING)
        return PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;

    payload = item.val.string;

    /* Get the digital signature */
    QCBORDecode_GetNext(&decode_context, &item);
    if (item.uDataType != QCBOR_TYPE_BYTE_STRING)
        return PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;

    signature = item.val.string;

/* ToDo: Compute hash and validate the signature
    //Compute the hash from the token
    status = pal_compute_hash(cose_algorithm_id,
                              buffer,
                              final_hash,
                              protected_headers,
                              payload);
    if (status != PAL_ATTEST_SUCCESS)
        return status;

    //Verify the signature
    status = psa_asymmetric_verify(kid,
                                   0,
                                   final_hash.ptr,
                                   final_hash.len,
                                   0,
                                   NULL,
                                   signature.ptr,
                                   signature.len,
                                   &len);
    if (status != PAL_ATTEST_SUCCESS)
        return status;
*/

    /* Initialize the Decoder and validate the payload format */
    QCBORDecode_Init(&decode_context, payload, QCBOR_DECODE_MODE_NORMAL);
    status = QCBORDecode_GetNext(&decode_context, &item);
    if (status != PAL_ATTEST_SUCCESS)
        return status;

    if (item.uDataType != QCBOR_TYPE_MAP)
        return PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;

    /* Parse the payload and check the data type of each claim */
    status = parse_claims(&decode_context, item, completed_challenge);
    if (status != PAL_ATTEST_SUCCESS)
        return status;

    if (((mandatory_claims & CLAIM_VALUE) != CLAIM_VALUE) &&
       (mandatory_claims != (1 << (EAT_CBOR_ARM_RANGE_BASE - EAT_CBOR_ARM_LABEL_NO_SW_COMPONENTS))))
        return PAL_ATTEST_TOKEN_NOT_ALL_MANDATORY_CLAIMS;

    if (sw_component_present && ((mandaroty_sw_components & SW_CLAIM_VALUE) != SW_CLAIM_VALUE))
        return PAL_ATTEST_TOKEN_NOT_ALL_MANDATORY_CLAIMS;

    return PAL_ATTEST_SUCCESS;
}
