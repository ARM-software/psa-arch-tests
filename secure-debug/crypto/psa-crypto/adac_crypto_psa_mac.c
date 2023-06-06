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

#if (defined(PSA_ADAC_CMAC) || defined(PSA_ADAC_HMAC))

psa_status_t psa_adac_mac_verify(psa_algorithm_t alg, const uint8_t *inputs[], size_t input_sizes[],
                                 size_t input_count, const uint8_t key[], size_t key_size,
                                 uint8_t mac[], size_t mac_size)
{
    psa_status_t ret = PSA_ERROR_NOT_SUPPORTED;
    psa_algorithm_t algo;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_handle_t handle;
    size_t bits;

    if (alg == PSA_ALG_CMAC) {
        if (key_size != 16U) {
            ret = PSA_ERROR_INVALID_ARGUMENT;
        } else{
            ret = PSA_SUCCESS;
            algo = PSA_ALG_CMAC;
            bits = 128;
        }
    }

    if (PSA_SUCCESS == ret) {
        psa_set_key_usage_flags(&attributes, 0);
        psa_set_key_algorithm(&attributes, algo);
        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_VERIFY_HASH);
        psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_VOLATILE);
        psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
        psa_set_key_bits(&attributes, bits);

        ret = psa_import_key(&attributes, key, key_size, &handle);
        if (PSA_SUCCESS == ret) {
            psa_mac_operation_t operation = psa_mac_operation_init();

            ret = psa_mac_verify_setup(&operation, handle, algo);
            for (size_t i = 0; (PSA_SUCCESS == ret) && (i < input_count); i++)
                ret = psa_mac_update(&operation, inputs[i], input_sizes[i]);

            if (PSA_SUCCESS == ret)
                ret = psa_mac_verify_finish(&operation, mac, mac_size);

            psa_destroy_key(handle);
            psa_mac_abort(&operation);
        }
    }

    return ret;
}

psa_status_t psa_adac_derive_hmac(uint8_t *key, uint8_t *info, size_t size, uint8_t *output)
{
    psa_algorithm_t algo = PSA_ALG_HKDF(PSA_ALG_SHA_256);
    psa_key_handle_t handle;

    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, 0);
    psa_set_key_algorithm(&attributes, algo);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DERIVE);
    psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_VOLATILE);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_DERIVE);
    psa_set_key_bits(&attributes, 256);

    psa_status_t ret = psa_import_key(&attributes, key, 32, &handle);
    if (PSA_SUCCESS == ret) {
        psa_key_derivation_operation_t operation = psa_key_derivation_operation_init();

        ret = psa_key_derivation_setup(&operation, algo);
        if (PSA_SUCCESS == ret) {
            ret = psa_key_derivation_input_key(&operation, PSA_KEY_DERIVATION_INPUT_SECRET, handle);
        }
        if (PSA_SUCCESS == ret) {
            ret = psa_key_derivation_input_bytes(&operation, PSA_KEY_DERIVATION_INPUT_INFO,
                                                 info, size);
        }
        if (PSA_SUCCESS == ret) {
            ret = psa_key_derivation_output_bytes(&operation, output, 32);
        }
        psa_destroy_key(handle);
        psa_key_derivation_abort(&operation);
    }
    return ret;
}

/*  A key-derivation function can iterates n times until l bits of keying material are generated.
        For each of the iterations of the PRF, i=1 to n, do:
        result(0) = 0;
        K(i) = PRF (Ki, [i] || Label || 0x00 || Context || length);
        results(i) = result(i-1) || K(i);

        concisely, result(i) = K(i) || k(i-1) || .... || k(0)*/

// NIST SP800-108 + Inspiration from SCP03
// Label || 0x00 || [L]_2 || [i]_2 || Context

psa_status_t psa_adac_derive_cmac(uint8_t *key, uint8_t *context, size_t context_size,
                                  uint8_t *output)
{
    psa_algorithm_t algo = PSA_ALG_CMAC;
    psa_key_handle_t handle;
    size_t l = 0;
    uint8_t info[16] = {
            // Label = 'PSA ADAC 1.0'
            0x50, 0x53, 0x41, 0x20, 0x41, 0x44, 0x41, 0x43, 0x20, 0x31, 0x2E, 0x30,
            // Separator (0x00)
            0x00,
            // [L]_2 = 128 (encoded in 16 bit)
            0x00, 0x80,
            // [i]_2 = 8-bit counter
            0x01
            // Context = input
    };

    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    psa_set_key_usage_flags(&attributes, 0);
    psa_set_key_algorithm(&attributes, algo);
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 128);

    psa_status_t ret = psa_import_key(&attributes, key, 16, &handle);

    if (PSA_SUCCESS == ret) {
        psa_mac_operation_t operation = psa_mac_operation_init();

        ret = psa_mac_sign_setup(&operation, handle, algo);
        if (PSA_SUCCESS == ret) {
            ret = psa_mac_update(&operation, info, sizeof(info));
        }
        if (PSA_SUCCESS == ret) {
            ret = psa_mac_update(&operation, context, context_size);
        }
        if (PSA_SUCCESS == ret) {
            ret = psa_mac_sign_finish(&operation, output, 16, &l);
            if ((PSA_SUCCESS == ret) && (l != 16U))
                ret = PSA_ERROR_GENERIC_ERROR;
        }
        psa_destroy_key(handle);
        psa_mac_abort(&operation);
    }

    return ret;
}

