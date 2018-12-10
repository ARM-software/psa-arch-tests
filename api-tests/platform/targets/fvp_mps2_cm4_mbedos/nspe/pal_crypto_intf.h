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
#include "pal_common.h"

#define PAL_STATUS_UNSUPPORTED_FUNC      0xFF

typedef uint32_t psa_key_usage_t;
typedef uint32_t psa_algorithm_t;
typedef int32_t  psa_status_t;
typedef uint32_t psa_key_type_t;
typedef uint32_t psa_key_slot_t;
typedef uint32_t psa_key_lifetime_t;
typedef struct mbedtls_cipher_base_t mbedtls_cipher_base_t;

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
    PAL_CRYPTO_GENERATE_KEY             = 0x16,
    PAL_CRYPTO_GENERATOR_READ           = 0x17,
    PAL_CRYPTO_KEY_DERIVATION           = 0x18,
    PAL_CRYPTO_GET_GENERATOR_CAPACITY   = 0x19,
    PAL_CRYPTO_GENERATOR_IMPORT_KEY     = 0x20,
    PAL_CRYPTO_GENERATOR_ABORT          = 0x21,
    PAL_CRYPTO_AEAD_ENCRYPT             = 0x22,
    PAL_CRYPTO_MAC_SIGN_SETUP,
    PAL_CRYPTO_FREE                     = 0xFE,
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

/**
 * \brief     Supported {cipher type, cipher mode} pairs.
 *
 * \warning   RC4 and DES are considered weak ciphers and their use
 *            constitutes a security risk. Arm recommends considering stronger
 *            ciphers instead.
 */
