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

#ifndef _VAL_CRYPTO_H_
#define _VAL_CRYPTO_H_

#include "val.h"
#include <stdarg.h>

#if PSA_IPC_IMPLEMENTED
#include "psa/client.h"
#else
typedef int32_t psa_status_t;
#endif

#define BYTES_TO_BITS(byte)             (byte * 8)
/* Size */
#define AES_16B_KEY_SIZE                16
#define AES_24B_KEY_SIZE                24
#define AES_32B_KEY_SIZE                32
#define AES_18B_KEY_SIZE                18
#define AES_34B_KEY_SIZE                34
#define DES_8B_KEY_SIZE                 8
#define DES3_2KEY_SIZE                  16
#define DES3_3KEY_SIZE                  24
#define SIZE_128B                       128
#define SIZE_256B                       256
#define SIZE_512B                       512
#define BUFFER_SIZE                     1200
#define HASH_64B                        64

/* Key Slot */
#define INVALID_KEY_SLOT                0xDEAD
#define ZERO_KEY_SLOT                   0
#define OCCUPIED_KEY_SLOT               1
#define MAX_KEY_SLOT                    32

/* Key Type */
#define PSA_KEY_TYPE_RAW_DATA                   ((psa_key_type_t)0x50000001)
#define PSA_KEY_TYPE_DERIVE                     ((psa_key_type_t)0x52000000)
#define PSA_KEY_TYPE_CATEGORY_MASK              ((psa_key_type_t)0x70000000)
#define PSA_KEY_TYPE_CATEGORY_SYMMETRIC         ((psa_key_type_t)0x40000000)
#define PSA_KEY_TYPE_PAIR_FLAG                  ((psa_key_type_t)0x10000000)
#define PSA_KEY_TYPE_AES                        ((psa_key_type_t)0x40000001)
#define PSA_KEY_TYPE_DES                        ((psa_key_type_t)0x40000002)
#define PSA_KEY_TYPE_RSA_PUBLIC_KEY             ((psa_key_type_t)0x60010000)
#define PSA_KEY_TYPE_RSA_KEYPAIR                ((psa_key_type_t)0x70010000)
#define PSA_KEY_TYPE_VENDOR_FLAG                ((psa_key_type_t)0x80000000)
#define PSA_KEY_TYPE_ECC_PUBLIC_KEY_BASE        ((psa_key_type_t)0x60030000)
#define PSA_KEY_TYPE_ECC_KEYPAIR_BASE           ((psa_key_type_t)0x70030000)
#define PSA_KEY_TYPE_ECC_CURVE_MASK             ((psa_key_type_t)0x0000ffff)

/* Key Lifetime */
#define PSA_KEY_LIFETIME_VOLATILE               ((psa_key_lifetime_t)0x00000000)
#define PSA_KEY_LIFETIME_PERSISTENT             ((psa_key_lifetime_t)0x00000001)
#define PSA_KEY_LIFETIME_WRITE_ONCE             ((psa_key_lifetime_t)0x7fffffff)
#define PSA_KEY_LIFETIME_INVALID                ((psa_key_lifetime_t)0xffffffff)

