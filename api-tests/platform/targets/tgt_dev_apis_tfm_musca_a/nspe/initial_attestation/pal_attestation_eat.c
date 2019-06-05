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

#include "pal_attestation_crypto.h"

uint32_t    mandatory_claims = 0;
uint32_t    mandaroty_sw_components = 0;
bool_t      sw_component_present = 0;

static int pal_encode_cose_key(struct q_useful_buf_c *cose_key,
                               struct q_useful_buf buffer_for_cose_key,
                               struct q_useful_buf_c x_cord, struct q_useful_buf_c y_cord)
{
    uint32_t                  return_value;
    QCBORError                qcbor_result;
    QCBOREncodeContext        cbor_encode_ctx;
    int32_t                   cose_curve_id = P_256;
    struct q_useful_buf_c       encoded_key_id;

    /* Get the public key x and y */
    /* Encode it into a COSE_Key structure */
    QCBOREncode_Init(&cbor_encode_ctx, buffer_for_cose_key);
    QCBOREncode_OpenMap(&cbor_encode_ctx);
    QCBOREncode_AddInt64ToMapN(&cbor_encode_ctx,
                               COSE_KEY_COMMON_KTY,
                               COSE_KEY_TYPE_EC2);
    QCBOREncode_AddInt64ToMapN(&cbor_encode_ctx,
                               COSE_KEY_PARAM_CRV,
                               cose_curve_id);
    QCBOREncode_AddBytesToMapN(&cbor_encode_ctx,
                               COSE_KEY_PARAM_X_COORDINATE,
                               x_cord);
    QCBOREncode_AddBytesToMapN(&cbor_encode_ctx,
                               COSE_KEY_PARAM_Y_COORDINATE,
                               y_cord);
    QCBOREncode_CloseMap(&cbor_encode_ctx);

    qcbor_result = QCBOREncode_Finish(&cbor_encode_ctx, &encoded_key_id);
    if (qcbor_result != QCBOR_SUCCESS)
    {
        /* Mainly means that the COSE_Key was too big for buffer_for_cose_key */
        return_value = PAL_ATTEST_ERR_PROTECTED_HEADERS;
        goto Done;
    }

    /* Finish up and return */
    *cose_key = encoded_key_id;
    return_value = PAL_ATTEST_SUCCESS;

Done:
    return return_value;
}


static int get_items_in_map(QCBORDecodeContext *decode_context,
                            struct items_to_get_t *item_list)
{
    int                     item_index;
    QCBORItem               item;
    struct items_to_get_t  *item_ptr = item_list;

    /* initialize the data type of all items in the list */
    while (item_ptr->label != 0)
    {
        item_ptr->item.uDataType = QCBOR_TYPE_NONE;
        item_ptr++;
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

        item_ptr = item_list;
        while (item_ptr->label != 0)
        {
            if (item.label.int64 == item_ptr->label)
            {
                item_ptr->item = item;
            }
            item_ptr++;
        }
    }

    return PAL_ATTEST_SUCCESS;
}

static int get_item_in_map(QCBORDecodeContext *decode_context,
                           int32_t label,
                           QCBORItem *item)
{
    struct items_to_get_t   item_list[2];

    item_list[0].label = label;
    item_list[1].label = 0;

    if (get_items_in_map(decode_context, item_list))
    {
        return PAL_ATTEST_ERROR;
    }

    if (item_list[0].item.uDataType == QCBOR_TYPE_NONE)
    {
        return PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
    }

    *item = item_list[0].item;

    return PAL_ATTEST_SUCCESS;
}

static int parse_unprotected_headers(QCBORDecodeContext *decode_context,
                                     struct q_useful_buf_c *child,
                                     bool *loop_back)
{
    struct items_to_get_t   item_list[3];

    item_list[0].label = COSE_HEADER_PARAM_KID;
    item_list[1].label = T_COSE_SHORT_CIRCUIT_LABEL;
    item_list[2].label = 0;
    *loop_back = false;

    if (get_items_in_map(decode_context, item_list))
    {
        return PAL_ATTEST_ERROR;
    }

    if (item_list[1].item.uDataType == QCBOR_TYPE_TRUE)
    {
        *loop_back = true;
    }

    if (item_list[0].item.uDataType != QCBOR_TYPE_BYTE_STRING)
    {
        return PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
    }

    *child = item_list[0].item.val.string;

    return PAL_ATTEST_SUCCESS;
}

