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

static uint32_t         public_key_registered;
static psa_key_handle_t public_key_handle;
static inline struct q_useful_buf_c useful_buf_head(struct q_useful_buf_c buf,
                                                  size_t amount)
{
    return UsefulBuf_Head(buf, amount);
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
    psa_hash_operation_t psa_hash = PSA_HASH_OPERATION_INIT;
    psa_algorithm_t      psa_alg;

    /* Map the algorithm ID */
    psa_alg = cose_hash_alg_id_to_psa(cose_hash_alg_id);

    /* Actually do the hash set up */
    hash_ctx->status = psa_hash_setup(&psa_hash, psa_alg);

    /* Copy the PSA handle back into the context */
    hash_ctx->context.handle = psa_hash.handle;

    /* Map errors and return */
    return (psa_status_t)hash_ctx->status;
}

void pal_cose_crypto_hash_update(struct pal_cose_crypto_hash *hash_ctx,
                                 struct q_useful_buf_c data_to_hash)
{
    psa_hash_operation_t psa_hash;

    /* Copy the PSA handle out of the generic context */
    psa_hash.handle = (uint32_t)hash_ctx->context.handle;

    if (hash_ctx->status != PSA_SUCCESS)
        return;

    if (data_to_hash.ptr == NULL) {
        /* No data was passed in to be hashed indicating the mode of use is
         * the computation of the size of hash. This mode is hashing is used
         * by t_cose when it is requested to compute the size of the signed
         * data it might compute, which is in turn used to compute the
         * size of a would be token. When computing the size, the size
         * like this, there is nothing to do in update()
         */
        return;
    }

    /* Actually hash the data */
    hash_ctx->status = psa_hash_update(&psa_hash, data_to_hash.ptr, data_to_hash.len);

    /* Copy the PSA handle back into the context. */
    hash_ctx->context.handle = psa_hash.handle;
}

int32_t pal_cose_crypto_hash_finish(struct pal_cose_crypto_hash *hash_ctx,
                                    struct q_useful_buf buffer_to_hold_result,
                                    struct q_useful_buf_c *hash_result)
{
    psa_hash_operation_t psa_hash;

    /* Copy the PSA handle out of the generic context */
    psa_hash.handle = (uint32_t)hash_ctx->context.handle;

    if (hash_ctx->status != PSA_SUCCESS)
        goto Done;

    /* Actually finish up the hash */
    hash_ctx->status = psa_hash_finish(&psa_hash, buffer_to_hold_result.ptr,
                                       buffer_to_hold_result.len, &(hash_result->len));

    hash_result->ptr = buffer_to_hold_result.ptr;

    /* Copy the PSA handle back into the context. */
    hash_ctx->context.handle = psa_hash.handle;

Done:
    return ((psa_status_t)hash_ctx->status);

}

int pal_create_sha256(struct q_useful_buf_c bytes_to_hash, struct q_useful_buf buffer_for_hash,
                      struct q_useful_buf_c *hash)
{
    uint32_t                      return_value = 0;
    struct pal_cose_crypto_hash   hash_ctx;

    return_value = pal_cose_crypto_hash_start(&hash_ctx, COSE_ALG_SHA256_PROPRIETARY);
    if (return_value)
        goto Done;

    pal_cose_crypto_hash_update(&hash_ctx, bytes_to_hash);
    return_value = pal_cose_crypto_hash_finish(&hash_ctx,
                                             buffer_for_hash,
                                             hash);

Done:
    return return_value;
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
    QCBOREncode_AddSZString(&cbor_encode_ctx, COSE_SIG_CONTEXT_STRING_SIGNATURE1);

    /* body_protected */
    QCBOREncode_AddBytes(&cbor_encode_ctx, protected_headers);

    /* sign_protected is not used for Sign1 */
    /* external_aad */
    QCBOREncode_AddBytes(&cbor_encode_ctx, NULL_USEFUL_BUF_C);

    /* The short fake payload. */
    QCBOREncode_AddBytesLenOnly(&cbor_encode_ctx, payload);
    QCBOREncode_CloseArray(&cbor_encode_ctx);

