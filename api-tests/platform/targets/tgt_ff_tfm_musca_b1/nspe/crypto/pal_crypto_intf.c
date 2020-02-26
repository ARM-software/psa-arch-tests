/** @file
 * Copyright (c) 2020, Arm Limited or its affiliates. All rights reserved.
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


#include "pal_crypto_intf.h"

#define  PAL_KEY_SLOT_COUNT  32

/**
    @brief    - This API will call the requested crypto function
    @param    - type    : function code
                valist  : variable argument list
    @return   - error status
**/
int32_t pal_crypto_function(int type, va_list valist)
{
    int                              i;
    psa_status_t                     status;
    uint8_t                         *buffer, *ciphertext, *plaintext;
    const uint8_t                   *nonce, *additional_data, *salt, *peer;
    size_t                          *length, size, ciphertext_size, nonce_length;
    size_t                           salt_length, peer_length, additional_data_length;
#ifdef NOT_SUPPORTED
    size_t                          *tag_length, plaintext_size;
    psa_aead_operation_t            *aead_operation;
#endif
    psa_key_attributes_t            *attributes;
    psa_key_handle_t                *handle, key_handle;
    psa_key_type_t                  *key_type_out, key_type;
    psa_key_usage_t                 *usage_out, usage;
    psa_key_id_t                    *key_id_out, key_id;
    psa_key_lifetime_t              *key_lifetime_out, key_lifetime;
    psa_algorithm_t                 *key_alg_out, key_alg, alg;
    psa_hash_operation_t            *hash_operation, *target_operation;
    psa_mac_operation_t             *mac_operation;
    psa_cipher_operation_t          *cipher_operation;
    psa_key_derivation_operation_t  *derive_operation;
    psa_key_derivation_step_t        step;

    switch (type)
    {
        case PAL_CRYPTO_INIT:
            return psa_crypto_init();
        case PAL_CRYPTO_GENERATE_RANDOM:
            buffer = va_arg(valist, uint8_t *);
            size = va_arg(valist, int);
            return psa_generate_random(buffer, size);
        case PAL_CRYPTO_IMPORT_KEY:
            attributes = va_arg(valist, psa_key_attributes_t *);
            buffer = va_arg(valist, uint8_t *);
            size = va_arg(valist, size_t);
            handle = (psa_key_handle_t *)va_arg(valist, int *);
            status = psa_import_key(attributes, buffer, size, handle);
            return status;
        case PAL_CRYPTO_SET_KEY_TYPE:
            attributes = va_arg(valist, psa_key_attributes_t *);
            key_type = va_arg(valist, psa_key_type_t);
            psa_set_key_type(attributes, key_type);
            return 0;
        case PAL_CRYPTO_SET_KEY_BITS:
            attributes = va_arg(valist, psa_key_attributes_t *);
            size = va_arg(valist, size_t);
            psa_set_key_bits(attributes, size);
            return 0;
        case PAL_CRYPTO_GET_KEY_ATTRIBUTES:
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            attributes = va_arg(valist, psa_key_attributes_t *);
            return psa_get_key_attributes(key_handle, attributes);
        case PAL_CRYPTO_GET_KEY_TYPE:
            attributes = va_arg(valist, psa_key_attributes_t *);
            key_type_out = va_arg(valist, psa_key_type_t *);
            *key_type_out = psa_get_key_type(attributes);
            return 0;
        case PAL_CRYPTO_EXPORT_KEY:
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            buffer = (uint8_t *)(va_arg(valist, uint8_t *));
            size = va_arg(valist, size_t);
            length = (size_t *)va_arg(valist, size_t *);
            return psa_export_key(key_handle, buffer, size, length);
        case PAL_CRYPTO_SET_KEY_USAGE_FLAGS:
            attributes = va_arg(valist, psa_key_attributes_t *);
            usage = va_arg(valist, psa_key_usage_t);
            psa_set_key_usage_flags(attributes, usage);
            return 0;
        case PAL_CRYPTO_RESET_KEY_ATTRIBUTES:
            attributes = va_arg(valist, psa_key_attributes_t *);
            psa_reset_key_attributes(attributes);
            return 0;
        case PAL_CRYPTO_EXPORT_PUBLIC_KEY:
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            buffer = (uint8_t *)(va_arg(valist, uint8_t *));
            size = va_arg(valist, size_t);
            length = (size_t *)va_arg(valist, size_t *);
            return psa_export_public_key(key_handle, buffer, size, length);
        case PAL_CRYPTO_SET_KEY_ID:
            attributes = va_arg(valist, psa_key_attributes_t *);
            key_id = va_arg(valist, psa_key_id_t);
            psa_set_key_id(attributes, key_id);
            return 0;
        case PAL_CRYPTO_SET_KEY_LIFETIME:
            attributes = va_arg(valist, psa_key_attributes_t *);
            key_lifetime = va_arg(valist, psa_key_lifetime_t);
            psa_set_key_lifetime(attributes, key_lifetime);
            return 0;
        case PAL_CRYPTO_SET_KEY_ALGORITHM:
            attributes = va_arg(valist, psa_key_attributes_t *);
            key_alg = va_arg(valist, psa_algorithm_t);
            psa_set_key_algorithm(attributes, key_alg);
            return 0;
        case PAL_CRYPTO_GET_KEY_ID:
            attributes = va_arg(valist, psa_key_attributes_t *);
            key_id_out = va_arg(valist, psa_key_id_t *);
            *key_id_out = psa_get_key_id(attributes);
            return 0;
        case PAL_CRYPTO_GET_KEY_LIFETIME:
            attributes = va_arg(valist, psa_key_attributes_t *);
            key_lifetime_out = va_arg(valist, psa_key_lifetime_t *);
            *key_lifetime_out = psa_get_key_lifetime(attributes);
            return 0;
        case PAL_CRYPTO_GET_KEY_USAGE_FLAGS:
            attributes = va_arg(valist, psa_key_attributes_t *);
            usage_out = va_arg(valist, psa_key_usage_t *);
            *usage_out = psa_get_key_usage_flags(attributes);
            return 0;
        case PAL_CRYPTO_GET_KEY_ALGORITHM:
            attributes = va_arg(valist, psa_key_attributes_t *);
            key_alg_out = va_arg(valist, psa_algorithm_t *);
            *key_alg_out = psa_get_key_algorithm(attributes);
            return 0;
        case PAL_CRYPTO_GET_KEY_BITS:
            attributes = va_arg(valist, psa_key_attributes_t *);
            length = va_arg(valist, size_t *);
            *length = psa_get_key_bits(attributes);
            return 0;
        case PAL_CRYPTO_DESTROY_KEY:
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            return psa_destroy_key(key_handle);
        case PAL_CRYPTO_HASH_SETUP:
            hash_operation = va_arg(valist, psa_hash_operation_t*);
            alg = va_arg(valist, psa_algorithm_t);
            return psa_hash_setup(hash_operation, alg);
        case PAL_CRYPTO_HASH_UPDATE:
            hash_operation = va_arg(valist, psa_hash_operation_t*);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            return psa_hash_update(hash_operation, buffer, size);
        case PAL_CRYPTO_HASH_VERIFY:
            hash_operation = va_arg(valist, psa_hash_operation_t*);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            return psa_hash_verify(hash_operation, buffer, size);
        case PAL_CRYPTO_HASH_FINISH:
            hash_operation = va_arg(valist, psa_hash_operation_t*);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            length = va_arg(valist, size_t*);
            return psa_hash_finish(hash_operation, buffer, size, length);
        case PAL_CRYPTO_HASH_ABORT:
            hash_operation = va_arg(valist, psa_hash_operation_t*);
            return psa_hash_abort(hash_operation);
#ifdef NOT_SUPPORTED
        case PAL_CRYPTO_HASH_COMPUTE:
            alg = va_arg(valist, psa_algorithm_t);
            plaintext = va_arg(valist, uint8_t*);
            plaintext_size = va_arg(valist, size_t);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            length = va_arg(valist, size_t*);
            return psa_hash_compute(alg, plaintext, plaintext_size, buffer, size, length);
        case PAL_CRYPTO_HASH_COMPARE:
            alg = va_arg(valist, psa_algorithm_t);
            plaintext = va_arg(valist, uint8_t*);
            plaintext_size = va_arg(valist, size_t);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            return psa_hash_compare(alg, plaintext, plaintext_size, buffer, size);
#endif
        case PAL_CRYPTO_HASH_CLONE:
            hash_operation = va_arg(valist, psa_hash_operation_t*);
            target_operation = va_arg(valist, psa_hash_operation_t*);
            return psa_hash_clone(hash_operation, target_operation);
        case PAL_CRYPTO_GENERATE_KEY:
            attributes = va_arg(valist, psa_key_attributes_t *);
            handle = (psa_key_handle_t *)va_arg(valist, int *);
            return psa_generate_key(attributes, handle);
        case PAL_CRYPTO_AEAD_ENCRYPT:
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            alg = va_arg(valist, psa_algorithm_t);
            nonce = va_arg(valist, const uint8_t *);
            nonce_length = va_arg(valist, size_t);
            additional_data = va_arg(valist, const uint8_t *);
            additional_data_length = va_arg(valist, size_t);
            plaintext = va_arg(valist, uint8_t *);
            size = va_arg(valist, size_t);
            ciphertext = va_arg(valist, uint8_t *);
            ciphertext_size = va_arg(valist, size_t);
            length = va_arg(valist, size_t*);
            return psa_aead_encrypt(key_handle, alg, nonce, nonce_length, additional_data,
            additional_data_length, plaintext, size, ciphertext, ciphertext_size, length);
        case PAL_CRYPTO_AEAD_DECRYPT:
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            alg = va_arg(valist, psa_algorithm_t);
            nonce = va_arg(valist, const uint8_t *);
            nonce_length = va_arg(valist, size_t);
            additional_data = va_arg(valist, const uint8_t *);
            additional_data_length = va_arg(valist, size_t);
            ciphertext = va_arg(valist, uint8_t *);
            ciphertext_size = va_arg(valist, size_t);
            plaintext = va_arg(valist, uint8_t *);
            size = va_arg(valist, size_t);
            length = va_arg(valist, size_t*);
            return psa_aead_decrypt(key_handle, alg, nonce, nonce_length, additional_data,
            additional_data_length, ciphertext, ciphertext_size, plaintext, size, length);
#ifdef NO_SUPPORT
        case PAL_CRYPTO_AEAD_ENCRYPT_SETUP:
            aead_operation = va_arg(valist, psa_aead_operation_t *);
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            alg = va_arg(valist, psa_algorithm_t);
            return psa_aead_encrypt_setup(aead_operation, key_handle, alg);
        case PAL_CRYPTO_AEAD_DECRYPT_SETUP:
            aead_operation = va_arg(valist, psa_aead_operation_t *);
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            alg = va_arg(valist, psa_algorithm_t);
            return psa_aead_decrypt_setup(aead_operation, key_handle, alg);
        case PAL_CRYPTO_AEAD_GENERATE_NONCE:
            aead_operation = va_arg(valist, psa_aead_operation_t *);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            length = (size_t *)va_arg(valist, size_t*);
            return psa_aead_generate_nonce(aead_operation, buffer, size, length);
        case PAL_CRYPTO_AEAD_SET_NONCE:
            aead_operation = va_arg(valist, psa_aead_operation_t *);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            return psa_aead_set_nonce(aead_operation, buffer, size);
        case PAL_CRYPTO_AEAD_SET_LENGTHS:
            aead_operation = va_arg(valist, psa_aead_operation_t *);
            size = va_arg(valist, size_t);
            plaintext_size = va_arg(valist, size_t);
            return psa_aead_set_lengths(aead_operation, size, plaintext_size);
        case PAL_CRYPTO_AEAD_UPDATE_AD:
            aead_operation = va_arg(valist, psa_aead_operation_t *);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            return psa_aead_update_ad(aead_operation, buffer, size);
        case PAL_CRYPTO_AEAD_UPDATE:
            aead_operation = va_arg(valist, psa_aead_operation_t *);
            plaintext = va_arg(valist, uint8_t*);
            plaintext_size = va_arg(valist, size_t);
            ciphertext = va_arg(valist, uint8_t*);
            ciphertext_size = va_arg(valist, size_t);
            length = va_arg(valist, size_t*);
            return psa_aead_update(aead_operation, plaintext, plaintext_size, ciphertext,
            ciphertext_size, length);
        case PAL_CRYPTO_AEAD_FINISH:
            aead_operation = va_arg(valist, psa_aead_operation_t *);
            ciphertext = va_arg(valist, uint8_t*);
            ciphertext_size = va_arg(valist, size_t);
            length = va_arg(valist, size_t*);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            tag_length = (size_t *)va_arg(valist, size_t*);
            return psa_aead_finish(aead_operation, ciphertext, ciphertext_size, length, buffer,
            size, tag_length);
        case PAL_CRYPTO_AEAD_VERIFY:
            aead_operation = va_arg(valist, psa_aead_operation_t *);
            plaintext = va_arg(valist, uint8_t*);
            plaintext_size = va_arg(valist, size_t);
            length = (size_t *)va_arg(valist, size_t*);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            return psa_aead_verify(aead_operation, plaintext, plaintext_size, length, buffer, size);
        case PAL_CRYPTO_AEAD_ABORT:
            aead_operation = va_arg(valist, psa_aead_operation_t *);
            return psa_aead_abort(aead_operation);
#endif
        case PAL_CRYPTO_MAC_SIGN_SETUP:
            mac_operation = va_arg(valist, psa_mac_operation_t*);
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            alg = va_arg(valist, psa_algorithm_t);
            return psa_mac_sign_setup(mac_operation, key_handle, alg);
        case PAL_CRYPTO_MAC_UPDATE:
            mac_operation = va_arg(valist, psa_mac_operation_t*);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            return psa_mac_update(mac_operation, buffer, size);
        case PAL_CRYPTO_MAC_SIGN_FINISH:
            mac_operation = va_arg(valist, psa_mac_operation_t*);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            length = (size_t *)va_arg(valist, size_t*);
            return psa_mac_sign_finish(mac_operation, buffer, size, length);
        case PAL_CRYPTO_MAC_VERIFY_SETUP:
            mac_operation = va_arg(valist, psa_mac_operation_t*);
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            alg = va_arg(valist, psa_algorithm_t);
            return psa_mac_verify_setup(mac_operation, key_handle, alg);
        case PAL_CRYPTO_MAC_VERIFY_FINISH:
            mac_operation = va_arg(valist, psa_mac_operation_t*);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            return psa_mac_verify_finish(mac_operation, buffer, size);
        case PAL_CRYPTO_MAC_ABORT:
            mac_operation = va_arg(valist, psa_mac_operation_t*);
            return psa_mac_abort(mac_operation);
#ifdef NO_SUPPORT
        case PAL_CRYPTO_MAC_COMPUTE:
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            alg = va_arg(valist, psa_algorithm_t);
            plaintext = va_arg(valist, uint8_t*);
            plaintext_size = va_arg(valist, size_t);
            ciphertext = va_arg(valist, uint8_t*);
            ciphertext_size = va_arg(valist, size_t);
            length = va_arg(valist, size_t*);
            return psa_mac_compute(key_handle, alg, plaintext, plaintext_size, ciphertext,
            ciphertext_size, length);
        case PAL_CRYPTO_MAC_VERIFY:
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            alg = va_arg(valist, psa_algorithm_t);
            plaintext = va_arg(valist, uint8_t*);
            plaintext_size = va_arg(valist, size_t);
            ciphertext = va_arg(valist, uint8_t*);
            ciphertext_size = va_arg(valist, size_t);
            return psa_mac_verify(key_handle, alg, plaintext, plaintext_size, ciphertext,
            ciphertext_size);
        case PAL_CRYPTO_ASYMMTERIC_ENCRYPT:
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            alg = va_arg(valist, psa_algorithm_t);
            plaintext = va_arg(valist, uint8_t *);
            size = va_arg(valist, size_t);
            salt = va_arg(valist, const uint8_t *);
            salt_length = va_arg(valist, size_t);
            ciphertext = va_arg(valist, uint8_t *);
            ciphertext_size = va_arg(valist, size_t);
            length = va_arg(valist, size_t*);
            return psa_asymmetric_encrypt(key_handle, alg, plaintext, size, salt, salt_length,
            ciphertext, ciphertext_size, length);
        case PAL_CRYPTO_ASYMMTERIC_DECRYPT:
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            alg = va_arg(valist, psa_algorithm_t);
            plaintext = va_arg(valist, uint8_t *);
            size = va_arg(valist, size_t);
            salt = va_arg(valist, const uint8_t *);
            salt_length = va_arg(valist, size_t);
            ciphertext = va_arg(valist, uint8_t *);
            ciphertext_size = va_arg(valist, size_t);
            length = va_arg(valist, size_t*);
            return psa_asymmetric_decrypt(key_handle, alg, plaintext, size, salt, salt_length,
            ciphertext, ciphertext_size, length);
        case PAL_CRYPTO_CIPHER_ENCRYPT_SETUP:
            cipher_operation =  va_arg(valist, psa_cipher_operation_t *);
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            alg = va_arg(valist, psa_algorithm_t);
            return psa_cipher_encrypt_setup(cipher_operation, key_handle, alg);
        case PAL_CRYPTO_CIPHER_DECRYPT_SETUP:
            cipher_operation =  va_arg(valist, psa_cipher_operation_t *);
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            alg = va_arg(valist, psa_algorithm_t);
            return psa_cipher_decrypt_setup(cipher_operation, key_handle, alg);
        case PAL_CRYPTO_CIPHER_GENERATE_IV:
            cipher_operation =  va_arg(valist, psa_cipher_operation_t *);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            length = va_arg(valist, size_t*);
            return psa_cipher_generate_iv(cipher_operation, buffer, size, length);
        case PAL_CRYPTO_CIPHER_SET_IV:
            cipher_operation =  va_arg(valist, psa_cipher_operation_t *);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            return psa_cipher_set_iv(cipher_operation, buffer, size);
        case PAL_CRYPTO_CIPHER_UPDATE:
            cipher_operation =  va_arg(valist, psa_cipher_operation_t *);
            plaintext = va_arg(valist, uint8_t *);
            size = va_arg(valist, size_t);
            ciphertext = va_arg(valist, uint8_t *);
            ciphertext_size = va_arg(valist, size_t);
            length = va_arg(valist, size_t*);
            return psa_cipher_update(cipher_operation, plaintext, size, ciphertext, ciphertext_size,
            length);
        case PAL_CRYPTO_CIPHER_FINISH:
            cipher_operation =  va_arg(valist, psa_cipher_operation_t *);
            ciphertext = va_arg(valist, uint8_t *);
            ciphertext_size = va_arg(valist, size_t);
            length = va_arg(valist, size_t*);
            return psa_cipher_finish(cipher_operation, ciphertext, ciphertext_size, length);
        case PAL_CRYPTO_CIPHER_ABORT:
            cipher_operation =  va_arg(valist, psa_cipher_operation_t *);
            return psa_cipher_abort(cipher_operation);
#ifdef NO_SUPPORT
        case PAL_CRYPTO_CIPHER_ENCRYPT:
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            alg = va_arg(valist, psa_algorithm_t);
            plaintext = va_arg(valist, uint8_t *);
            size = va_arg(valist, size_t);
            ciphertext = va_arg(valist, uint8_t *);
            ciphertext_size = va_arg(valist, size_t);
            length = va_arg(valist, size_t*);
            return psa_cipher_encrypt(key_handle, alg, plaintext, size, ciphertext, ciphertext_size,
            length);
        case PAL_CRYPTO_CIPHER_DECRYPT:
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            alg = va_arg(valist, psa_algorithm_t);
            plaintext = va_arg(valist, uint8_t *);
            size = va_arg(valist, size_t);
            ciphertext = va_arg(valist, uint8_t *);
            ciphertext_size = va_arg(valist, size_t);
            length = va_arg(valist, size_t*);
            return psa_cipher_decrypt(key_handle, alg, plaintext, size, ciphertext, ciphertext_size,
            length);
#endif
        case PAL_CRYPTO_ASYMMTERIC_SIGN:
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            alg = va_arg(valist, psa_algorithm_t);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            ciphertext = va_arg(valist, uint8_t *);
            ciphertext_size = va_arg(valist, size_t);
            length = va_arg(valist, size_t*);
            return psa_asymmetric_sign(key_handle, alg, buffer, size, ciphertext, ciphertext_size,
            length);
        case PAL_CRYPTO_ASYMMTERIC_VERIFY:
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            alg = va_arg(valist, psa_algorithm_t);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            ciphertext = va_arg(valist, uint8_t *);
            ciphertext_size = va_arg(valist, size_t);
            return psa_asymmetric_verify(key_handle, alg, buffer, size, ciphertext,
            ciphertext_size);
        case PAL_CRYPTO_RAW_KEY_AGREEMENT:
            alg = va_arg(valist, psa_algorithm_t);
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            peer = va_arg(valist, uint8_t*);
            peer_length = va_arg(valist, size_t);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            length = va_arg(valist, size_t*);
            return psa_raw_key_agreement(alg, key_handle, peer, peer_length, buffer, size, length);
        case PAL_CRYPTO_COPY_KEY:
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            attributes = va_arg(valist, psa_key_attributes_t *);
            handle = (psa_key_handle_t *)va_arg(valist, int *);
            return psa_copy_key(key_handle, attributes, handle);
        case PAL_CRYPTO_KEY_DERIVATION_SETUP:
            derive_operation = va_arg(valist, psa_key_derivation_operation_t *);
            alg = va_arg(valist, psa_algorithm_t);
            return psa_key_derivation_setup(derive_operation, alg);
        case PAL_CRYPTO_KEY_DERIVATION_INPUT_BYTES:
            derive_operation = va_arg(valist, psa_key_derivation_operation_t *);
            step = (psa_key_derivation_step_t)va_arg(valist, int);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            return psa_key_derivation_input_bytes(derive_operation, step, buffer, size);
        case PAL_CRYPTO_KEY_DERIVATION_INPUT_KEY:
            derive_operation = va_arg(valist, psa_key_derivation_operation_t *);
            step = (psa_key_derivation_step_t)va_arg(valist, int);
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            return psa_key_derivation_input_key(derive_operation, step, key_handle);
        case PAL_CRYPTO_KEY_DERIVATION_KEY_AGREEMENT:
            derive_operation = va_arg(valist, psa_key_derivation_operation_t *);
            step = (psa_key_derivation_step_t)va_arg(valist, int);
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            peer = va_arg(valist, uint8_t*);
            peer_length = va_arg(valist, size_t);
            return psa_key_derivation_key_agreement(derive_operation, step, key_handle, peer,
            peer_length);
        case PAL_CRYPTO_KEY_DERIVATION_OUTPUT_BYTES:
            derive_operation = va_arg(valist, psa_key_derivation_operation_t *);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            return psa_key_derivation_output_bytes(derive_operation, buffer, size);
        case PAL_CRYPTO_KEY_DERIVATION_OUTPUT_KEY:
            attributes = va_arg(valist, psa_key_attributes_t *);
            derive_operation = va_arg(valist, psa_key_derivation_operation_t *);
            handle = (psa_key_handle_t *)va_arg(valist, int *);
            return psa_key_derivation_output_key(attributes, derive_operation, handle);
        case PAL_CRYPTO_KEY_DERIVATION_SET_CAPACITY:
            derive_operation = va_arg(valist, psa_key_derivation_operation_t *);
            size = va_arg(valist, size_t);
            return psa_key_derivation_set_capacity(derive_operation, size);
        case PAL_CRYPTO_KEY_DERIVATION_GET_CAPACITY:
            derive_operation = va_arg(valist, psa_key_derivation_operation_t *);
            length = va_arg(valist, size_t *);
            return psa_key_derivation_get_capacity(derive_operation, length);
        case PAL_CRYPTO_KEY_DERIVATION_ABORT:
            derive_operation = va_arg(valist, psa_key_derivation_operation_t *);
            return psa_key_derivation_abort(derive_operation);
        case PAL_CRYPTO_OPEN_KEY:
            key_id = va_arg(valist, psa_key_id_t);
            handle = (psa_key_handle_t *)va_arg(valist, int *);
            return psa_open_key(key_id, handle);
        case PAL_CRYPTO_CLOSE_KEY:
            key_handle = (psa_key_handle_t)va_arg(valist, int);
            return psa_close_key(key_handle);
        case PAL_CRYPTO_FREE:
            for (i = 0; i < PAL_KEY_SLOT_COUNT; i++)
                psa_destroy_key(i);
            return 0;
        default:
            return PAL_STATUS_UNSUPPORTED_FUNC;
    }
}
