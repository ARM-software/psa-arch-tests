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

#ifndef ADAC_CRYPTO_PSA_H
#define ADAC_CRYPTO_PSA_H

#include <psa_adac_config.h>
#include <psa_adac.h>
#include <psa/crypto.h>
#include <psa_adac_crypto_api.h>
#include <psa_adac_cryptosystems.h>

psa_status_t psa_adac_verify_vendor(uint8_t key_type, uint8_t *key, size_t key_size,
                                    psa_algorithm_t hash_algo, const uint8_t *inputs[],
                                   size_t input_sizes[], size_t input_count,
                                    psa_algorithm_t sig_algo, uint8_t *sig, size_t sig_size);

psa_status_t psa_adac_verify_mac(uint8_t key_type, uint8_t *key, size_t key_size,
                                 const uint8_t *inputs[], size_t input_sizes[], size_t input_count,
                                 psa_algorithm_t mac_algo, uint8_t *mac, size_t mac_size);

#endif //ADAC_CRYPTO_PSA_H
