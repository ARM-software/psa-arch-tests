/** @file
 * Copyright (c) 2019-2024, Arm Limited or its affiliates. All rights reserved.
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

#ifndef _VAL_INITIAL_ATTESTATION_H_
#define _VAL_INITIAL_ATTESTATION_H_

#ifdef INITIAL_ATTESTATION

#include "val.h"
#include "qcbor.h"

#define COSE_HEADER_PARAM_ALG             1
#define COSE_HEADER_PARAM_KID             4

/**
 * The size of X and Y coordinate in 2 parameter style EC public
 * key. Format is as defined in [COSE (RFC 8152)]
 * (https://tools.ietf.org/html/rfc8152) and [SEC 1: Elliptic Curve
 * Cryptography](http://www.secg.org/sec1-v2.pdf).
 *
 * This size is well-known and documented in public standards.
 */
#define T_COSE_CRYPTO_EC_P256_COORD_SIZE  32
#define T_COSE_CRYPTO_SHA256_SIZE         32

#define MAX_ENCODED_COSE_KEY_SIZE \
   (1 + /* 1 byte to encode map */ \
    2 + /* 2 bytes to encode key type */ \
    2 + /* 2 bytes to encode curve */ \
    2 * /* the X and Y coordinates at 32 bytes each */ \
        (T_COSE_CRYPTO_EC_P256_COORD_SIZE + 1 + 2))

#define USEFUL_BUF_MAKE_STACK_UB UsefulBuf_MAKE_STACK_UB

#define MAX_CHALLENGE_SIZE      PSA_INITIAL_ATTEST_CHALLENGE_SIZE_64

#ifdef PSA_ATTESTATION_PROFILE_1

/* Profile-1 token claim values */
#define PSATOKEN_PROFILE_DEFINITION                 (-75000)
#define PSATOKEN_CLIENT_ID                          (-75001)
#define PSATOKEN_SECURITY_LIFECYCLE                 (-75002)
#define PSATOKEN_IMPLEMENTATION_ID                  (-75003)
#define PSATOKEN_BOOT_SEED                          (-75004)
#define PSATOKEN_HW_VERSION                         (-75005)
#define PSATOKEN_SW_COMPONENTS                      (-75006)
#define PSATOKEN_NO_SW_COMPONENTS                   (-75007)
#define PSATOKEN_NONCE                              (-75008)
#define PSATOKEN_UEID                               (-75009)
#define PSATOKEN_VERIFICATION_SERVICE_INDICATOR     (-75010)

#define PROFILE_1_MANDATORY_CLAIMS          7
#define PROFILE_1_MANDATORY_SW_COMPS        1

#endif

#ifdef PSA_ATTESTATION_PROFILE_2

/* Profile-2 token claim values */
#define PSATOKEN_NONCE                              (10)
#define PSATOKEN_UEID                               (256)
#define PSATOKEN_PROFILE_DEFINITION                 (265)
#define PSATOKEN_BOOT_SEED                          (268)
#define PSATOKEN_CLIENT_ID                          (2394)
#define PSATOKEN_SECURITY_LIFECYCLE                 (2395)
#define PSATOKEN_IMPLEMENTATION_ID                  (2396)
#define PSATOKEN_HW_VERSION                         (2398)
#define PSATOKEN_SW_COMPONENTS                      (2399)
#define PSATOKEN_VERIFICATION_SERVICE_INDICATOR     (2400)

#define PROFILE_2_MANDATORY_CLAIMS          6
#define PROFILE_2_MANDATORY_SW_COMPS        2

#endif

/* Common Software component claims between both profiles */
#define PSATOKEN_SW_COMPONENT_TYPE                  (1u)
#define PSATOKEN_SW_COMPONENT_MEASUREMENT           (2u)
#define PSATOKEN_SW_COMPONENT_EPOCH                 (3u)
#define PSATOKEN_SW_COMPONENT_VERSION               (4u)
#define PSATOKEN_SW_COMPONENT_SIGNER_ID             (5u)
#define PSATOKEN_SW_COMPONENT_MEASUREMENT_DESC      (6u)

#define VAL_ATTEST_MIN_ERROR                70

enum attestation_error_code {
    VAL_ATTEST_SUCCESS = 0,
    VAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING = VAL_ATTEST_MIN_ERROR,
    VAL_ATTEST_TOKEN_CHALLENGE_MISMATCH,
    VAL_ATTEST_TOKEN_NOT_SUPPORTED,
    VAL_ATTEST_TOKEN_NOT_ALL_MANDATORY_CLAIMS,
    VAL_ATTEST_HASH_LENGTH_MISMATCH,
    VAL_ATTEST_HASH_MISMATCH,
    VAL_ATTEST_HASH_FAIL,
    VAL_ATTEST_HASH_UNSUPPORTED,
    VAL_ATTEST_HASH_BUFFER_SIZE,
    VAL_ATTEST_ERR_PROTECTED_HEADERS,
    VAL_ATTEST_ERR_SIGN_STRUCT,
    VAL_ATTEST_ERR_KEY_FAIL,
    VAL_ATTEST_ERR_SIGNATURE_FAIL,
    VAL_ATTEST_ERR_CBOR_STRUCTURE,
    VAL_ATTEST_ERR_SMALL_BUFFER,
    VAL_ATTEST_ERROR,
};

struct items_to_get_t {
    int64_t label;
    QCBORItem item;
};

enum attestation_function_code {
    VAL_INITIAL_ATTEST_GET_TOKEN        = 0x1,
    VAL_INITIAL_ATTEST_GET_TOKEN_SIZE   = 0x2,
    VAL_INITIAL_ATTEST_VERIFY_TOKEN     = 0x3,
    VAL_INITIAL_ATTEST_COMPUTE_HASH     = 0x4,
    VAL_INITIAL_ATTEST_VERIFY_WITH_PK   = 0x5,
};

int32_t val_initial_attest_verify_token(uint8_t *challenge, size_t challenge_size,
                                        uint8_t *token, size_t token_size);
#endif /* INITIAL_ATTESTATION */

int32_t val_attestation_function(int type, ...);
#endif /* _VAL_INITIAL_ATTESTATION_H_ */
