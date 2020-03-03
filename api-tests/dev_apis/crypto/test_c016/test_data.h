/** @file
 * Copyright (c) 2019-2020, Arm Limited or its affiliates. All rights reserved.
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

#include "val_crypto.h"

typedef struct {
    char                    test_desc[75];
    psa_key_handle_t        key_handle;
    psa_key_type_t          key_type;
    psa_key_usage_t         usage;
    psa_algorithm_t         key_alg;
    size_t                  attr_bits;
    uint32_t                expected_bit_length;
    uint32_t                expected_range[2];
    psa_status_t            expected_status;
} test_data;

static test_data check1[] = {
#ifdef ARCH_TEST_CIPER_MODE_CTR
#ifdef ARCH_TEST_AES_128
{"Test psa_generate_key 16 Byte AES\n", 1, PSA_KEY_TYPE_AES,
 PSA_KEY_USAGE_EXPORT, PSA_ALG_CTR, BYTES_TO_BITS(AES_16B_KEY_SIZE),
 BYTES_TO_BITS(AES_16B_KEY_SIZE), {AES_16B_KEY_SIZE, AES_16B_KEY_SIZE}, PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_AES_192
{"Test psa_generate_key 24 Byte AES\n", 2, PSA_KEY_TYPE_AES,
 PSA_KEY_USAGE_EXPORT, PSA_ALG_CTR, BYTES_TO_BITS(AES_24B_KEY_SIZE),
 BYTES_TO_BITS(AES_24B_KEY_SIZE), {AES_24B_KEY_SIZE, AES_24B_KEY_SIZE}, PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_AES_256
{"Test psa_generate_key 32 Byte AES\n", 3, PSA_KEY_TYPE_AES,
 PSA_KEY_USAGE_EXPORT, PSA_ALG_CTR, BYTES_TO_BITS(AES_32B_KEY_SIZE),
 BYTES_TO_BITS(AES_32B_KEY_SIZE), {AES_32B_KEY_SIZE, AES_32B_KEY_SIZE}, PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_1KEY
{"Test psa_generate_key with DES 64 bit key\n", 4, PSA_KEY_TYPE_DES,
 PSA_KEY_USAGE_EXPORT, PSA_ALG_CTR, BYTES_TO_BITS(DES_8B_KEY_SIZE),
 BYTES_TO_BITS(DES_8B_KEY_SIZE), {DES_8B_KEY_SIZE, DES_8B_KEY_SIZE}, PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_2KEY
{"Test psa_generate_key with Triple DES 2-Key\n", 5, PSA_KEY_TYPE_DES,
 PSA_KEY_USAGE_EXPORT, PSA_ALG_CTR, BYTES_TO_BITS(DES3_2KEY_SIZE),
 BYTES_TO_BITS(DES3_2KEY_SIZE), {DES3_2KEY_SIZE, DES3_2KEY_SIZE}, PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_3KEY
{"Test psa_generate_key with Triple DES 3-Key\n", 6, PSA_KEY_TYPE_DES,
 PSA_KEY_USAGE_EXPORT, PSA_ALG_CTR, BYTES_TO_BITS(DES3_3KEY_SIZE),
 BYTES_TO_BITS(DES3_3KEY_SIZE), {DES3_3KEY_SIZE, DES3_3KEY_SIZE}, PSA_SUCCESS
},
#endif

#endif

{"Test psa_generate_key with RSA 2048 Keypair\n", 7, PSA_KEY_TYPE_RSA_KEY_PAIR,
 PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_SIGN | PSA_KEY_USAGE_VERIFY, PSA_ALG_RSA_PKCS1V15_SIGN_RAW,
 2048, 2048, {1190, 1194}, PSA_SUCCESS
},

#ifdef ARCH_TEST_ECC_CURVE_SECP224R1
#ifdef ARCH_TEST_ASYMMETRIC_ENCRYPTION
{"Test psa_generate_key with ECC KeyPair\n", 8,
 PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_CURVE_SECP224R1),
 PSA_KEY_USAGE_EXPORT, PSA_ALG_CATEGORY_ASYMMETRIC_ENCRYPTION, 224,
 224, {28, 28}, PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_RSA_PKCS1V15_SIGN_RAW
#ifdef ARCH_TEST_RSA_2048
{"Test psa_generate_key with RSA 2048 Public key\n", 9, PSA_KEY_TYPE_RSA_PUBLIC_KEY,
 PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_SIGN | PSA_KEY_USAGE_VERIFY, PSA_ALG_RSA_PKCS1V15_SIGN_RAW,
 2048, 2048, {1190, 1194}, PSA_ERROR_NOT_SUPPORTED
},
#endif
#endif

{"Test psa_generate_key with invalid key type\n", 10, 0,
 PSA_KEY_USAGE_EXPORT, PSA_ALG_CTR, BYTES_TO_BITS(AES_16B_KEY_SIZE),
 BYTES_TO_BITS(AES_16B_KEY_SIZE), {AES_16B_KEY_SIZE, AES_16B_KEY_SIZE}, PSA_ERROR_NOT_SUPPORTED
},

{"Test psa_generate_key with invalid usage flags\n", 11, PSA_KEY_TYPE_AES,
 PSA_KEY_USAGE_INVALID, PSA_ALG_CTR, BYTES_TO_BITS(AES_16B_KEY_SIZE),
 BYTES_TO_BITS(AES_16B_KEY_SIZE), {AES_16B_KEY_SIZE, AES_16B_KEY_SIZE}, PSA_ERROR_INVALID_ARGUMENT
},
};
