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

static uint32_t  public_key_registered = 0;

static inline struct q_useful_buf_c useful_buf_head(struct q_useful_buf_c buf,
                                                  size_t amount)
{
    return UsefulBuf_Head(buf, amount);
}

static uint32_t check_hash_sizes(void)
{
    if (T_COSE_CRYPTO_SHA256_SIZE != PSA_HASH_SIZE(PSA_ALG_SHA_256))
    {
        return PAL_ATTEST_HASH_FAIL;
    }

    return PAL_ATTEST_SUCCESS;
}

static psa_ecc_curve_t attest_map_elliptic_curve_type(int32_t cose_curve)
{
    psa_ecc_curve_t psa_curve;

    /*FixMe: Mapping is not complete, missing ones: P384, P521, ED25519, ED448 */
    switch (cose_curve)
    {
    case P_256:
        psa_curve = PSA_ECC_CURVE_SECP256R1;
        break;
    default:
        psa_curve = USHRT_MAX;
    }

    return psa_curve;
}

static psa_algorithm_t cose_hash_alg_id_to_psa(int32_t cose_hash_alg_id)
{
    psa_algorithm_t status;

    switch (cose_hash_alg_id)
    {
    case COSE_ALG_SHA256_PROPRIETARY:
        status = PSA_ALG_SHA_256;
        break;
    default:
        status = PSA_ALG_MD4;
        break;
    }

    return status;
}

static int32_t hash_alg_id_from_sig_alg_id(int32_t cose_sig_alg_id)
{
    switch (cose_sig_alg_id)
    {
        case COSE_ALGORITHM_ES256:
            return COSE_ALG_SHA256_PROPRIETARY;
        default:
            return INT32_MAX;
    }
}

int32_t pal_cose_crypto_hash_start(struct pal_cose_crypto_hash *hash_ctx, int32_t cose_hash_alg_id)
{
    int32_t                          cose_ret = PAL_ATTEST_SUCCESS;
    psa_status_t                     psa_ret;
    struct pal_cose_psa_crypto_hash *psa_hash_ctx;

    cose_ret = check_hash_sizes();
    if (cose_ret)
    {
        goto error;
    }

    if (sizeof(struct pal_cose_crypto_hash) < sizeof(struct pal_cose_psa_crypto_hash))
    {
        cose_ret = PAL_ATTEST_HASH_FAIL;
        goto error;
    }

    psa_hash_ctx = (struct pal_cose_psa_crypto_hash *)hash_ctx;
    psa_ret = psa_hash_setup(&psa_hash_ctx->operation, cose_hash_alg_id_to_psa(cose_hash_alg_id));

    if (psa_ret == PAL_ATTEST_SUCCESS)
    {
        psa_hash_ctx->status = PAL_ATTEST_SUCCESS;
        cose_ret = PAL_ATTEST_SUCCESS;
    }
    else if (psa_ret == PSA_ERROR_NOT_SUPPORTED)
    {
        cose_ret = PAL_ATTEST_HASH_UNSUPPORTED;
    }
    else
    {
        cose_ret = PAL_ATTEST_HASH_FAIL;
    }

error:
    return cose_ret;
}

void pal_cose_crypto_hash_update(struct pal_cose_crypto_hash *hash_ctx,
                                 struct q_useful_buf_c data_to_hash)
{
    struct pal_cose_psa_crypto_hash *psa_hash_ctx;

    if (sizeof(struct pal_cose_crypto_hash) < sizeof(struct pal_cose_psa_crypto_hash))
    {
        return;
    }

    psa_hash_ctx = (struct pal_cose_psa_crypto_hash *)hash_ctx;

    if (psa_hash_ctx->status == PAL_ATTEST_SUCCESS)
    {
        if (data_to_hash.ptr != NULL)
        {
            psa_hash_ctx->status = psa_hash_update(&psa_hash_ctx->operation,
                                                   data_to_hash.ptr,
                                                   data_to_hash.len);
        }
        else
        {
            /* Intentionally do nothing, just computing the size of the token */
        }
    }
}

int32_t pal_cose_crypto_hash_finish(struct pal_cose_crypto_hash *hash_ctx,
                                    struct q_useful_buf buffer_to_hold_result,
                                    struct q_useful_buf_c *hash_result)
{
    uint32_t                         cose_ret = PAL_ATTEST_SUCCESS;
    psa_status_t                     psa_ret;
    struct pal_cose_psa_crypto_hash *psa_hash_ctx;

    if (sizeof(struct pal_cose_crypto_hash) < sizeof(struct pal_cose_psa_crypto_hash))
    {
        cose_ret = PAL_ATTEST_HASH_FAIL;
        goto error;
    }

    psa_hash_ctx = (struct pal_cose_psa_crypto_hash *)hash_ctx;

    if (psa_hash_ctx->status == PAL_ATTEST_SUCCESS)
    {
        psa_ret = psa_hash_finish(&psa_hash_ctx->operation,
                                  buffer_to_hold_result.ptr,
                                  buffer_to_hold_result.len,
                                  &(hash_result->len));

        if (psa_ret == PAL_ATTEST_SUCCESS)
        {
            hash_result->ptr = buffer_to_hold_result.ptr;
            cose_ret = 0;
        }
        else if (psa_ret == PSA_ERROR_BUFFER_TOO_SMALL)
        {
            cose_ret = PAL_ATTEST_HASH_BUFFER_SIZE;
        }
        else
        {
            cose_ret = PAL_ATTEST_HASH_FAIL;
        }
    }
    else
    {
        cose_ret = PAL_ATTEST_HASH_FAIL;
    }

error:
    return cose_ret;
}

