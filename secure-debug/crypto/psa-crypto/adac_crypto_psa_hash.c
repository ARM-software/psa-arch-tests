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

#include <psa_adac.h>
#include <psa_adac_crypto_api.h>

psa_status_t psa_adac_hash(psa_algorithm_t alg, const uint8_t *input, size_t input_size,
                           uint8_t *hash, size_t hash_size, size_t *hash_length)
{
    return psa_adac_hash_multiple(alg, &input, &input_size, 1, hash, hash_size, hash_length);
}

psa_status_t psa_adac_hash_multiple(psa_algorithm_t alg, const uint8_t *inputs[],
                                    size_t input_sizes[], size_t input_count,
                                    uint8_t hash[], size_t hash_size, size_t *hash_length)
{
    psa_status_t status;

    if (PSA_ALG_IS_VENDOR_DEFINED(alg) != 0) {
        // TODO: Add support for extra algorithms
        status = PSA_ERROR_NOT_SUPPORTED;
    } else {
        psa_hash_operation_t hashOperation = PSA_HASH_OPERATION_INIT;

        status = psa_hash_setup(&hashOperation, alg);
        for (size_t i = 0; (i < input_count) && (PSA_SUCCESS == status); i++)
            status = psa_hash_update(&hashOperation, inputs[i], input_sizes[i]);

        if (PSA_SUCCESS == status)
            status = psa_hash_finish(&hashOperation, hash, hash_size, hash_length);
    }

    return status;
}

psa_status_t psa_adac_hash_verify(psa_algorithm_t alg, const uint8_t input[], size_t input_size,
                                  uint8_t hash[], size_t hash_size)
{
    psa_status_t status;
    if (PSA_ALG_IS_VENDOR_DEFINED(alg) != 0) {
        // TODO: Add support for extra algorithms
        status = PSA_ERROR_NOT_SUPPORTED;
    } else {
        psa_hash_operation_t hashOperation = PSA_HASH_OPERATION_INIT;

        status = psa_hash_setup(&hashOperation, alg);
        if (PSA_SUCCESS == status)
            status = psa_hash_update(&hashOperation, input, input_size);

        if (PSA_SUCCESS == status)
            status = psa_hash_verify(&hashOperation, hash, hash_size);
    }

    return status;
}

static psa_status_t hash_check(const uint8_t *a, size_t la, const uint8_t *b, size_t lb)
{
    uint8_t chk = 1;

    if (la == lb) {
        chk = 0;
        for (size_t i = 0; i < la; i++) {
            chk |= (uint8_t) (a[i] ^ b[i]);
        }
    }

    return (chk == 0U) ? PSA_SUCCESS : PSA_ERROR_INVALID_SIGNATURE;
}

psa_status_t psa_adac_hash_verify_multiple(psa_algorithm_t alg,
                                           const uint8_t input[], size_t input_length,
                                           uint8_t *hash[], size_t hash_size[], size_t hash_count)
{
    psa_status_t status;

    if (PSA_ALG_IS_VENDOR_DEFINED(alg) != 0) {
        // TODO: Add support for extra algorithms
        status = PSA_ERROR_NOT_SUPPORTED;
    } else {
        psa_hash_operation_t hashOperation = PSA_HASH_OPERATION_INIT;
        // cppcheck-suppress misra-c2012-18.8
        uint8_t tmp[PSA_HASH_MAX_SIZE];
        size_t tmp_len;

        status = psa_hash_setup(&hashOperation, alg);
        if (PSA_SUCCESS == status)
            status = psa_hash_update(&hashOperation, input, input_length);

        if (PSA_SUCCESS == status)
            status = psa_hash_finish(&hashOperation, tmp, sizeof(tmp), &tmp_len);

        if (PSA_SUCCESS == status) {
            // PSA_ADAC_LOG_TDUMP("crypto", "target", tmp, tmp_len);
            for (size_t i = 0; i < hash_count; i++) {
                // PSA_ADAC_LOG_TDUMP("crypto", "candidate", hash[i], hash_size[i]);
                status = hash_check(hash[i], hash_size[i], tmp, tmp_len);
                if (status == PSA_SUCCESS)
                    break;
            }
        }
    }

    return status;
}