/* Algorithm */
#define PSA_ALG_BLOCK_CIPHER_PAD_NONE           ((psa_algorithm_t)0x00000000)
#define PSA_ALG_BLOCK_CIPHER_BASE               ((psa_algorithm_t)0x04000000)
#define PSA_KEY_USAGE_EXPORT                    ((psa_key_usage_t)0x00000001)
#define PSA_KEY_USAGE_ENCRYPT                   ((psa_key_usage_t)0x00000100)
#define PSA_KEY_USAGE_DECRYPT                   ((psa_key_usage_t)0x00000200)
#define PSA_KEY_USAGE_SIGN                      ((psa_key_usage_t)0x00000400)
#define PSA_KEY_USAGE_VERIFY                    ((psa_key_usage_t)0x00000800)
#define PSA_KEY_USAGE_DERIVE                    ((psa_key_usage_t)0x00001000)
#define PSA_ALG_RSA_PKCS1V15_SIGN_BASE          ((psa_algorithm_t)0x10020000)
#define PSA_ALG_ECDSA_BASE                      ((psa_algorithm_t)0x10060000)
#define PSA_ALG_CATEGORY_ASYMMETRIC_ENCRYPTION  ((psa_algorithm_t)0x12000000)
#define PSA_ALG_RSA_PKCS1V15_CRYPT              ((psa_algorithm_t)0x12020000)
#define PSA_ALG_HMAC_BASE                       ((psa_algorithm_t)0x02800000)
#define PSA_ALG_CIPHER_MAC_BASE                 ((psa_algorithm_t)0x02c00000)
#define PSA_ALG_CBC_MAC                         ((psa_algorithm_t)0x02c00001)
#define PSA_ALG_CMAC                            ((psa_algorithm_t)0x02c00002)
#define PSA_ALG_GMAC                            ((psa_algorithm_t)0x02c00003)
#define PSA_ALG_CFB                             ((psa_algorithm_t)0x04c00002)
#define PSA_ALG_CCM                             ((psa_algorithm_t)0x06001001)
#define PSA_ALG_GCM                             ((psa_algorithm_t)0x06001002)
#define PSA_ALG_RSA_PKCS1V15_SIGN_RAW           PSA_ALG_RSA_PKCS1V15_SIGN_BASE
#define PSA_ALG_HKDF_BASE                       ((psa_algorithm_t)0x30000100)
#define PSA_KEY_USAGE_INVALID                   0xFFFFFFFF
#define PSA_ALG_INVALID                         0xFFFFFFFF

/* Hash Algorithm */
#define PSA_ALG_CATEGORY_HASH                   ((psa_algorithm_t)0x01000000)
#define PSA_ALG_HASH_MASK                       ((psa_algorithm_t)0x000000ff)
#define PSA_ALG_MD2                             ((psa_algorithm_t)0x01000001)
#define PSA_ALG_MD4                             ((psa_algorithm_t)0x01000002)
#define PSA_ALG_MD5                             ((psa_algorithm_t)0x01000003)
#define PSA_ALG_RIPEMD160                       ((psa_algorithm_t)0x01000004)
#define PSA_ALG_SHA_1                           ((psa_algorithm_t)0x01000005)
#define PSA_ALG_SHA_224                         ((psa_algorithm_t)0x01000008)
#define PSA_ALG_SHA_256                         ((psa_algorithm_t)0x01000009)
#define PSA_ALG_SHA_384                         ((psa_algorithm_t)0x0100000a)
#define PSA_ALG_SHA_512                         ((psa_algorithm_t)0x0100000b)
#define PSA_ALG_SHA_512_224                     ((psa_algorithm_t)0x0100000c)
#define PSA_ALG_SHA_512_256                     ((psa_algorithm_t)0x0100000d)
#define PSA_ALG_SHA3_224                        ((psa_algorithm_t)0x01000010)
#define PSA_ALG_SHA3_256                        ((psa_algorithm_t)0x01000011)
#define PSA_ALG_SHA3_384                        ((psa_algorithm_t)0x01000012)
#define PSA_ALG_SHA3_512                        ((psa_algorithm_t)0x01000013)

