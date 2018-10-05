/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
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

#ifndef _PAL_CRYPTO_H_
#define _PAL_CRYPTO_H_

#include <stdarg.h>

#define PAL_STATUS_UNSUPPORTED_FUNC      0xFF

typedef uint32_t psa_key_usage_t;
typedef uint32_t psa_algorithm_t;
typedef int32_t  psa_status_t;
typedef uint32_t psa_key_type_t;
typedef uint32_t psa_key_slot_t;
typedef uint32_t psa_key_lifetime_t;
typedef uint32_t size_t;

enum crypto_function_code {
    PAL_CRYPTO_INIT                     = 0x1,
    PAL_CRYPTO_GENERATE_RANDOM          = 0x2,
    PAL_CRYPTO_IMPORT_KEY               = 0x3,
    PAL_CRYPTO_EXPORT_KEY               = 0x4,
    PAL_CRYPTO_EXPORT_PUBLIC_KEY        = 0x5,
    PAL_CRYPTO_DESTROY_KEY              = 0x6,
    PAL_CRYPTO_GET_KEY_INFO             = 0x7,
    PAL_CRYPTO_KEY_POLICY_INIT          = 0x8,
    PAL_CRYPTO_KEY_POLICY_SET_USAGE     = 0x9,
    PAL_CRYPTO_KEY_POLICY_GET_USAGE     = 0xA,
    PAL_CRYPTO_KEY_POLICY_GET_ALGORITHM = 0xB,
    PAL_CRYPTO_SET_KEY_POLICY           = 0xC,
    PAL_CRYPTO_GET_KEY_POLICY           = 0xD,
    PAL_CRYPTO_GET_KEY_INFORMATION      = 0xE,
    PAL_CRYPTO_GET_KEY_LIFETIME         = 0xF,
    PAL_CRYPTO_SET_KEY_LIFETIME         = 0x10,
    PAL_CRYPTO_HASH_SETUP               = 0x11,
    PAL_CRYPTO_HASH_UPDATE              = 0x12,
    PAL_CRYPTO_HASH_VERIFY              = 0x13,
    PAL_CRYPTO_HASH_FINISH              = 0x14,
    PAL_CRYPTO_HASH_ABORT               = 0x15,
};

struct psa_key_policy_s {
    psa_key_usage_t usage;
    psa_algorithm_t alg;
};

typedef struct {
    unsigned char cksum[16];    /*!< checksum of the data block */
    unsigned char state[48];    /*!< intermediate digest state  */
    unsigned char buffer[16];   /*!< data block being processed */
    size_t left;                /*!< amount of data in buffer   */
} mbedtls_md2_context;

typedef struct {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[4];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
} mbedtls_md4_context;

typedef struct {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[4];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
} mbedtls_md5_context;

typedef struct {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[5];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
} mbedtls_ripemd160_context;

typedef struct {
    uint32_t total[2];          /*!< The number of Bytes processed.  */
    uint32_t state[5];          /*!< The intermediate digest state.  */
    unsigned char buffer[64];   /*!< The data block being processed. */
} mbedtls_sha1_context;

typedef struct {
    uint32_t total[2];          /*!< The number of Bytes processed.  */
    uint32_t state[8];          /*!< The intermediate digest state.  */
    unsigned char buffer[64];   /*!< The data block being processed. */
    int is224;                  /*!< Determines which function to use:
                                     0: Use SHA-256, or 1: Use SHA-224. */
} mbedtls_sha256_context;

typedef struct {
    uint64_t total[2];          /*!< The number of Bytes processed. */
    uint64_t state[8];          /*!< The intermediate digest state. */
    unsigned char buffer[128];  /*!< The data block being processed. */
    int is384;                  /*!< Determines which function to use:
                                     0: Use SHA-512, or 1: Use SHA-384. */
} mbedtls_sha512_context;

struct psa_hash_operation_s {
    psa_algorithm_t alg;
    union
    {
        unsigned dummy; /* Make the union non-empty even with no supported algorithms. */
        mbedtls_md2_context md2;
        mbedtls_md4_context md4;
        mbedtls_md5_context md5;
        mbedtls_ripemd160_context ripemd160;
        mbedtls_sha1_context sha1;
        mbedtls_sha256_context sha256;
        mbedtls_sha512_context sha512;
    } ctx;
};

typedef struct psa_hash_operation_s psa_hash_operation_t;
typedef struct psa_key_policy_s psa_key_policy_t;

psa_status_t psa_crypto_init(void);
psa_status_t psa_import_key(psa_key_slot_t key, psa_key_type_t type,
                            const uint8_t *data, size_t data_length);
psa_status_t psa_export_key(psa_key_slot_t key, uint8_t *data,
                            size_t data_size, size_t *data_length);
void psa_key_policy_init(psa_key_policy_t *policy);
void psa_key_policy_set_usage(psa_key_policy_t *policy, psa_key_usage_t usage,
                              psa_algorithm_t alg);
psa_status_t psa_set_key_policy(psa_key_slot_t key, const psa_key_policy_t *policy);
psa_status_t psa_destroy_key(psa_key_slot_t key);
psa_status_t psa_export_public_key(psa_key_slot_t key, uint8_t *data,
                                   size_t data_size, size_t *data_length);
psa_status_t psa_generate_key(psa_key_slot_t key,  psa_key_type_t type,
                              size_t bits, const void *extra, size_t extra_size);
psa_status_t psa_get_key_information(psa_key_slot_t key, psa_key_type_t *type, size_t *bits);
psa_status_t psa_get_key_policy(psa_key_slot_t key, psa_key_policy_t *policy);
psa_key_usage_t psa_key_policy_get_usage(const psa_key_policy_t *policy);
psa_algorithm_t psa_key_policy_get_algorithm(const psa_key_policy_t *policy);
psa_status_t psa_get_key_lifetime(psa_key_slot_t key,
                                  psa_key_lifetime_t *lifetime);
psa_status_t psa_set_key_lifetime(psa_key_slot_t key,
                                  psa_key_lifetime_t lifetime);
psa_status_t psa_hash_setup(psa_hash_operation_t *operation,
                            psa_algorithm_t alg);

psa_status_t psa_hash_update(psa_hash_operation_t *operation,
                             const uint8_t *input,
                             size_t input_length);
psa_status_t psa_hash_verify(psa_hash_operation_t *operation,
                             const uint8_t *hash,
                             size_t hash_length);
psa_status_t psa_hash_finish(psa_hash_operation_t *operation,
                             uint8_t *hash,
                             size_t hash_size,
                             size_t *hash_length);
psa_status_t psa_hash_abort(psa_hash_operation_t *operation);
psa_status_t psa_generate_random(uint8_t *output, size_t output_size);
psa_status_t pal_crypto_function(int type, va_list valist);
#endif /* _PAL_CRYPTO_H_ */
