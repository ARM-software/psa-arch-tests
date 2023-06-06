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

#ifndef MANAGER_CRYPTO_CONFIG_H
#define MANAGER_CRYPTO_CONFIG_H

#include <psa_adac_config.h>

#define MBEDTLS_PSA_CRYPTO_C

/* System support */
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_MEMORY_BUFFER_ALLOC_C

#if defined(PSA_ADAC_RSA3072) || defined(PSA_ADAC_RSA4096)
#define MBEDTLS_RSA_C
#define MBEDTLS_PKCS1_V21
#define MBEDTLS_OID_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_GENPRIME

/* Support RSA key sizes up to 4096 bit */
#define MBEDTLS_MPI_MAX_SIZE 512
#endif

/* PSA ADAC */
#if defined(PSA_ADAC_EC_P256) || defined(PSA_ADAC_EC_P521)
#define MBEDTLS_ECDSA_C
#define MBEDTLS_ECDSA_DETERMINISTIC
#define MBEDTLS_ECP_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_HMAC_DRBG_C
#if defined(PSA_ADAC_EC_P256)
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#endif
#if defined(PSA_ADAC_EC_P521)
#define MBEDTLS_ECP_DP_SECP521R1_ENABLED
#endif
#endif

#ifdef PSA_ADAC_CMAC
#define MBEDTLS_CMAC_C
#endif

#ifdef PSA_ADAC_HMAC
#define MBEDTLS_HKDF_C
#endif

/* Needed by PSA Crypto API Implementation */
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_ENTROPY_C
#define MBEDTLS_AES_C

#define MBEDTLS_MD_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_SHA512_C

/* Add CCM on Windows */
#if defined(_MSC_VER)
#define MBEDTLS_CCM_C
#endif

/* Needed by manager */
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_PK_WRITE_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_PK_C
#define MBEDTLS_PEM_PARSE_C
#define MBEDTLS_BASE64_C
#define MBEDTLS_CIPHER_C
#define MBEDTLS_FS_IO

/* Optimizations */
#define MBEDTLS_AESNI_C
#define MBEDTLS_HAVE_ASM

#include "mbedtls/check_config.h"

#endif /* MBEDTLS_CONFIG_BOOT_H */
