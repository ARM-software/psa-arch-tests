/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
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


#include "pal_common.h"
#include "pal_crypto_intf.h"

/**
    @brief    - This API will call the requested crypto function
    @param    - type    : function code
                valist  : variable argument list
    @return   - error status
**/
#ifdef CRYPTO_SUITE
psa_status_t pal_crypto_function(int type, va_list valist)
{
    size_t                  size, *length;
    uint8_t                 *buffer;
    uint32_t                status;
    psa_key_slot_t          key_slot;
    psa_key_type_t          key_type, *key_type_out;
    psa_key_policy_t        *policy;
    psa_key_usage_t         usage, *usage_out;
    psa_key_lifetime_t      lifetime, *lifetime_out;
    psa_algorithm_t         alg, *alg_out;
    psa_hash_operation_t    *operation;

    switch (type)
    {
        case PAL_CRYPTO_INIT:
            return psa_crypto_init();
        case PAL_CRYPTO_GENERATE_RANDOM:
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, int);
            return psa_generate_random(buffer, size);
        case PAL_CRYPTO_IMPORT_KEY:
            key_slot = va_arg(valist, psa_key_slot_t);
            key_type = va_arg(valist, psa_key_type_t);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, int);
            status = psa_import_key(key_slot, key_type, buffer, size);
            return status;
        case PAL_CRYPTO_EXPORT_KEY:
            key_slot = va_arg(valist, psa_key_slot_t);
            buffer = (uint8_t *)(va_arg(valist, uint8_t*));
            size = va_arg(valist, int);
            length = (size_t *)va_arg(valist, size_t*);
            status = psa_export_key(key_slot, buffer, size, length);
            return status;
        case PAL_CRYPTO_EXPORT_PUBLIC_KEY:
            key_slot = va_arg(valist, psa_key_slot_t);
            buffer = (uint8_t *)(va_arg(valist, uint8_t*));
            size = va_arg(valist, int);
            length = (size_t *)va_arg(valist, size_t*);
            status = psa_export_public_key(key_slot, buffer, size, length);
            return status;
        case PAL_CRYPTO_KEY_POLICY_INIT:
            policy = va_arg(valist, psa_key_policy_t*);
            psa_key_policy_init(policy);
            return 0;
        case PAL_CRYPTO_KEY_POLICY_SET_USAGE:
            policy = va_arg(valist, psa_key_policy_t*);
            usage = va_arg(valist, psa_key_usage_t);
            alg = va_arg(valist, psa_algorithm_t);
            psa_key_policy_set_usage(policy, usage, alg);
            return 0;
        case PAL_CRYPTO_SET_KEY_POLICY:
            key_slot = va_arg(valist, psa_key_slot_t);
            policy = va_arg(valist, psa_key_policy_t*);
            return psa_set_key_policy(key_slot, policy);
        case PAL_CRYPTO_DESTROY_KEY:
            key_slot = va_arg(valist, psa_key_slot_t);
            status = psa_destroy_key(key_slot);
            return status;
        case PAL_CRYPTO_GET_KEY_INFORMATION:
            key_slot = va_arg(valist, psa_key_slot_t);
            key_type_out = va_arg(valist, psa_key_type_t*);
            length = (size_t *)va_arg(valist, size_t*);
            status = psa_get_key_information(key_slot, key_type_out, length);
            return status;
        case PAL_CRYPTO_GET_KEY_POLICY:
            key_slot = va_arg(valist, psa_key_slot_t);
            policy = va_arg(valist, psa_key_policy_t*);
            return psa_get_key_policy(key_slot, policy);
        case PAL_CRYPTO_KEY_POLICY_GET_USAGE:
            policy = va_arg(valist, psa_key_policy_t*);
            usage_out = va_arg(valist, psa_key_usage_t*);
            *usage_out = psa_key_policy_get_usage(policy);
            return 0;
        case PAL_CRYPTO_KEY_POLICY_GET_ALGORITHM:
            policy = va_arg(valist, psa_key_policy_t*);
            alg_out = va_arg(valist, psa_algorithm_t*);
            *alg_out = psa_key_policy_get_algorithm(policy);
            return 0;
        case PAL_CRYPTO_GET_KEY_LIFETIME:
            key_slot = va_arg(valist, psa_key_slot_t);
            lifetime_out = va_arg(valist, psa_key_lifetime_t*);
            return psa_get_key_lifetime(key_slot, lifetime_out);
        case PAL_CRYPTO_SET_KEY_LIFETIME:
            key_slot = va_arg(valist, psa_key_slot_t);
            lifetime = va_arg(valist, psa_key_lifetime_t);
            return psa_set_key_lifetime(key_slot, lifetime);
        case PAL_CRYPTO_HASH_SETUP:
            operation = va_arg(valist, psa_hash_operation_t*);
            alg = va_arg(valist, psa_algorithm_t);
            return psa_hash_setup(operation, alg);
        case PAL_CRYPTO_HASH_UPDATE:
            operation = va_arg(valist, psa_hash_operation_t*);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            return psa_hash_update(operation, buffer, size);
        case PAL_CRYPTO_HASH_VERIFY:
            operation = va_arg(valist, psa_hash_operation_t*);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            return psa_hash_verify(operation, buffer, size);
        case PAL_CRYPTO_HASH_FINISH:
            operation = va_arg(valist, psa_hash_operation_t*);
            buffer = va_arg(valist, uint8_t*);
            size = va_arg(valist, size_t);
            length = va_arg(valist, size_t*);
            return psa_hash_finish(operation, buffer, size, length);
        case PAL_CRYPTO_HASH_ABORT:
            operation = va_arg(valist, psa_hash_operation_t*);
            return psa_hash_abort(operation);
        default:
            return PAL_STATUS_UNSUPPORTED_FUNC;
    }
}

#else
psa_status_t pal_crypto_function(int type, va_list valist)
{
    return PAL_STATUS_UNSUPPORTED_FUNC;
}
#endif