    /* Get the result and convert it to struct q_useful_buf_c representation */
    qcbor_result = QCBOREncode_Finish(&cbor_encode_ctx, &tbs_first_part);
    if (qcbor_result)
    {
        /* Mainly means that the protected_headers were too big
         * (which should never happen)
         */
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

    /* This is hashing of the first part, all the CBOR
     * except the payload.
     */
    pal_cose_crypto_hash_update(&hash_ctx, useful_buf_head(tbs_first_part, tbs_first_part.len));

    /* Hash the payload, the second part. */
    pal_cose_crypto_hash_update(&hash_ctx, payload);

    /* Finish the hash and set up to return it */
    status = pal_cose_crypto_hash_finish(&hash_ctx, buffer_for_hash, hash);

Done:
    return status;
}

static uint32_t pal_import_attest_key(psa_algorithm_t key_alg)
{
    psa_status_t     status             = PAL_ATTEST_ERROR;
    psa_key_usage_t  usage              = PSA_KEY_USAGE_VERIFY;
    psa_ecc_curve_t  psa_curve          = attest_map_elliptic_curve_type(P_256);
    psa_key_type_t   attest_key_type    = PSA_KEY_TYPE_ECC_PUBLIC_KEY(psa_curve);
    size_t           public_key_size    = attest_key.pubx_key_size + attest_key.puby_key_size;

#if defined(CRYPTO_VERSION_BETA1) || defined(CRYPTO_VERSION_BETA2)
    psa_key_policy_t policy;

    if (!public_key_registered)
    {
        if (psa_curve == USHRT_MAX)
            return PAL_ATTEST_ERROR;

        /* Setup the key policy for public key */
        policy = psa_key_policy_init();
        psa_key_policy_set_usage(&policy, usage, key_alg);

        status = psa_allocate_key(&public_key_handle);
        if (status != PSA_SUCCESS)
            return PAL_ATTEST_ERR_KEY_FAIL;

        status = psa_set_key_policy(public_key_handle, &policy);
        if (status != PSA_SUCCESS)
            return PAL_ATTEST_ERR_KEY_FAIL;

        /* Import the public key */
        status = psa_import_key(public_key_handle,
                                attest_key_type,
                               (const uint8_t *)&attest_public_key,
                                public_key_size + 1);
        if (status != PSA_SUCCESS)
            return PAL_ATTEST_ERR_KEY_FAIL;

        public_key_registered = 1;
    }

#elif defined(CRYPTO_VERSION_BETA3)
    psa_key_attributes_t  attributes = PSA_KEY_ATTRIBUTES_INIT;

    if (!public_key_registered)
    {
        if (psa_curve == USHRT_MAX)
            return PAL_ATTEST_ERROR;

        /* Set the attributes for the public key */
        psa_set_key_type(&attributes, attest_key_type);
        psa_set_key_bits(&attributes, public_key_size + 1);
        psa_set_key_usage_flags(&attributes, usage);
        psa_set_key_algorithm(&attributes, key_alg);

        /* Import the public key */
        status = psa_import_key(&attributes,
                               (const uint8_t *)&attest_public_key,
                                public_key_size + 1,
                                &public_key_handle);

        if (status != PSA_SUCCESS)
            return PAL_ATTEST_ERR_KEY_FAIL;

        public_key_registered = 1;
    }
#endif

    return status;
}

static uint32_t pal_destroy_attest_key(void)
{
    psa_status_t     status;

    if (!public_key_registered)
        return PAL_ATTEST_ERR_KEY_FAIL;

    status =  psa_destroy_key(public_key_handle);
    if (status != PSA_SUCCESS)
        return PAL_ATTEST_ERR_KEY_FAIL;

    public_key_registered = 0;

    return PAL_ATTEST_SUCCESS;
}

uint32_t pal_crypto_pub_key_verify(int32_t cose_algorithm_id,
                                   struct q_useful_buf_c token_hash,
                                   struct q_useful_buf_c signature)
{
    int32_t status = PAL_ATTEST_ERROR;
    psa_algorithm_t key_alg = PSA_ALG_ECDSA(PSA_ALG_SHA_256);

    /* Register the attestation public key */
    status = pal_import_attest_key(key_alg);
    if (status != PAL_ATTEST_SUCCESS)
        return status;

    /* Verify the signature */
    status = psa_asymmetric_verify(public_key_handle,
                                   key_alg, token_hash.ptr, token_hash.len,
                                   signature.ptr, signature.len);
    if (status != PSA_SUCCESS)
        return PAL_ATTEST_ERR_SIGNATURE_FAIL;

    /* Unregister the attestation public key */
    status = pal_destroy_attest_key();
    if (status != PSA_SUCCESS)
        return PAL_ATTEST_ERR_KEY_FAIL;

    return PAL_ATTEST_SUCCESS;
}
