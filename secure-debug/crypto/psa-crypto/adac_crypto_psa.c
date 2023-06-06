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

#include "adac_crypto_psa.h"
#include <psa_adac_debug.h>

psa_status_t psa_adac_crypto_init(void)
{
    psa_status_t r = psa_crypto_init();
    if (r == PSA_SUCCESS)
        PSA_ADAC_LOG_INFO("psa-crypto", "PSA Crypto API Initialized\n");
    else
        PSA_ADAC_LOG_ERR("psa-crypto", "PSA Crypto API Initialization failure => %d\n", r);

    return r;
}

psa_status_t psa_adac_generate_challenge(uint8_t *output, size_t output_size)
{
    return psa_generate_random(output, output_size);
}

psa_status_t psa_adac_verify_vendor(uint8_t key_type, uint8_t *key, size_t key_size,
                                    psa_algorithm_t hash_algo, const uint8_t *inputs[],
                                    size_t input_sizes[], size_t input_count,
                                    psa_algorithm_t sig_algo, uint8_t *sig, size_t sig_size)
{
    psa_status_t ret = PSA_ERROR_NOT_SUPPORTED;
#if (defined(PSA_ADAC_CMAC) || defined(PSA_ADAC_HMAC))
    if ((key_type == CMAC_AES) || (key_type == HMAC_SHA256)) {
        ret = PSA_SUCCESS;
#if defined(PSA_ADAC_HMAC)
        if ((key_type == HMAC_SHA256) && ((sig_algo != HMAC_SIGN_ALGORITHM) ||
                                          (hash_algo != HMAC_HASH_ALGORITHM)))
            ret = PSA_ERROR_INVALID_ARGUMENT;
#endif
#if defined(PSA_ADAC_CMAC)
        if ((key_type == CMAC_AES) && ((sig_algo != CMAC_SIGN_ALGORITHM) ||
                                       (hash_algo != CMAC_HASH_ALGORITHM)))
            ret = PSA_ERROR_INVALID_ARGUMENT;
#endif
        if (PSA_SUCCESS == ret)
            ret = psa_adac_verify_mac(key_type, key, key_size,
                                      inputs, input_sizes, input_count, sig_algo, sig, sig_size);
    }
#endif

    // TODO: Add support for extra algorithms
    return ret;
}
