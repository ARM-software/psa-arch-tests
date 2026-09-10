/** @file
 * Copyright (c) 2018-2021, 2026, Arm Limited or its affiliates. All rights reserved.
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

#include "test_crypto_common.h"

typedef struct {
    char                    test_desc[75];
    psa_key_type_t          type;
    const uint8_t          *data;
    size_t                  data_length;
    size_t                  bits;
    size_t                  expected_bits;
    psa_key_usage_t         usage_flags;
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_CIPHER_MODE_CTR
#ifdef ARCH_TEST_AES_128
{
    .test_desc       = "Test psa_import_key 16 bytes AES\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .bits            = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},

#endif

#ifdef ARCH_TEST_AES_192
{
    .test_desc       = "Test psa_import_key 24 bytes AES\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_24B_KEY_SIZE,
    .bits            = BYTES_TO_BITS(AES_24B_KEY_SIZE),
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_AES_256
{
    .test_desc       = "Test psa_import_key 32 bytes AES\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_32B_KEY_SIZE,
    .bits            = BYTES_TO_BITS(AES_32B_KEY_SIZE),
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_RSA_PKCS1V15_SIGN_RAW
#ifdef ARCH_TEST_RSA_2048
{
    .test_desc       = "Test psa_import_key 2048 RSA public key\n",
    .type            = PSA_KEY_TYPE_RSA_PUBLIC_KEY,
    .data            = rsa_256_key_data,
    .data_length     = 270,
    .bits            = BYTES_TO_BITS(256),
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},

{
    .test_desc       = "Test psa_import_key with RSA 2048 keypair\n",
    .type            = PSA_KEY_TYPE_RSA_KEY_PAIR,
    .data            = rsa_256_key_pair,
    .data_length     = 1193,
    .bits            = BYTES_TO_BITS(256),
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_CIPHER_MODE_CTR
#ifdef ARCH_TEST_DES_1KEY
{
    .test_desc       = "Test psa_import_key with DES 8 bytes key\n",
    .type            = PSA_KEY_TYPE_DES,
    .data            = key_data,
    .data_length     = DES_8B_KEY_SIZE,
    .bits            = BYTES_TO_BITS(DES_8B_KEY_SIZE),
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_2KEY
{
    .test_desc       = "Test psa_import_key with Triple DES 2-Key\n",
    .type            = PSA_KEY_TYPE_DES,
    .data            = key_data,
    .data_length     = DES3_2B_KEY_SIZE,
    .bits            = BYTES_TO_BITS(DES3_2B_KEY_SIZE),
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_DES_3KEY
{
    .test_desc       = "Test psa_import_key with Triple DES 3-Key\n",
    .type            = PSA_KEY_TYPE_DES,
    .data            = key_data,
    .data_length     = DES3_3B_KEY_SIZE,
    .bits            = BYTES_TO_BITS(DES3_3B_KEY_SIZE),
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_ECDSA
#ifdef ARCH_TEST_ECC_CURVE_SECP256R1
{
    .test_desc       = "Test psa_import_key with EC Public key\n",
    .type            = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1),
    .data            = ec_key_data,
    .data_length     = 65,
    .bits            = 256,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_ECC_CURVE_SECP224R1
{
    .test_desc       = "Test psa_import_key with EC keypair\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data            = ec_key_pair,
    .data_length     = 28,
    .bits            = 224,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_CIPHER_MODE_CTR
#ifdef ARCH_TEST_AES
{
    .test_desc       = "Test psa_import_key 16 bytes AES with invalid bits\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .bits            = BYTES_TO_BITS(AES_16B_KEY_SIZE)+1,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},

{
    .test_desc       = "Test psa_import_key with key data greater than the algorithm size\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_34B_KEY_SIZE,
    .bits            = BYTES_TO_BITS(AES_34B_KEY_SIZE),
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},

{
    .test_desc       = "Test psa_import_key with incorrect key data size\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_18B_KEY_SIZE,
    .bits            = BYTES_TO_BITS(AES_18B_KEY_SIZE),
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
#endif

{
    .test_desc       = "Test psa_import_key with invalid key type value\n",
    .type            = PSA_KEY_TYPE_NONE,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .bits            = BYTES_TO_BITS(AES_16B_KEY_SIZE),
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_ERROR_NOT_SUPPORTED
},
#endif

#ifdef ARCH_TEST_CIPHER_MODE_CTR
#ifdef ARCH_TEST_AES_128
{
    .test_desc       = "Test psa_import_key AES-128 with bits=0 (inferred)\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_16B_KEY_SIZE,
    .bits            = 0,
    .expected_bits   = 128,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},
#endif
#ifdef ARCH_TEST_AES_256
{
    .test_desc       = "Test psa_import_key AES-256 with bits=0 (inferred)\n",
    .type            = PSA_KEY_TYPE_AES,
    .data            = key_data,
    .data_length     = AES_32B_KEY_SIZE,
    .bits            = 0,
    .expected_bits   = 256,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_RSA_PKCS1V15_SIGN_RAW
#ifdef ARCH_TEST_RSA_2048
{
    .test_desc       = "Test psa_import_key RSA-2048 pub with bits=0 (inferred)\n",
    .type            = PSA_KEY_TYPE_RSA_PUBLIC_KEY,
    .data            = rsa_256_key_data,
    .data_length     = 270,
    .bits            = 0,
    .expected_bits   = 2048,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},
#endif
#endif

#ifdef ARCH_TEST_ECDSA
#ifdef ARCH_TEST_ECC_CURVE_SECP256R1
{
    .test_desc       = "Test psa_import_key P-256 pub with bits=0 (inferred)\n",
    .type            = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1),
    .data            = ec_key_data,
    .data_length     = 65,
    .bits            = 0,
    .expected_bits   = 256,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},
{
    .test_desc       = "Test psa_import_key P-256 pub with bits=224 (mismatch)\n",
    .type            = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1),
    .data            = ec_key_data,
    .data_length     = 65,
    .bits            = 224,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
#endif

#ifdef ARCH_TEST_ECC_CURVE_SECP384R1
{
    .test_desc       = "Test psa_import_key P-384 pub bits=384\n",
    .type            = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1),
    .data            = ecdsa_secp384r1_sha384_pub,
    .data_length     = 97,
    .bits            = 384,
    .expected_bits   = 384,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},
{
    .test_desc       = "Test psa_import_key P-384 pub bits=0 (inferred)\n",
    .type            = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1),
    .data            = ecdsa_secp384r1_sha384_pub,
    .data_length     = 97,
    .bits            = 0,
    .expected_bits   = 384,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},
{
    .test_desc       = "Test psa_import_key P-384 pub bits=256 (mismatch)\n",
    .type            = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1),
    .data            = ecdsa_secp384r1_sha384_pub,
    .data_length     = 97,
    .bits            = 256,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
{
    .test_desc       = "Test psa_import_key P-384 keypair bits=384\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data            = ecdsa_secp384r1_sha384_priv,
    .data_length     = 48,
    .bits            = 384,
    .expected_bits   = 384,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},
{
    .test_desc       = "Test psa_import_key P-384 keypair bits=0 (inferred)\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data            = ecdsa_secp384r1_sha384_priv,
    .data_length     = 48,
    .bits            = 0,
    .expected_bits   = 384,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},
#endif

#ifdef ARCH_TEST_ECC_CURVE_SECP521R1
/* The P-521 cases below are the regression gate for the (data_length - 1) * 4 derivation
 * bug: 66 bytes coordinate -> formula yields 528 instead of 521 */
{
    .test_desc       = "Test psa_import_key P-521 pub bits=521 (noisy bug)\n",
    .type            = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1),
    .data            = ec_secp521_pub_data,
    .data_length     = 133,
    .bits            = 521,
    .expected_bits   = 521,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},
{
    .test_desc       = "Test psa_import_key P-521 pub bits=0 (silent bug)\n",
    .type            = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1),
    .data            = ec_secp521_pub_data,
    .data_length     = 133,
    .bits            = 0,
    .expected_bits   = 521,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},
{
    .test_desc       = "Test psa_import_key P-521 pub bits=528 (not spec)\n",
    .type            = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1),
    .data            = ec_secp521_pub_data,
    .data_length     = 133,
    .bits            = 528,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
{
    .test_desc       = "Test psa_import_key P-521 pub bits=256 (way wrong)\n",
    .type            = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1),
    .data            = ec_secp521_pub_data,
    .data_length     = 133,
    .bits            = 256,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
{
    .test_desc       = "Test psa_import_key P-521 keypair bits=521\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data            = ec_secp521_priv_data,
    .data_length     = 66,
    .bits            = 521,
    .expected_bits   = 521,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},
{
    .test_desc       = "Test psa_import_key P-521 keypair bits=0 (inferred)\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data            = ec_secp521_priv_data,
    .data_length     = 66,
    .bits            = 0,
    .expected_bits   = 521,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_SUCCESS
},
{
    .test_desc       = "Test psa_import_key P-521 keypair bits=528 (not spec)\n",
    .type            = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
    .data            = ec_secp521_priv_data,
    .data_length     = 66,
    .bits            = 528,
    .usage_flags     = PSA_KEY_USAGE_EXPORT,
    .expected_status = PSA_ERROR_INVALID_ARGUMENT
},
#endif
#endif /* ARCH_TEST_ECDSA */
};