/* Error codes */
#define PSA_ERROR_UNKNOWN_ERROR                 ((psa_status_t)1)
#define PSA_ERROR_NOT_SUPPORTED                 ((psa_status_t)2)
#define PSA_ERROR_NOT_PERMITTED                 ((psa_status_t)3)
#define PSA_ERROR_BUFFER_TOO_SMALL              ((psa_status_t)4)
#define PSA_ERROR_OCCUPIED_SLOT                 ((psa_status_t)5)
#define PSA_ERROR_EMPTY_SLOT                    ((psa_status_t)6)
#define PSA_ERROR_BAD_STATE                     ((psa_status_t)7)
#define PSA_ERROR_INVALID_ARGUMENT              ((psa_status_t)8)
#define PSA_ERROR_INSUFFICIENT_MEMORY           ((psa_status_t)9)
#define PSA_ERROR_INSUFFICIENT_STORAGE          ((psa_status_t)10)
#define PSA_ERROR_COMMUNICATION_FAILURE         ((psa_status_t)11)
#define PSA_ERROR_STORAGE_FAILURE               ((psa_status_t)12)
#define PSA_ERROR_HARDWARE_FAILURE              ((psa_status_t)13)
#define PSA_ERROR_TAMPERING_DETECTED            ((psa_status_t)14)
#define PSA_ERROR_INSUFFICIENT_ENTROPY          ((psa_status_t)15)
#define PSA_ERROR_INVALID_SIGNATURE             ((psa_status_t)16)
#define PSA_ERROR_INVALID_PADDING               ((psa_status_t)17)
#define PSA_ERROR_INSUFFICIENT_CAPACITY         ((psa_status_t)18)

/* Encoding of curve identifiers */
#define PSA_ECC_CURVE_SECT163K1                 ((psa_ecc_curve_t) 0x0001)
#define PSA_ECC_CURVE_SECT163R1                 ((psa_ecc_curve_t) 0x0002)
#define PSA_ECC_CURVE_SECT163R2                 ((psa_ecc_curve_t) 0x0003)
#define PSA_ECC_CURVE_SECT193R1                 ((psa_ecc_curve_t) 0x0004)
#define PSA_ECC_CURVE_SECT193R2                 ((psa_ecc_curve_t) 0x0005)
#define PSA_ECC_CURVE_SECT233K1                 ((psa_ecc_curve_t) 0x0006)
#define PSA_ECC_CURVE_SECT233R1                 ((psa_ecc_curve_t) 0x0007)
#define PSA_ECC_CURVE_SECT239K1                 ((psa_ecc_curve_t) 0x0008)
#define PSA_ECC_CURVE_SECT283K1                 ((psa_ecc_curve_t) 0x0009)
#define PSA_ECC_CURVE_SECT283R1                 ((psa_ecc_curve_t) 0x000a)
#define PSA_ECC_CURVE_SECT409K1                 ((psa_ecc_curve_t) 0x000b)
#define PSA_ECC_CURVE_SECT409R1                 ((psa_ecc_curve_t) 0x000c)
#define PSA_ECC_CURVE_SECT571K1                 ((psa_ecc_curve_t) 0x000d)
#define PSA_ECC_CURVE_SECT571R1                 ((psa_ecc_curve_t) 0x000e)
#define PSA_ECC_CURVE_SECP160K1                 ((psa_ecc_curve_t) 0x000f)
#define PSA_ECC_CURVE_SECP160R1                 ((psa_ecc_curve_t) 0x0010)
#define PSA_ECC_CURVE_SECP160R2                 ((psa_ecc_curve_t) 0x0011)
#define PSA_ECC_CURVE_SECP192K1                 ((psa_ecc_curve_t) 0x0012)
#define PSA_ECC_CURVE_SECP192R1                 ((psa_ecc_curve_t) 0x0013)
#define PSA_ECC_CURVE_SECP224K1                 ((psa_ecc_curve_t) 0x0014)
#define PSA_ECC_CURVE_SECP224R1                 ((psa_ecc_curve_t) 0x0015)
#define PSA_ECC_CURVE_SECP256K1                 ((psa_ecc_curve_t) 0x0016)
#define PSA_ECC_CURVE_SECP256R1                 ((psa_ecc_curve_t) 0x0017)
#define PSA_ECC_CURVE_SECP384R1                 ((psa_ecc_curve_t) 0x0018)
#define PSA_ECC_CURVE_SECP521R1                 ((psa_ecc_curve_t) 0x0019)
#define PSA_ECC_CURVE_BRAINPOOL_P256R1          ((psa_ecc_curve_t) 0x001a)
#define PSA_ECC_CURVE_BRAINPOOL_P384R1          ((psa_ecc_curve_t) 0x001b)
#define PSA_ECC_CURVE_BRAINPOOL_P512R1          ((psa_ecc_curve_t) 0x001c)
#define PSA_ECC_CURVE_CURVE25519                ((psa_ecc_curve_t) 0x001d)
#define PSA_ECC_CURVE_CURVE448                  ((psa_ecc_curve_t) 0x001e)
#define PSA_ECC_CURVE_FFDHE_2048                ((psa_ecc_curve_t) 0x0100)
#define PSA_ECC_CURVE_FFDHE_3072                ((psa_ecc_curve_t) 0x0101)
#define PSA_ECC_CURVE_FFDHE_4096                ((psa_ecc_curve_t) 0x0102)
#define PSA_ECC_CURVE_FFDHE_6144                ((psa_ecc_curve_t) 0x0103)
#define PSA_ECC_CURVE_FFDHE_8192                ((psa_ecc_curve_t) 0x0104)