typedef enum {
    MBEDTLS_CIPHER_NONE = 0,             /**< Placeholder to mark the end of cipher-pair lists. */
    MBEDTLS_CIPHER_NULL,                 /**< The identity stream cipher. */
    MBEDTLS_CIPHER_AES_128_ECB,          /**< AES cipher with 128-bit ECB mode. */
    MBEDTLS_CIPHER_AES_192_ECB,          /**< AES cipher with 192-bit ECB mode. */
    MBEDTLS_CIPHER_AES_256_ECB,          /**< AES cipher with 256-bit ECB mode. */
    MBEDTLS_CIPHER_AES_128_CBC,          /**< AES cipher with 128-bit CBC mode. */
    MBEDTLS_CIPHER_AES_192_CBC,          /**< AES cipher with 192-bit CBC mode. */
    MBEDTLS_CIPHER_AES_256_CBC,          /**< AES cipher with 256-bit CBC mode. */
    MBEDTLS_CIPHER_AES_128_CFB128,       /**< AES cipher with 128-bit CFB128 mode. */
    MBEDTLS_CIPHER_AES_192_CFB128,       /**< AES cipher with 192-bit CFB128 mode. */
    MBEDTLS_CIPHER_AES_256_CFB128,       /**< AES cipher with 256-bit CFB128 mode. */
    MBEDTLS_CIPHER_AES_128_CTR,          /**< AES cipher with 128-bit CTR mode. */
    MBEDTLS_CIPHER_AES_192_CTR,          /**< AES cipher with 192-bit CTR mode. */
    MBEDTLS_CIPHER_AES_256_CTR,          /**< AES cipher with 256-bit CTR mode. */
    MBEDTLS_CIPHER_AES_128_GCM,          /**< AES cipher with 128-bit GCM mode. */
    MBEDTLS_CIPHER_AES_192_GCM,          /**< AES cipher with 192-bit GCM mode. */
    MBEDTLS_CIPHER_AES_256_GCM,          /**< AES cipher with 256-bit GCM mode. */
    MBEDTLS_CIPHER_CAMELLIA_128_ECB,     /**< Camellia cipher with 128-bit ECB mode. */
    MBEDTLS_CIPHER_CAMELLIA_192_ECB,     /**< Camellia cipher with 192-bit ECB mode. */
    MBEDTLS_CIPHER_CAMELLIA_256_ECB,     /**< Camellia cipher with 256-bit ECB mode. */
    MBEDTLS_CIPHER_CAMELLIA_128_CBC,     /**< Camellia cipher with 128-bit CBC mode. */
    MBEDTLS_CIPHER_CAMELLIA_192_CBC,     /**< Camellia cipher with 192-bit CBC mode. */
    MBEDTLS_CIPHER_CAMELLIA_256_CBC,     /**< Camellia cipher with 256-bit CBC mode. */
    MBEDTLS_CIPHER_CAMELLIA_128_CFB128,  /**< Camellia cipher with 128-bit CFB128 mode. */
    MBEDTLS_CIPHER_CAMELLIA_192_CFB128,  /**< Camellia cipher with 192-bit CFB128 mode. */
    MBEDTLS_CIPHER_CAMELLIA_256_CFB128,  /**< Camellia cipher with 256-bit CFB128 mode. */
    MBEDTLS_CIPHER_CAMELLIA_128_CTR,     /**< Camellia cipher with 128-bit CTR mode. */
    MBEDTLS_CIPHER_CAMELLIA_192_CTR,     /**< Camellia cipher with 192-bit CTR mode. */
    MBEDTLS_CIPHER_CAMELLIA_256_CTR,     /**< Camellia cipher with 256-bit CTR mode. */
    MBEDTLS_CIPHER_CAMELLIA_128_GCM,     /**< Camellia cipher with 128-bit GCM mode. */
    MBEDTLS_CIPHER_CAMELLIA_192_GCM,     /**< Camellia cipher with 192-bit GCM mode. */
    MBEDTLS_CIPHER_CAMELLIA_256_GCM,     /**< Camellia cipher with 256-bit GCM mode. */
    MBEDTLS_CIPHER_DES_ECB,              /**< DES cipher with ECB mode. */
    MBEDTLS_CIPHER_DES_CBC,              /**< DES cipher with CBC mode. */
    MBEDTLS_CIPHER_DES_EDE_ECB,          /**< DES cipher with EDE ECB mode. */
    MBEDTLS_CIPHER_DES_EDE_CBC,          /**< DES cipher with EDE CBC mode. */
    MBEDTLS_CIPHER_DES_EDE3_ECB,         /**< DES cipher with EDE3 ECB mode. */
    MBEDTLS_CIPHER_DES_EDE3_CBC,         /**< DES cipher with EDE3 CBC mode. */
    MBEDTLS_CIPHER_BLOWFISH_ECB,         /**< Blowfish cipher with ECB mode. */
    MBEDTLS_CIPHER_BLOWFISH_CBC,         /**< Blowfish cipher with CBC mode. */
    MBEDTLS_CIPHER_BLOWFISH_CFB64,       /**< Blowfish cipher with CFB64 mode. */
    MBEDTLS_CIPHER_BLOWFISH_CTR,         /**< Blowfish cipher with CTR mode. */
    MBEDTLS_CIPHER_ARC4_128,             /**< RC4 cipher with 128-bit mode. */
    MBEDTLS_CIPHER_AES_128_CCM,          /**< AES cipher with 128-bit CCM mode. */
    MBEDTLS_CIPHER_AES_192_CCM,          /**< AES cipher with 192-bit CCM mode. */
    MBEDTLS_CIPHER_AES_256_CCM,          /**< AES cipher with 256-bit CCM mode. */
    MBEDTLS_CIPHER_CAMELLIA_128_CCM,     /**< Camellia cipher with 128-bit CCM mode. */
    MBEDTLS_CIPHER_CAMELLIA_192_CCM,     /**< Camellia cipher with 192-bit CCM mode. */
    MBEDTLS_CIPHER_CAMELLIA_256_CCM,     /**< Camellia cipher with 256-bit CCM mode. */
} mbedtls_cipher_type_t;

