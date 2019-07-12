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

#include "qcbor.h"
#include "pal_common.h"
#include "psa/crypto.h"

#define PAL_ATTEST_MIN_ERROR              30

/* NIST P-256 also known as secp256r1 */
#define P_256                             1

#define COSE_HEADER_PARAM_ALG             1
#define COSE_HEADER_PARAM_KID             4

#define COSE_KEY_COMMON_KTY               1
#define COSE_KEY_TYPE_EC2                 2
#define COSE_KEY_PARAM_CRV               -1
#define COSE_KEY_PARAM_X_COORDINATE      -2
#define COSE_KEY_PARAM_Y_COORDINATE      -3
#define COSE_ALGORITHM_ES256             -7
#define COSE_ALG_SHA256_PROPRIETARY      -72000

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
    1 + /* 1 byte to encode map */ \
    2 + /* 2 bytes to encode key type */ \
    2 + /* 2 bytes to encode curve */ \
    2 * /* the X and Y coordinates at 32 bytes each */ \
        (T_COSE_CRYPTO_EC_P256_COORD_SIZE + 1 + 2)
#define USEFUL_BUF_MAKE_STACK_UB UsefulBuf_MAKE_STACK_UB

#define COSE_SIG_CONTEXT_STRING_SIGNATURE1 "Signature1"

/* Private value. Intentionally not documented for Doxygen.
 * This is the size allocated for the encoded protected headers.  It
 * needs to be big enough for make_protected_header() to succeed. It
 * currently sized for one header with an algorithm ID up to 32 bits
 * long -- one byte for the wrapping map, one byte for the label, 5
 * bytes for the ID. If this is made accidentially too small, QCBOR will
 * only return an error, and not overrun any buffers.
 *
 * 9 extra bytes are added, rounding it up to 16 total, in case some
 * other protected header is to be added.
 */
#define T_COSE_SIGN1_MAX_PROT_HEADER (1+1+5+9)

/**
 * This is the size of the first part of the CBOR encoded TBS
 * bytes. It is around 20 bytes. See create_tbs_hash().
 */
#define T_COSE_SIZE_OF_TBS \
    1 + /* For opening the array */ \
    sizeof(COSE_SIG_CONTEXT_STRING_SIGNATURE1) + /* "Signature1" */ \
    2 + /* Overhead for encoding string */ \
    T_COSE_SIGN1_MAX_PROT_HEADER + /* entire protected headers */ \
    3 * ( /* 3 NULL bstrs for fields not used */ \
        1 /* size of a NULL bstr */  \
    )

/*
 CBOR Label for proprietary header indicating short-circuit
 signing was used. Just a random number in the proprietary
 label space */
#define T_COSE_SHORT_CIRCUIT_LABEL              (-8675309)

#define EAT_CBOR_ARM_RANGE_BASE                 (-75000)
#define EAT_CBOR_ARM_LABEL_PROFILE_DEFINITION   (EAT_CBOR_ARM_RANGE_BASE - 0)
#define EAT_CBOR_ARM_LABEL_CLIENT_ID            (EAT_CBOR_ARM_RANGE_BASE - 1)
#define EAT_CBOR_ARM_LABEL_SECURITY_LIFECYCLE   (EAT_CBOR_ARM_RANGE_BASE - 2)
#define EAT_CBOR_ARM_LABEL_IMPLEMENTATION_ID    (EAT_CBOR_ARM_RANGE_BASE - 3)
#define EAT_CBOR_ARM_LABEL_BOOT_SEED            (EAT_CBOR_ARM_RANGE_BASE - 4)
#define EAT_CBOR_ARM_LABEL_HW_VERSION           (EAT_CBOR_ARM_RANGE_BASE - 5)
#define EAT_CBOR_ARM_LABEL_SW_COMPONENTS        (EAT_CBOR_ARM_RANGE_BASE - 6)
#define EAT_CBOR_ARM_LABEL_NO_SW_COMPONENTS     (EAT_CBOR_ARM_RANGE_BASE - 7)
#define EAT_CBOR_ARM_LABEL_NONCE                (EAT_CBOR_ARM_RANGE_BASE - 8)
#define EAT_CBOR_ARM_LABEL_UEID                 (EAT_CBOR_ARM_RANGE_BASE - 9)
#define EAT_CBOR_ARM_LABEL_ORIGINATION          (EAT_CBOR_ARM_RANGE_BASE - 10)

#define CBOR_ARM_TOTAL_CLAIM_INSTANCE           10

#define EAT_CBOR_SW_COMPONENT_TYPE              (1u)
#define EAT_CBOR_SW_COMPONENT_MEASUREMENT       (2u)
#define EAT_CBOR_SW_COMPONENT_EPOCH             (3u)
#define EAT_CBOR_SW_COMPONENT_VERSION           (4u)
#define EAT_CBOR_SW_COMPONENT_SIGNER_ID         (5u)
#define EAT_CBOR_SW_COMPONENT_MEASUREMENT_DESC  (6u)

#define MANDATORY_CLAIM_WITH_SW_COMP           (1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_NONCE)              |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_UEID)               |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_IMPLEMENTATION_ID)  |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_CLIENT_ID)          |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_SECURITY_LIFECYCLE) |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_BOOT_SEED)          |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_SW_COMPONENTS))

#define MANDATORY_CLAIM_NO_SW_COMP             (1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_NONCE)              |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_UEID)               |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_IMPLEMENTATION_ID)  |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_CLIENT_ID)          |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_SECURITY_LIFECYCLE) |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_BOOT_SEED)          |     \
                                                1 << (EAT_CBOR_ARM_RANGE_BASE                      \
                                                    - EAT_CBOR_ARM_LABEL_NO_SW_COMPONENTS))

#define MANDATORY_SW_COMP                      (1 << EAT_CBOR_SW_COMPONENT_MEASUREMENT      |     \
                                                1 << EAT_CBOR_SW_COMPONENT_SIGNER_ID)

#define NULL_USEFUL_BUF_C  NULLUsefulBufC

enum attestation_error_code {
    PAL_ATTEST_SUCCESS = 0,
    PAL_ATTEST_TOKEN_ERR_CBOR_FORMATTING = PAL_ATTEST_MIN_ERROR,
    PAL_ATTEST_TOKEN_CHALLENGE_MISMATCH,
    PAL_ATTEST_TOKEN_NOT_SUPPORTED,
    PAL_ATTEST_TOKEN_NOT_ALL_MANDATORY_CLAIMS,
    PAL_ATTEST_HASH_LENGTH_MISMATCH,
    PAL_ATTEST_HASH_MISMATCH,
    PAL_ATTEST_HASH_FAIL,
    PAL_ATTEST_HASH_UNSUPPORTED,
    PAL_ATTEST_HASH_BUFFER_SIZE,
    PAL_ATTEST_ERR_PROTECTED_HEADERS,
    PAL_ATTEST_ERR_SIGN_STRUCT,
    PAL_ATTEST_ERROR,
};

struct items_to_get_t {
    int64_t label;
    QCBORItem item;
};

int32_t pal_initial_attest_verify_token(uint8_t *challenge, uint32_t challenge_size,
                                        uint8_t *token, uint32_t token_size);