#define PSA_ALG_HMAC_HASH(hmac_alg)                             \
    (PSA_ALG_CATEGORY_HASH | ((hmac_alg) & PSA_ALG_HASH_MASK))

/* The size of the output hash */
#define PSA_HASH_SIZE(alg)                                      \
    (                                                           \
        PSA_ALG_HMAC_HASH(alg) == PSA_ALG_MD2 ? 16 :            \
        PSA_ALG_HMAC_HASH(alg) == PSA_ALG_MD4 ? 16 :            \
        PSA_ALG_HMAC_HASH(alg) == PSA_ALG_MD5 ? 16 :            \
        PSA_ALG_HMAC_HASH(alg) == PSA_ALG_RIPEMD160 ? 20 :      \
        PSA_ALG_HMAC_HASH(alg) == PSA_ALG_SHA_1 ? 20 :          \
        PSA_ALG_HMAC_HASH(alg) == PSA_ALG_SHA_224 ? 28 :        \
        PSA_ALG_HMAC_HASH(alg) == PSA_ALG_SHA_256 ? 32 :        \
        PSA_ALG_HMAC_HASH(alg) == PSA_ALG_SHA_384 ? 48 :        \
        PSA_ALG_HMAC_HASH(alg) == PSA_ALG_SHA_512 ? 64 :        \
        PSA_ALG_HMAC_HASH(alg) == PSA_ALG_SHA_512_224 ? 28 :    \
        PSA_ALG_HMAC_HASH(alg) == PSA_ALG_SHA_512_256 ? 32 :    \
        PSA_ALG_HMAC_HASH(alg) == PSA_ALG_SHA3_224 ? 28 :       \
        PSA_ALG_HMAC_HASH(alg) == PSA_ALG_SHA3_256 ? 32 :       \
        PSA_ALG_HMAC_HASH(alg) == PSA_ALG_SHA3_384 ? 48 :       \
        PSA_ALG_HMAC_HASH(alg) == PSA_ALG_SHA3_512 ? 64 :       \
        0)

/** The public key type corresponding to a key pair type. */
#define PSA_KEY_TYPE_PUBLIC_KEY_OF_KEYPAIR(type)        \
    ((type) & ~PSA_KEY_TYPE_PAIR_FLAG)

/** Whether a key type is an RSA key (pair or public-only). */
#define PSA_KEY_TYPE_IS_RSA(type)                                       \
    (PSA_KEY_TYPE_PUBLIC_KEY_OF_KEYPAIR(type) == PSA_KEY_TYPE_RSA_PUBLIC_KEY)

/** Whether a key type is an elliptic curve key (pair or public-only). */
#define PSA_KEY_TYPE_IS_ECC(type)                                       \
    ((PSA_KEY_TYPE_PUBLIC_KEY_OF_KEYPAIR(type) &                        \
      ~PSA_KEY_TYPE_ECC_CURVE_MASK) == PSA_KEY_TYPE_ECC_PUBLIC_KEY_BASE)