/** Supported cipher modes. */
typedef enum {
    MBEDTLS_MODE_NONE = 0,               /**< None. */
    MBEDTLS_MODE_ECB,                    /**< The ECB cipher mode. */
    MBEDTLS_MODE_CBC,                    /**< The CBC cipher mode. */
    MBEDTLS_MODE_CFB,                    /**< The CFB cipher mode. */
    MBEDTLS_MODE_OFB,                    /**< The OFB cipher mode - unsupported. */
    MBEDTLS_MODE_CTR,                    /**< The CTR cipher mode. */
    MBEDTLS_MODE_GCM,                    /**< The GCM cipher mode. */
    MBEDTLS_MODE_STREAM,                 /**< The stream cipher mode. */
    MBEDTLS_MODE_CCM,                    /**< The CCM cipher mode. */
} mbedtls_cipher_mode_t;

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

typedef struct {
    /** Full cipher identifier. For example,
     * MBEDTLS_CIPHER_AES_256_CBC.
     */
    mbedtls_cipher_type_t type;

    /** The cipher mode. For example, MBEDTLS_MODE_CBC. */
    mbedtls_cipher_mode_t mode;

    /** The cipher key length, in bits. This is the
     * default length for variable sized ciphers.
     * Includes parity bits for ciphers like DES.
     */
    unsigned int key_bitlen;

    /** Name of the cipher. */
    const char *name;

    /** IV or nonce size, in Bytes.
     * For ciphers that accept variable IV sizes,
     * this is the recommended size.
     */
    unsigned int iv_size;

    /** Bitflag comprised of MBEDTLS_CIPHER_VARIABLE_IV_LEN and
     *  MBEDTLS_CIPHER_VARIABLE_KEY_LEN indicating whether the
     *  cipher supports variable IV or variable key sizes, respectively.
     */
    int flags;

    /** The block size, in Bytes. */
    unsigned int block_size;

    /** Struct for base cipher information and functions. */
    const mbedtls_cipher_base_t *base;

} mbedtls_cipher_info_t;

/** Type of operation. */
typedef enum {
    MBEDTLS_OPERATION_NONE = -1,
    MBEDTLS_DECRYPT = 0,
    MBEDTLS_ENCRYPT,
} mbedtls_operation_t;

/** Maximum length of any IV, in Bytes. */
#define MBEDTLS_MAX_IV_LENGTH      16
/** Maximum block size of any cipher, in Bytes. */
#define MBEDTLS_MAX_BLOCK_LENGTH   16

#if defined(MBEDTLS_SHA512_C)
#define PSA_HASH_MAX_SIZE 64
#define PSA_HMAC_MAX_HASH_BLOCK_SIZE 128
#else
#define PSA_HASH_MAX_SIZE 32
#define PSA_HMAC_MAX_HASH_BLOCK_SIZE 64
#endif

/**
 * Generic cipher context.
 */
typedef struct {
    /** Information about the associated cipher. */
    const mbedtls_cipher_info_t *cipher_info;

    /** Key length to use. */
    int key_bitlen;

    /** Operation that the key of the context has been
     * initialized for.
     */
    mbedtls_operation_t operation;

#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
    /** Padding functions to use, if relevant for
     * the specific cipher mode.
     */
    void (*add_padding)(unsigned char *output, size_t olen, size_t data_len);
    int (*get_padding)(unsigned char *input, size_t ilen, size_t *data_len);
#endif

    /** Buffer for input that has not been processed yet. */
    unsigned char unprocessed_data[MBEDTLS_MAX_BLOCK_LENGTH];

    /** Number of Bytes that have not been processed yet. */
    size_t unprocessed_len;

    /** Current IV or NONCE_COUNTER for CTR-mode. */
    unsigned char iv[MBEDTLS_MAX_IV_LENGTH];

    /** IV size in Bytes, for ciphers with variable-length IVs. */
    size_t iv_size;

    /** The cipher-specific context. */
    void *cipher_ctx;

#if defined(MBEDTLS_CMAC_C)
    /** CMAC-specific context. */
    mbedtls_cmac_context_t *cmac_ctx;
#endif
} mbedtls_cipher_context_t;