int pal_create_sha256(struct q_useful_buf_c bytes_to_hash, struct q_useful_buf buffer_for_hash,
                      struct q_useful_buf_c *hash)
{
    uint32_t                      status = PAL_ATTEST_SUCCESS;
    struct pal_cose_crypto_hash   hash_ctx;

    status = pal_cose_crypto_hash_start(&hash_ctx, COSE_ALG_SHA256_PROPRIETARY);
    if (status)
        return status;

    pal_cose_crypto_hash_update(&hash_ctx, bytes_to_hash);
    status = pal_cose_crypto_hash_finish(&hash_ctx, buffer_for_hash, hash);

    return status;
}

uint32_t pal_compute_hash(int32_t cose_alg_id, struct q_useful_buf buffer_for_hash,
                          struct q_useful_buf_c *hash, struct q_useful_buf_c protected_headers,
                          struct q_useful_buf_c payload)
{
    uint32_t                    status;
    QCBOREncodeContext          cbor_encode_ctx;
    struct q_useful_buf_c       tbs_first_part;
    QCBORError                  qcbor_result;
    struct pal_cose_crypto_hash hash_ctx = {{0}};
    int32_t                     hash_alg_id;
    UsefulBuf_MAKE_STACK_UB    (buffer_for_TBS_first_part, T_COSE_SIZE_OF_TBS);

    /* This builds the CBOR-format to-be-signed bytes */
    QCBOREncode_Init(&cbor_encode_ctx, buffer_for_TBS_first_part);
    QCBOREncode_OpenArray(&cbor_encode_ctx);
    /* context */
    QCBOREncode_AddSZString(&cbor_encode_ctx,
                            COSE_SIG_CONTEXT_STRING_SIGNATURE1);
    /* body_protected */
    QCBOREncode_AddBytes(&cbor_encode_ctx,
                         protected_headers);
    /* sign_protected */
    QCBOREncode_AddBytes(&cbor_encode_ctx, NULL_USEFUL_BUF_C);
    /* external_aad */
    QCBOREncode_AddBytes(&cbor_encode_ctx, NULL_USEFUL_BUF_C);
    /* fake payload */
    QCBOREncode_AddBytes(&cbor_encode_ctx, NULL_USEFUL_BUF_C);
    QCBOREncode_CloseArray(&cbor_encode_ctx);

    /* Get the result and convert it to struct q_useful_buf_c representation */
    qcbor_result = QCBOREncode_Finish(&cbor_encode_ctx, &tbs_first_part);
    if (qcbor_result)
    {
        /* Mainly means that the protected_headers were too big
         (which should never happen) */
        status = PAL_ATTEST_ERR_SIGN_STRUCT;
        goto Done;
    }

    /* Start the hashing */
    hash_alg_id = hash_alg_id_from_sig_alg_id(cose_alg_id);

    /* Don't check hash_alg_id for failure. pal_cose_crypto_hash_start()
     * will handle it properly
     */
    status = pal_cose_crypto_hash_start(&hash_ctx, hash_alg_id);
    if (status)
        goto Done;

    /* Hash the first part of the TBS. Take all but the last two
     * bytes. The last two bytes are the fake payload from above. It
     * is replaced by the real payload which is hashed next. The fake
     * payload is needed so the array count is right. This is one of
     * the main things that make it possible to implement with one
     * buffer for the whole cose sign1.
     */
    pal_cose_crypto_hash_update(&hash_ctx, useful_buf_head(tbs_first_part,
                                              tbs_first_part.len - 2));

    /* Hash the payload */
    pal_cose_crypto_hash_update(&hash_ctx, payload);

    /* Finish the hash and set up to return it */
    status = pal_cose_crypto_hash_finish(&hash_ctx,
                                             buffer_for_hash,
                                             hash);

Done:
    return status;
}

uint32_t pal_import_attest_key(int32_t alg)
{
    psa_key_type_t   attest_key_type;
    size_t           public_key_size;
    psa_status_t     status = PSA_SUCCESS;
    psa_key_policy_t policy;
    psa_ecc_curve_t  psa_curve;
    psa_key_handle_t public_key_handle;

    /* Mapping of COSE curve type to PSA curve types */
    psa_curve = attest_map_elliptic_curve_type(P_256);
    if (psa_curve == USHRT_MAX)
        return PAL_ATTEST_ERROR;

    /* Setup the key policy for public key */
    policy = psa_key_policy_init();
    psa_key_policy_set_usage(&policy, PSA_KEY_USAGE_VERIFY, alg);

    status = psa_allocate_key(&public_key_handle);
    if (status != PSA_SUCCESS)
        return status;

    status = psa_set_key_policy(public_key_handle, &policy);
    if (status != PSA_SUCCESS)
        return status;

    attest_key_type = PSA_KEY_TYPE_ECC_PUBLIC_KEY(psa_curve);

    /* Register public key to crypto service */
    public_key_size = attest_key.pubx_key_size + attest_key.puby_key_size;

    status = psa_import_key(public_key_handle,
                                attest_key_type,
                                (const uint8_t *)&attest_public_key,
                                public_key_size + 1);

    return status;
}


uint32_t pal_crypto_pub_key_verify(int32_t cose_algorithm_id,
                                   struct q_useful_buf_c token_hash,
                                   struct q_useful_buf_c signature)
{
    uint32_t status = PAL_ATTEST_SUCCESS;

    if (!public_key_registered)
    {
        status = pal_import_attest_key(cose_algorithm_id);
        if (status != PAL_ATTEST_SUCCESS)
            return status;

        public_key_registered = 1;
    }

/*
 * Enable the verify function when Trusted Firmare - M Supports

 *  Verify the signature a hash or short message using a public key.
    status = psa_asymmetric_verify(public_key_handle,
                                    cose_algorithm_id, token_hash.ptr, token_hash.len,
                                    signature.ptr, signature.len);
*/
    return status;
}