psa_status_t psa_adac_derive_key(uint8_t *crt, size_t crt_size,
                                 uint8_t key_type, uint8_t *key, size_t key_size)
{
    psa_status_t ret;

    if (key_type == CMAC_AES) {
#ifdef PSA_ADAC_CMAC
        if ((key_size == 16U) && (crt_size >= sizeof(certificate_cmac_cmac_t))) {
            ret = psa_adac_derive_cmac(key, crt, offsetof(certificate_cmac_cmac_t, signature), key);
            PSA_ADAC_LOG_DUMP("psa_adac_derive_key", "cmac", key, 16);
        } else {
            ret = PSA_ERROR_INVALID_ARGUMENT;
        }
#else
        ret = PSA_ERROR_NOT_SUPPORTED;
#endif
    } else if (key_type == HMAC_SHA256) {
#ifdef PSA_ADAC_HMAC
        if ((key_size == 32U) && (crt_size >= sizeof(certificate_hmac_hmac_t))) {
            ret = psa_adac_derive_hmac(key, crt, offsetof(certificate_hmac_hmac_t, signature), key);
            PSA_ADAC_LOG_DUMP("psa_adac_derive_key", "hmac", key, 32);
        } else {
            ret = PSA_ERROR_INVALID_ARGUMENT;
        }
#else
        ret = PSA_ERROR_NOT_SUPPORTED;
#endif
    } else {
        ret = PSA_ERROR_NOT_SUPPORTED;
    }

    return ret;
}

psa_status_t psa_adac_verify_mac(uint8_t key_type, uint8_t *key, size_t key_size,
                                 const uint8_t *inputs[], size_t input_sizes[], size_t input_count,
                                 psa_algorithm_t mac_algo, uint8_t *mac, size_t mac_size)
{
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_handle_t handle = -1;
    psa_status_t ret = PSA_ERROR_NOT_SUPPORTED;
    psa_key_type_t type = 0;
    size_t bits = 0;

#ifdef PSA_ADAC_CMAC
    if ((key_type == CMAC_AES) && (mac_algo == CMAC_SIGN_ALGORITHM)) {
        if (key_size != 16U) {
            ret = PSA_ERROR_INVALID_ARGUMENT;
        } else {
            ret = PSA_SUCCESS;
            type = PSA_KEY_TYPE_AES;
            bits = 128;
        }
    }
#endif

#ifdef PSA_ADAC_HMAC
    if ((key_type == HMAC_SHA256) && (mac_algo == HMAC_SIGN_ALGORITHM)) {
        if (key_size != 32U) {
            ret = PSA_ERROR_INVALID_ARGUMENT;
        } else {
            ret = PSA_SUCCESS;
            type = PSA_KEY_TYPE_HMAC;
            bits = 256;
        }
    }
#endif

    if (ret == PSA_SUCCESS) {
        psa_set_key_usage_flags(&attributes, 0);
        psa_set_key_algorithm(&attributes, mac_algo);
        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_VERIFY_HASH);
        psa_set_key_type(&attributes, type);
        psa_set_key_bits(&attributes, bits);
        ret = psa_import_key(&attributes, key, key_size, &handle);

        if (PSA_SUCCESS != ret) {
            PSA_ADAC_LOG_ERR("self", "Error importing key (%d)\n", ret);
        } else {
            psa_mac_operation_t operation = psa_mac_operation_init();

            ret = psa_mac_verify_setup(&operation, handle, mac_algo);
            for (size_t i = 0; (PSA_SUCCESS == ret) && (i < input_count); i++) {
                ret = psa_mac_update(&operation, inputs[i], input_sizes[i]);
            }
            if (PSA_SUCCESS == ret) {
                ret = psa_mac_verify_finish(&operation, mac, mac_size);
            }
            if (PSA_SUCCESS != ret) {
                PSA_ADAC_LOG_ERR("self", "Error during MAC verification (%d)\n", ret);
            }
            psa_destroy_key(handle);
            psa_mac_abort(&operation);
        }
    }

    return ret;
}

#endif
