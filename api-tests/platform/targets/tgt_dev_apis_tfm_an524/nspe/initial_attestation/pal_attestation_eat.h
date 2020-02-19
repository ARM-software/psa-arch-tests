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

#ifndef _PAL_ATTESTATION_EAT_H_
#define _PAL_ATTESTATION_EAT_H_

#include "qcbor.h"
#include "pal_common.h"

#define PAL_ATTEST_MIN_ERROR              30

#define COSE_ALGORITHM_ES256             -7
#define COSE_ALG_SHA256_PROPRIETARY      -72000

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
    3 * (/* 3 NULL bstrs for fields not used */ \
        1 /* size of a NULL bstr */  \
    )
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
    PAL_ATTEST_ERR_KEY_FAIL,
    PAL_ATTEST_ERR_SIGNATURE_FAIL,
    PAL_ATTEST_ERR_CBOR_STRUCTURE,
    PAL_ATTEST_ERR_SMALL_BUFFER,
    PAL_ATTEST_ERROR,
};

#endif /* _PAL_ATTESTATION_EAT_H_ */