typedef struct {
        /** The hash context. */
        struct psa_hash_operation_s hash_ctx;
        /** The HMAC part of the context. */
        uint8_t opad[PSA_HMAC_MAX_HASH_BLOCK_SIZE];
} psa_hmac_internal_data;

struct psa_mac_operation_s {
    psa_algorithm_t alg;
    unsigned int key_set : 1;
    unsigned int iv_required : 1;
    unsigned int iv_set : 1;
    unsigned int has_input : 1;
    unsigned int is_sign : 1;
    uint8_t mac_size;
    union {
        unsigned dummy; /* Make the union non-empty even with no supported algorithms. */
#if defined(MBEDTLS_MD_C)
        psa_hmac_internal_data hmac;
#endif
#if defined(MBEDTLS_CMAC_C)
        mbedtls_cipher_context_t cmac;
#endif
    } ctx;
};

typedef struct {
    uint8_t *info;
    size_t info_length;
    psa_hmac_internal_data hmac;
    uint8_t prk[PSA_HASH_MAX_SIZE];
    uint8_t output_block[PSA_HASH_MAX_SIZE];
#if PSA_HASH_MAX_SIZE > 0xff
#error "PSA_HASH_MAX_SIZE does not fit in uint8_t"
#endif
    uint8_t offset_in_block;
    uint8_t block_number;
} psa_hkdf_generator_t;

struct psa_crypto_generator_s {
    psa_algorithm_t alg;
    size_t capacity;
    union {
        struct {
            uint8_t *data;
            size_t size;
        } buffer;
#if defined(MBEDTLS_MD_C)
        psa_hkdf_generator_t hkdf;
#endif
    } ctx;
};

typedef struct psa_hash_operation_s psa_hash_operation_t;
typedef struct psa_key_policy_s psa_key_policy_t;
typedef struct psa_mac_operation_s psa_mac_operation_t;
typedef struct psa_crypto_generator_s psa_crypto_generator_t;

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
                             const uint8_t *input, size_t input_length);
psa_status_t psa_hash_verify(psa_hash_operation_t *operation,
                             const uint8_t *hash, size_t hash_length);
psa_status_t psa_hash_finish(psa_hash_operation_t *operation,
                             uint8_t *hash, size_t hash_size, size_t *hash_length);
psa_status_t psa_hash_abort(psa_hash_operation_t *operation);
psa_status_t psa_generate_random(uint8_t *output, size_t output_size);
psa_status_t psa_generate_key(psa_key_slot_t key, psa_key_type_t type, size_t bits,
                              const void *extra, size_t extra_size);
psa_status_t psa_generator_read(psa_crypto_generator_t *generator, uint8_t *output,
                                size_t output_length);
psa_status_t psa_key_derivation(psa_crypto_generator_t *generator, psa_key_slot_t key,
                                psa_algorithm_t alg, const uint8_t *salt, size_t salt_length,
                                const uint8_t *label, size_t label_length, size_t capacity);
psa_status_t psa_get_generator_capacity(const psa_crypto_generator_t *generator,
                                        size_t *capacity);
psa_status_t psa_generator_import_key(psa_key_slot_t key, psa_key_type_t type, size_t bits,
                                      psa_crypto_generator_t *generator);
psa_status_t psa_generator_abort(psa_crypto_generator_t *generator);
psa_status_t psa_aead_encrypt(psa_key_slot_t key, psa_algorithm_t alg, const uint8_t *nonce,
                              size_t nonce_length, const uint8_t *additional_data,
                              size_t additional_data_length, const uint8_t *plaintext,
                              size_t plaintext_length, uint8_t *ciphertext, size_t ciphertext_size,
                              size_t *ciphertext_length);
psa_status_t psa_mac_sign_setup(psa_mac_operation_t *operation, psa_key_slot_t key,
                                psa_algorithm_t alg);
void mbedtls_psa_crypto_free(void);
psa_status_t pal_crypto_function(int type, va_list valist);
#endif /* _PAL_CRYPTO_H_ */