static int parse_protected_headers(struct q_useful_buf_c protected_headers,
                                   int32_t *alg_id)
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

    if ((item.uDataType != QCBOR_TYPE_INT64) || (item.val.int64 > INT32_MAX))
    {
        return PAL_ATTEST_ERROR;
    }

    *alg_id = (int32_t)item.val.int64;

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
                                   struct q_useful_buf_c completed_challenge)
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
                for (i = 0; i <= count; i++)
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

                    if (i < count)
                    {
                        status = QCBORDecode_GetNext(decode_context, &item);
                        if (status != PAL_ATTEST_SUCCESS)
                            return PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING;
                    }
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
    int32_t             status = PAL_ATTEST_SUCCESS;
    bool                short_circuit;
    int32_t             cose_algorithm_id;
    QCBORItem           item;
    QCBORDecodeContext  decode_context;
    struct q_useful_buf_c completed_challenge;
    struct q_useful_buf_c completed_token;
    struct q_useful_buf_c payload;
    struct q_useful_buf_c signature;
    struct q_useful_buf_c protected_headers;
    struct q_useful_buf_c kid;
    struct q_useful_buf_c x_cord;
    struct q_useful_buf_c y_cord;
    struct q_useful_buf_c cose_key_to_hash;
    struct q_useful_buf_c key_hash;
    struct q_useful_buf_c token_hash;
    USEFUL_BUF_MAKE_STACK_UB(buf_to_hold_x_coord, T_COSE_CRYPTO_EC_P256_COORD_SIZE);
    USEFUL_BUF_MAKE_STACK_UB(buf_to_hold_y_coord, T_COSE_CRYPTO_EC_P256_COORD_SIZE);
    USEFUL_BUF_MAKE_STACK_UB(buffer_for_kid, T_COSE_CRYPTO_SHA256_SIZE);
    USEFUL_BUF_MAKE_STACK_UB(buffer_for_cose_key, MAX_ENCODED_COSE_KEY_SIZE);
    USEFUL_BUF_MAKE_STACK_UB(buffer_for_encoded_key, MAX_ENCODED_COSE_KEY_SIZE);
    USEFUL_BUF_MAKE_STACK_UB(buffer_for_token_hash, T_COSE_CRYPTO_SHA256_SIZE);

    kid.ptr = buffer_for_encoded_key.ptr;

    memcpy(buf_to_hold_x_coord.ptr, (const void *)attest_key.pubx_key, attest_key.pubx_key_size);
    memcpy(buf_to_hold_y_coord.ptr, (const void *)attest_key.puby_key, attest_key.puby_key_size);

    /* Update size */
    buf_to_hold_x_coord.len = attest_key.pubx_key_size;
    buf_to_hold_y_coord.len = attest_key.puby_key_size;

    x_cord.ptr = buf_to_hold_x_coord.ptr;
    x_cord.len = buf_to_hold_x_coord.len;
    y_cord.ptr = buf_to_hold_y_coord.ptr;
    y_cord.len = buf_to_hold_y_coord.len;

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

    /* Encode the given public key */
    status = pal_encode_cose_key(&cose_key_to_hash, buffer_for_cose_key, x_cord, y_cord);
    if (status != PAL_ATTEST_SUCCESS)
        return status;

    /* Create hash of the given public key */
    status = pal_create_sha256(cose_key_to_hash, buffer_for_kid, &key_hash);
    if (status != PSA_SUCCESS)
        return status;

    /* Compare the hash of the public key in token and hash of the given public key */
    if (kid.len != key_hash.len)
    {
        return PAL_ATTEST_HASH_LENGTH_MISMATCH;
    }

    if (memcmp(kid.ptr, key_hash.ptr, kid.len) != 0)
    {
        return PAL_ATTEST_HASH_MISMATCH;
    }

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

    /* Compute the hash from the token */
    status = pal_compute_hash(cose_algorithm_id, buffer_for_token_hash, &token_hash,
                              protected_headers, payload);
    if (status != PAL_ATTEST_SUCCESS)
        return status;

    /* Verify the signature */
    status = pal_crypto_pub_key_verify(cose_algorithm_id, token_hash, signature);
    if (status != PAL_ATTEST_SUCCESS)
        return status;

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

    if ((mandatory_claims & MANDATORY_CLAIM_WITH_SW_COMP) == MANDATORY_CLAIM_WITH_SW_COMP)
    {
        if ((mandaroty_sw_components & MANDATORY_SW_COMP) != MANDATORY_SW_COMP)
            return PAL_ATTEST_TOKEN_NOT_ALL_MANDATORY_CLAIMS;
    }
    else if ((mandatory_claims & MANDATORY_CLAIM_NO_SW_COMP) != MANDATORY_CLAIM_NO_SW_COMP)
    {
        return PAL_ATTEST_TOKEN_NOT_ALL_MANDATORY_CLAIMS;
    }

    return PAL_ATTEST_SUCCESS;
}