#define PSA_KEY_TYPE_IS_ECC_KEYPAIR(type)                               \
    (((type) & ~PSA_KEY_TYPE_ECC_CURVE_MASK) ==                         \
     PSA_KEY_TYPE_ECC_KEYPAIR_BASE)
#define PSA_KEY_TYPE_IS_ECC_PUBLIC_KEY(type)                            \
    (((type) & ~PSA_KEY_TYPE_ECC_CURVE_MASK) ==                         \
     PSA_KEY_TYPE_ECC_PUBLIC_KEY_BASE)

/*Macro to build an HMAC algorithm */
#define PSA_ALG_HKDF(hash_alg)                                  \
    (PSA_ALG_HKDF_BASE | ((hash_alg) & PSA_ALG_HASH_MASK))

typedef uint16_t psa_ecc_curve_t;
typedef uint32_t psa_key_usage_t;
typedef uint32_t psa_algorithm_t;
typedef uint32_t psa_key_type_t;
typedef uint32_t psa_key_slot_t;
typedef uint32_t psa_key_lifetime_t;

enum crypto_function_code {
    VAL_CRYPTO_INIT                     = 0x1,
    VAL_CRYPTO_GENERATE_RANDOM          = 0x2,
    VAL_CRYPTO_IMPORT_KEY               = 0x3,
    VAL_CRYPTO_EXPORT_KEY               = 0x4,
    VAL_CRYPTO_EXPORT_PUBLIC_KEY        = 0x5,
    VAL_CRYPTO_DESTROY_KEY              = 0x6,
    VAL_CRYPTO_GET_KEY_INFO             = 0x7,
    VAL_CRYPTO_KEY_POLICY_INIT          = 0x8,
    VAL_CRYPTO_KEY_POLICY_SET_USAGE     = 0x9,
    VAL_CRYPTO_KEY_POLICY_GET_USAGE     = 0xA,
    VAL_CRYPTO_KEY_POLICY_GET_ALGORITHM = 0xB,
    VAL_CRYPTO_SET_KEY_POLICY           = 0xC,
    VAL_CRYPTO_GET_KEY_POLICY           = 0xD,
    VAL_CRYPTO_GET_KEY_INFORMATION      = 0xE,
    VAL_CRYPTO_GET_KEY_LIFETIME         = 0xF,
    VAL_CRYPTO_SET_KEY_LIFETIME         = 0x10,
    VAL_CRYPTO_HASH_SETUP               = 0x11,
    VAL_CRYPTO_HASH_UPDATE              = 0x12,
    VAL_CRYPTO_HASH_VERIFY              = 0x13,
    VAL_CRYPTO_HASH_FINISH              = 0x14,
    VAL_CRYPTO_HASH_ABORT               = 0x15,
    VAL_CRYPTO_GENERATE_KEY             = 0x16,
    VAL_CRYPTO_GENERATOR_READ           = 0x17,
    VAL_CRYPTO_KEY_DERIVATION           = 0x18,
    VAL_CRYPTO_GET_GENERATOR_CAPACITY   = 0x19,
    VAL_CRYPTO_GENERATOR_IMPORT_KEY     = 0x20,
    VAL_CRYPTO_GENERATOR_ABORT          = 0x21,
    VAL_CRYPTO_AEAD_ENCRYPT             = 0x22,
    VAL_CRYPTO_MAC_SIGN_SETUP,
    VAL_CRYPTO_FREE                     = 0xFE,
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

typedef struct mbedtls_cipher_base_t mbedtls_cipher_base_t;

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

val_status_t val_crypto_function(int type, ...);
int32_t val_crypto_key_type_is_raw(psa_key_type_t type);
#endif /* _VAL_CRYPTO_H_ */
