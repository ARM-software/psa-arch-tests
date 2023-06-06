/** @file
 * Copyright (c) 2023, Arm Limited or its affiliates. All rights reserved.
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

#include "adac_crypto_psa.h"
#include <psa_adac_debug.h>
#include <psa_adac_config.h>

#include <string.h>

#if defined(PSA_ADAC_RSA3072) || defined(PSA_ADAC_RSA4096)
static const uint8_t f4[] = {0x02, 0x03, 0x01, 0x00, 0x01};
#endif

#ifdef PSA_ADAC_RSA3072
static const uint8_t rsa3072_header[] = {0x30, 0x82, 0x01, 0x89, 0x02, 0x82, 0x01, 0x80, 0x00};

psa_status_t load_rsa_3072_public_key(uint8_t *key, size_t key_size, psa_key_handle_t *handle)
{
    psa_status_t ret;

    if (key_size == RSA_3072_PUBLIC_KEY_SIZE) {
        uint8_t pub_key[398];
        size_t offset = 8UL;
        size_t pub_size = sizeof(pub_key) - 1UL;

        (void) memcpy(pub_key, rsa3072_header, sizeof(rsa3072_header));

        // If MSB is set, modulus need to be prefixed by 0 value
        if ((key[0] & (uint8_t) 0x80U) != 0x00U) {
            pub_key[3] = 0x8aU;
            pub_key[7] = 0x81U;
            offset = 9UL;
            pub_size += 1UL;
        }

        (void) memcpy(&(pub_key[offset]), key, RSA_3072_PUBLIC_KEY_SIZE);
        offset += RSA_3072_PUBLIC_KEY_SIZE;
        (void) memcpy(&(pub_key[offset]), f4, sizeof(f4));

        psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_VERIFY_HASH);
        psa_set_key_type(&attributes, PSA_KEY_TYPE_RSA_PUBLIC_KEY);
        psa_set_key_algorithm(&attributes, PSA_ALG_RSA_PSS(PSA_ALG_ANY_HASH));
        psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_VOLATILE);
        psa_set_key_bits(&attributes, 3072);

        ret = psa_import_key(&attributes, pub_key, pub_size, handle);
    } else {
        ret = PSA_ERROR_INVALID_ARGUMENT;
    }

    return ret;
}

#endif

#ifdef PSA_ADAC_RSA4096
static const uint8_t rsa4096_header[] = {0x30, 0x82, 0x02, 0x09, 0x02, 0x82, 0x02, 0x00, 0x00};

psa_status_t load_rsa_4096_public_key(uint8_t *key, size_t key_size, psa_key_handle_t *handle)
{
    psa_status_t ret;

    if (RSA_4096_PUBLIC_KEY_SIZE == key_size) {
        uint8_t pub_key[526];
        size_t offset = 8UL;
        size_t pub_size = sizeof(pub_key) - 1UL;

        (void) memcpy(pub_key, rsa4096_header, sizeof(rsa4096_header));

        // If MSB is set, modulus need to be prefixed by 0 value
        if ((key[0] & (uint8_t) 0x80) != 0x00U) {
            pub_key[3] = 0x0a;
            pub_key[7] = 0x01;
            offset = 9UL;
            pub_size += 1UL;
        }

        (void) memcpy(&(pub_key[offset]), key, RSA_4096_PUBLIC_KEY_SIZE);
        offset += RSA_4096_PUBLIC_KEY_SIZE;
        (void) memcpy(&(pub_key[offset]), f4, sizeof(f4));

        psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_VERIFY_HASH);
        psa_set_key_type(&attributes, PSA_KEY_TYPE_RSA_PUBLIC_KEY);
        psa_set_key_algorithm(&attributes, PSA_ALG_RSA_PSS(PSA_ALG_ANY_HASH));
        psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_VOLATILE);
        psa_set_key_bits(&attributes, 4096);

        ret = psa_import_key(&attributes, pub_key, pub_size, handle);
    } else {
        ret = PSA_ERROR_INVALID_ARGUMENT;
    }

    return ret;
}

#endif

#ifdef PSA_ADAC_EC_P256

psa_status_t load_ecdsa_p256_public_key(uint8_t *key, size_t key_size, psa_key_handle_t *handle)
{
    psa_status_t ret;

    if (ECDSA_P256_PUBLIC_KEY_SIZE == key_size) {
        // cppcheck-suppress misra-c2012-18.8
        uint8_t pub_key[ECDSA_P256_PUBLIC_KEY_SIZE + 1] = {0x04};

        (void) memcpy(&(pub_key[1]), key, ECDSA_P256_PUBLIC_KEY_SIZE);
        psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_VERIFY_HASH);
        psa_set_key_type(&attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));
        psa_set_key_algorithm(&attributes, PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_256));
        psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_VOLATILE);
        psa_set_key_bits(&attributes, 256);

        ret = psa_import_key(&attributes, pub_key, sizeof(pub_key), handle);
    } else {

        ret = PSA_ERROR_INVALID_ARGUMENT;
    }

    return ret;
}

#endif

#ifdef PSA_ADAC_EC_P521

psa_status_t load_ecdsa_p521_public_key(uint8_t *key, size_t key_size, psa_key_handle_t *handle)
{
    psa_status_t ret;

    if (ECDSA_P521_PUBLIC_KEY_SIZE == key_size) {
        // cppcheck-suppress misra-c2012-18.8
        uint8_t pub_key[ECDSA_P521_PUBLIC_KEY_SIZE + 1] = {0x04};

        (void) memcpy(&(pub_key[1]), key, ECDSA_P521_PUBLIC_KEY_SIZE);
        psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_VERIFY_HASH);
        psa_set_key_type(&attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));
        psa_set_key_algorithm(&attributes, PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_512));
        psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_VOLATILE);
        psa_set_key_bits(&attributes, 521);

        ret = psa_import_key(&attributes, pub_key, sizeof(pub_key), handle);
    } else {
        ret = PSA_ERROR_INVALID_ARGUMENT;
    }

    return ret;
}

#endif

psa_status_t psa_adac_load_public_key(uint8_t key_type, uint8_t *key, size_t key_size,
                                      psa_key_handle_t *handle)
{
    psa_status_t ret = PSA_ERROR_NOT_SUPPORTED;

    if (key_type == ECDSA_P256_SHA256) {
#ifdef PSA_ADAC_EC_P256
        PSA_ADAC_LOG_TRACE("psa-crypto", "Load EcdsaP256 Public-key\n");
        ret = load_ecdsa_p256_public_key(key, key_size, handle);
#endif
    } else if (key_type == ECDSA_P521_SHA512) {
#ifdef PSA_ADAC_EC_P521
        PSA_ADAC_LOG_TRACE("psa-crypto", "Load EcdsaP521 Public-key\n");
        ret = load_ecdsa_p521_public_key(key, key_size, handle);
#endif
    } else if (key_type == RSA_3072_SHA256) {
#ifdef PSA_ADAC_RSA3072
        PSA_ADAC_LOG_TRACE("psa-crypto", "Load Rsa3072 Public-key\n");
        ret = load_rsa_3072_public_key(key, key_size, handle);
#endif
    } else if (key_type == RSA_4096_SHA256) {
#ifdef PSA_ADAC_RSA4096
        PSA_ADAC_LOG_TRACE("psa-crypto", "Load Rsa4096 Public-key\n");
        ret = load_rsa_4096_public_key(key, key_size, handle);
#endif
    } else {
        ret = PSA_ERROR_NOT_SUPPORTED;
    }

    return ret;
}

psa_status_t psa_adac_verify_signature(uint8_t key_type, uint8_t *key, size_t key_size,
                                       psa_algorithm_t hash_algo, const uint8_t *inputs[],
                                       size_t input_sizes[], size_t input_count,
                                       psa_algorithm_t sig_algo, uint8_t *sig, size_t sig_size)
{
    // cppcheck-suppress misra-c2012-18.8
    uint8_t hash[PSA_HASH_MAX_SIZE];
    size_t hash_size;
    psa_key_handle_t handle;
    psa_status_t ret;

    if ((PSA_ALG_IS_VENDOR_DEFINED(sig_algo) != 0) ||
        (sig_algo == PSA_ALG_HMAC(PSA_ALG_SHA_256)) || (sig_algo == PSA_ALG_CMAC)) {
        ret = psa_adac_verify_vendor(key_type, key, key_size, hash_algo,
                                     inputs, input_sizes, input_count,
                                     sig_algo, sig, sig_size);
    } else {
        ret = psa_adac_load_public_key(key_type, key, key_size, &handle);
        if (PSA_SUCCESS != ret) {
            PSA_ADAC_LOG_ERR("psa-crypto", "Error loading public key (%d)\n", ret);
        } else {
            ret = psa_adac_hash_multiple(hash_algo, inputs, input_sizes, input_count,
                                         hash, sizeof(hash), &hash_size);
            if (PSA_SUCCESS != ret) {
                PSA_ADAC_LOG_ERR("psa-crypto", "Error hashing content (%d)\n", ret);
            } else {
                PSA_ADAC_LOG_TRACE("psa-crypto", "Verify signature\n");
                ret = psa_verify_hash(handle, sig_algo, hash, hash_size, sig, sig_size);
                PSA_ADAC_LOG_DEBUG("psa-crypto", "Signature verification %s\n",
                                   (ret == PSA_SUCCESS) ? "successful" : "failed");
            }

            psa_destroy_key(handle);
        }
    }

    return ret;
}
