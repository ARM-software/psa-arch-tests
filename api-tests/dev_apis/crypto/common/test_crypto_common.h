/** @file
 * Copyright (c) 2020-2023, Arm Limited or its affiliates. All rights reserved.
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

#ifndef _TEST_CRYPTO_COMMON_H_
#define _TEST_CRYPTO_COMMON_H_

#include "val_crypto.h"

#define INPUT_BYTES_DATA_LEN           16
#define ECDH_SECP_256_R1_PRV_KEY_LEN   32
#define ECDH_SECP_256_R1_PUB_KEY_LEN   65
#define ECDH_SECP_384_R1_PRV_KEY_LEN   48
#define ECDH_SECP_384_R1_PUB_KEY_LEN   97

#define AEAD_CIPHERTEXT_LEN_1          39
#define AEAD_CIPHERTEXT_LEN_2          40
#define AEAD_CIPHERTEXT_LEN_3          28
#define AEAD_CIPHERTEXT_LEN_4          40
#define AEAD_CIPHERTEXT_LEN_5          16
#define AEAD_CIPHERTEXT_LEN_6          40

#define INPUT_SALT_LEN                 4
#define INPUT_INFO_LEN                 4
#define INPUT_SEED_LEN                 4
#define INPUT_LABEL_LEN                5

#define PSA_ERROR_PROGRAMMER_ERROR      ((psa_status_t)-129)

/* min and max finding macro */
#define MIN(a, b) (((a) < (b))?(a):(b))
#define MAX(a, b) (((a) > (b))?(a):(b))

extern uint8_t key_data[];

extern uint8_t rsa_128_key_pair[];
extern uint8_t rsa_128_key_data[];
extern uint8_t rsa_256_key_pair[];
extern uint8_t rsa_256_key_data[];

extern uint8_t ec_key_data[];
extern uint8_t ec_key_pair[];
extern uint8_t ec_keypair_deterministic[];

extern uint8_t md2_hash[];
extern uint8_t md4_hash[];
extern uint8_t md5_hash[];
extern uint8_t ripemd_160_hash[];
extern uint8_t sha_1_hash[];
extern uint8_t sha_224_hash[];
extern uint8_t sha_256_hash[];
extern uint8_t sha_256_incorrect_hash[];
extern uint8_t sha_384_hash[];
extern uint8_t sha_512_hash[];

extern uint8_t ecdh_secp_256_r1_prv_key[];
extern uint8_t ecdh_secp_256_r1_pub_key[];
extern uint8_t ecdh_secp_384_r1_prv_key[];
extern uint8_t ecdh_secp_384_r1_pub_key[];

extern uint8_t nonce[];
extern uint8_t additional_data[];
extern uint8_t plaintext[];
extern uint8_t aead_ciphertext_1[AEAD_CIPHERTEXT_LEN_1];
extern uint8_t aead_ciphertext_2[AEAD_CIPHERTEXT_LEN_2];
extern uint8_t aead_invalid_ciphertext_2[AEAD_CIPHERTEXT_LEN_2];
extern uint8_t aead_ciphertext_3[AEAD_CIPHERTEXT_LEN_3];
extern uint8_t aead_ciphertext_4[AEAD_CIPHERTEXT_LEN_4];
extern uint8_t aead_ciphertext_5[AEAD_CIPHERTEXT_LEN_5];
extern uint8_t aead_ciphertext_6[AEAD_CIPHERTEXT_LEN_6];

extern uint8_t hash_input;
extern uint8_t input_bytes_data[INPUT_BYTES_DATA_LEN];
extern uint8_t input_salt[INPUT_SALT_LEN];
extern uint8_t input_info[INPUT_INFO_LEN];
extern uint8_t input_seed[INPUT_SEED_LEN];
extern uint8_t input_label[INPUT_LABEL_LEN];

extern unsigned char iv[];

extern unsigned char ciphertext_1[];
extern unsigned char ciphertext_2[];
extern unsigned char ciphertext_3[];
extern unsigned char ciphertext_4[];
extern unsigned char ciphertext_5[];
extern unsigned char ciphertext_6[];

extern uint8_t salt[];
extern unsigned char ciphertext_7[];
extern unsigned char ciphertext_8[];
extern unsigned char ciphertext_9[];

extern uint8_t signature_1[];
extern uint8_t signature_2[];
extern uint8_t signature_2_invalid[];
extern uint8_t signature_3[];
extern uint8_t signature_4[];
extern uint8_t signature_4_invalid[];
extern uint8_t signature_5[];
extern uint8_t signature_6[];

extern uint8_t hmac_sha224[];
extern uint8_t hmac_sha224_invalid[];
extern uint8_t hmac_sha256[];
extern uint8_t hmac_sha512[];
extern uint8_t cmac_aes_128[];

extern uint8_t expected_output[BUFFER_SIZE];

extern void crypto_common_exit_action(void);

#endif /* _TEST_CRYPTO_COMMON_H_ */
