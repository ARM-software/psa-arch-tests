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

#ifndef _PAL_CRYPTO_INTF_H_
#define _PAL_CRYPTO_INTF_H_

#include "pal_common.h"

enum crypto_function_code {
    PAL_CRYPTO_INIT                             = 0x1,
    PAL_CRYPTO_GENERATE_RANDOM                  = 0x2,
    PAL_CRYPTO_IMPORT_KEY                       = 0x3,
    PAL_CRYPTO_EXPORT_KEY                       = 0x4,
    PAL_CRYPTO_EXPORT_PUBLIC_KEY                = 0x5,
    PAL_CRYPTO_DESTROY_KEY                      = 0x6,
    PAL_CRYPTO_HASH_SETUP                       = 0x7,
    PAL_CRYPTO_HASH_UPDATE                      = 0x8,
    PAL_CRYPTO_HASH_VERIFY                      = 0x9,
    PAL_CRYPTO_HASH_FINISH                      = 0xA,
    PAL_CRYPTO_HASH_ABORT                       = 0xB,
    PAL_CRYPTO_GENERATE_KEY                     = 0xC,
    PAL_CRYPTO_AEAD_ENCRYPT                     = 0xD,
    PAL_CRYPTO_AEAD_DECRYPT                     = 0xE,
    PAL_CRYPTO_MAC_SIGN_SETUP                   = 0xF,
    PAL_CRYPTO_MAC_UPDATE                       = 0x10,
    PAL_CRYPTO_MAC_SIGN_FINISH                  = 0x11,
    PAL_CRYPTO_MAC_VERIFY_SETUP                 = 0x12,
    PAL_CRYPTO_MAC_VERIFY_FINISH                = 0x13,
    PAL_CRYPTO_MAC_ABORT                        = 0x14,
    PAL_CRYPTO_ASYMMTERIC_ENCRYPT               = 0x15,
    PAL_CRYPTO_ASYMMTERIC_DECRYPT               = 0x16,
    PAL_CRYPTO_CIPHER_ENCRYPT_SETUP             = 0x17,
    PAL_CRYPTO_CIPHER_DECRYPT_SETUP             = 0x18,
    PAL_CRYPTO_CIPHER_GENERATE_IV               = 0x19,
    PAL_CRYPTO_CIPHER_SET_IV                    = 0x1A,
    PAL_CRYPTO_CIPHER_UPDATE                    = 0x1B,
    PAL_CRYPTO_CIPHER_FINISH                    = 0x1C,
    PAL_CRYPTO_CIPHER_ABORT                     = 0x1D,
    PAL_CRYPTO_ASYMMTERIC_SIGN                  = 0x1E,
    PAL_CRYPTO_ASYMMTERIC_VERIFY                = 0x1F,
    PAL_CRYPTO_COPY_KEY                         = 0x20,
    PAL_CRYPTO_SET_KEY_TYPE                     = 0x21,
    PAL_CRYPTO_SET_KEY_BITS                     = 0x22,
    PAL_CRYPTO_GET_KEY_ATTRIBUTES               = 0x23,
    PAL_CRYPTO_GET_KEY_TYPE                     = 0x24,
    PAL_CRYPTO_SET_KEY_USAGE_FLAGS              = 0x25,
    PAL_CRYPTO_RESET_KEY_ATTRIBUTES             = 0x26,
    PAL_CRYPTO_SET_KEY_ID                       = 0x27,
    PAL_CRYPTO_SET_KEY_LIFETIME                 = 0x28,
    PAL_CRYPTO_SET_KEY_ALGORITHM                = 0x29,
    PAL_CRYPTO_GET_KEY_ID                       = 0x2A,
    PAL_CRYPTO_GET_KEY_LIFETIME                 = 0x2B,
    PAL_CRYPTO_GET_KEY_USAGE_FLAGS              = 0x2C,
    PAL_CRYPTO_GET_KEY_ALGORITHM                = 0x2D,
    PAL_CRYPTO_GET_KEY_BITS                     = 0x2E,
    PAL_CRYPTO_HASH_COMPUTE                     = 0x2F,
    PAL_CRYPTO_HASH_COMPARE                     = 0x30,
    PAL_CRYPTO_KEY_DERIVATION_SETUP             = 0x31,
    PAL_CRYPTO_KEY_DERIVATION_ABORT             = 0x32,
    PAL_CRYPTO_RAW_KEY_AGREEMENT                = 0x33,
    PAL_CRYPTO_KEY_DERIVATION_INPUT_BYTES       = 0x34,
    PAL_CRYPTO_KEY_DERIVATION_INPUT_KEY         = 0x35,
    PAL_CRYPTO_KEY_DERIVATION_KEY_AGREEMENT     = 0x36,
    PAL_CRYPTO_KEY_DERIVATION_OUTPUT_BYTES      = 0x37,
    PAL_CRYPTO_KEY_DERIVATION_OUTPUT_KEY        = 0x38,
    PAL_CRYPTO_KEY_DERIVATION_SET_CAPACITY      = 0x39,
    PAL_CRYPTO_KEY_DERIVATION_GET_CAPACITY      = 0x3A,
    PAL_CRYPTO_HASH_CLONE                       = 0x3B,
    PAL_CRYPTO_MAC_COMPUTE                      = 0x3C,
    PAL_CRYPTO_MAC_VERIFY                       = 0x3D,
    PAL_CRYPTO_CIPHER_ENCRYPT                   = 0x3F,
    PAL_CRYPTO_CIPHER_DECRYPT                   = 0x40,
    PAL_CRYPTO_OPEN_KEY                         = 0x41,
    PAL_CRYPTO_CLOSE_KEY                        = 0x42,
    PAL_CRYPTO_AEAD_ENCRYPT_SETUP               = 0x43,
    PAL_CRYPTO_AEAD_DECRYPT_SETUP               = 0x44,
    PAL_CRYPTO_AEAD_GENERATE_NONCE              = 0x45,
    PAL_CRYPTO_AEAD_SET_NONCE                   = 0x46,
    PAL_CRYPTO_AEAD_SET_LENGTHS                 = 0X47,
    PAL_CRYPTO_AEAD_UPDATE_AD                   = 0x48,
    PAL_CRYPTO_AEAD_UPDATE                      = 0x49,
    PAL_CRYPTO_AEAD_FINISH                      = 0x4A,
    PAL_CRYPTO_AEAD_VERIFY                      = 0x4B,
    PAL_CRYPTO_AEAD_ABORT                       = 0x4C,
    PAL_CRYPTO_FREE                             = 0xFE,
};

int32_t pal_crypto_function(int type, va_list valist);
#endif /* _PAL_CRYPTO_INTF_H_ */
