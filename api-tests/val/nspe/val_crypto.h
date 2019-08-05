/** @file
 * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
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
#define SIZE_32B                        32
#define SIZE_50B                        50
#define SIZE_128B                       128
#define SIZE_256B                       256
#define SIZE_512B                       512
#define BUFFER_SIZE                     1200
#define HASH_64B                        64
#define BUFFER_SIZE_HIGH                5130

/* Key Slot */
#define INVALID_KEY_SLOT                0xDEAD
#define ZERO_KEY_SLOT                   0
#define OCCUPIED_KEY_SLOT               1
#define MAX_KEY_SLOT                    32

#define PSA_KEY_LIFETIME_INVALID        0xFFFFFFFF
#define PSA_KEY_USAGE_INVALID           0xFFFFFFFF
#define PSA_HASH_ALG_INVALID            0x01FFFFFF
#define PSA_ALG_INVALID                 0xFFFFFFFF
#define PSA_KEY_DERIVATION_STEP_INVALID 0x0000FFFF

enum crypto_function_code {
    VAL_CRYPTO_INIT                          = 0x1,
    VAL_CRYPTO_GENERATE_RANDOM               = 0x2,
    VAL_CRYPTO_IMPORT_KEY                    = 0x3,
    VAL_CRYPTO_EXPORT_KEY                    = 0x4,
    VAL_CRYPTO_EXPORT_PUBLIC_KEY             = 0x5,
    VAL_CRYPTO_DESTROY_KEY                   = 0x6,
    VAL_CRYPTO_HASH_SETUP                    = 0x7,
    VAL_CRYPTO_HASH_UPDATE                   = 0x8,
    VAL_CRYPTO_HASH_VERIFY                   = 0x9,
    VAL_CRYPTO_HASH_FINISH                   = 0xA,
    VAL_CRYPTO_HASH_ABORT                    = 0xB,
    VAL_CRYPTO_GENERATE_KEY                  = 0xC,
    VAL_CRYPTO_AEAD_ENCRYPT                  = 0xD,
    VAL_CRYPTO_AEAD_DECRYPT                  = 0xE,
    VAL_CRYPTO_MAC_SIGN_SETUP                = 0xF,
    VAL_CRYPTO_MAC_UPDATE                    = 0x10,
    VAL_CRYPTO_MAC_SIGN_FINISH               = 0x11,
    VAL_CRYPTO_MAC_VERIFY_SETUP              = 0x12,
    VAL_CRYPTO_MAC_VERIFY_FINISH             = 0x13,
    VAL_CRYPTO_MAC_ABORT                     = 0x14,
    VAL_CRYPTO_ASYMMTERIC_ENCRYPT            = 0x15,
    VAL_CRYPTO_ASYMMTERIC_DECRYPT            = 0x16,
    VAL_CRYPTO_CIPHER_ENCRYPT_SETUP          = 0x17,
    VAL_CRYPTO_CIPHER_DECRYPT_SETUP          = 0x18,
    VAL_CRYPTO_CIPHER_GENERATE_IV            = 0x19,
    VAL_CRYPTO_CIPHER_SET_IV                 = 0x1A,
    VAL_CRYPTO_CIPHER_UPDATE                 = 0x1B,
    VAL_CRYPTO_CIPHER_FINISH                 = 0x1C,
    VAL_CRYPTO_CIPHER_ABORT                  = 0x1D,
    VAL_CRYPTO_ASYMMTERIC_SIGN               = 0x1E,
    VAL_CRYPTO_ASYMMTERIC_VERIFY             = 0x1F,
    VAL_CRYPTO_COPY_KEY                      = 0x20,
    VAL_CRYPTO_SET_KEY_TYPE                  = 0x21,
    VAL_CRYPTO_SET_KEY_BITS                  = 0x22,
    VAL_CRYPTO_GET_KEY_ATTRIBUTES            = 0x23,
    VAL_CRYPTO_GET_KEY_TYPE                  = 0x24,
    VAL_CRYPTO_SET_KEY_USAGE_FLAGS           = 0x25,
    VAL_CRYPTO_RESET_KEY_ATTRIBUTES          = 0x26,
    VAL_CRYPTO_SET_KEY_ID                    = 0x27,
    VAL_CRYPTO_SET_KEY_LIFETIME              = 0x28,
    VAL_CRYPTO_SET_KEY_ALGORITHM             = 0x29,
    VAL_CRYPTO_GET_KEY_ID                    = 0x2A,
    VAL_CRYPTO_GET_KEY_LIFETIME              = 0x2B,
    VAL_CRYPTO_GET_KEY_USAGE_FLAGS           = 0x2C,
    VAL_CRYPTO_GET_KEY_ALGORITHM             = 0x2D,
    VAL_CRYPTO_GET_KEY_BITS                  = 0x2E,
    VAL_CRYPTO_HASH_COMPUTE                  = 0x2F,
    VAL_CRYPTO_HASH_COMPARE                  = 0x30,
    VAL_CRYPTO_KEY_DERIVATION_SETUP          = 0x31,
    VAL_CRYPTO_KEY_DERIVATION_ABORT          = 0x32,
    VAL_CRYPTO_RAW_KEY_AGREEMENT             = 0x33,
    VAL_CRYPTO_KEY_DERIVATION_INPUT_BYTES    = 0x34,
    VAL_CRYPTO_KEY_DERIVATION_INPUT_KEY      = 0x35,
    VAL_CRYPTO_KEY_DERIVATION_KEY_AGREEMENT  = 0x36,
    VAL_CRYPTO_KEY_DERIVATION_OUTPUT_BYTES   = 0x37,
    VAL_CRYPTO_KEY_DERIVATION_OUTPUT_KEY     = 0x38,
    VAL_CRYPTO_KEY_DERIVATION_SET_CAPACITY   = 0x39,
    VAL_CRYPTO_KEY_DERIVATION_GET_CAPACITY   = 0x3A,
    VAL_CRYPTO_HASH_CLONE                    = 0x3B,
    VAL_CRYPTO_MAC_COMPUTE                   = 0x3C,
    VAL_CRYPTO_MAC_VERIFY                    = 0x3D,
    VAL_CRYPTO_CIPHER_ENCRYPT                = 0x3F,
    VAL_CRYPTO_CIPHER_DECRYPT                = 0x40,
    VAL_CRYPTO_OPEN_KEY                      = 0x41,
    VAL_CRYPTO_CLOSE_KEY                     = 0x42,
    VAL_CRYPTO_AEAD_ENCRYPT_SETUP            = 0x43,
    VAL_CRYPTO_AEAD_DECRYPT_SETUP            = 0x44,
    VAL_CRYPTO_AEAD_GENERATE_NONCE           = 0x45,
    VAL_CRYPTO_AEAD_SET_NONCE                = 0x46,
    VAL_CRYPTO_AEAD_SET_LENGTHS              = 0x47,
    VAL_CRYPTO_AEAD_UPDATE_AD                = 0x48,
    VAL_CRYPTO_AEAD_UPDATE                   = 0x49,
    VAL_CRYPTO_AEAD_FINISH                   = 0x4A,
    VAL_CRYPTO_AEAD_VERIFY                   = 0x4B,
    VAL_CRYPTO_AEAD_ABORT                    = 0x4C,
    VAL_CRYPTO_RESET                         = 0xFD,
    VAL_CRYPTO_FREE                          = 0xFE,
};

int32_t val_crypto_function(int type, ...);
#endif /* _VAL_CRYPTO_H_ */
