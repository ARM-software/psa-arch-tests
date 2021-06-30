/** @file
 * Copyright (c) 2021 Arm Limited or its affiliates. All rights reserved.
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

#include <val_adac.h>
#include <psa_adac_cryptosystems.h>
#include <psa_adac_sdm.h>
#include <psa_adac_debug.h>
#include <pal_interfaces.h>

void val_adac_host_init(void)
{
    psa_adac_platform_init();
    psa_crypto_init();
}

psa_status_t val_load_certificate_chain(const char *chain_file, uint8_t **chain, size_t *chain_size)
{
    int ret_val;
    psa_status_t r = PSA_SUCCESS;

    if (chain_file == NULL) {
        printf("Error:Path not found\n");
        r = PSA_ERROR_INVALID_ARGUMENT;
        return r;
    }
    ret_val = load_trust_chain(chain_file, chain, chain_size);
    if (ret_val != 0) {
        printf("Error loading trust chain (%s)\n", chain_file);
        r = PSA_ERROR_GENERIC_ERROR;
    }
    return r;
}

psa_status_t val_infer_cryptosystem(uint32_t *chain, size_t chain_size, psa_tlv_t **extns_list,
                              size_t *extns_count, uint8_t *key_system)
{
    int ret_val;
    psa_status_t r = PSA_SUCCESS;
    uint8_t key_type;
    size_t count, i;
    psa_tlv_t *current_extn;

    ret_val = split_tlv_static(chain, chain_size, extns_list, MAX_EXTENSIONS, extns_count);
    if (ret_val != 0) {
        PSA_ADAC_LOG_ERR("host", "Error parsing trust chain\n");
        r = PSA_ERROR_GENERIC_ERROR;
        return r;
    }
    count = *extns_count;
    if (count > MAX_EXTENSIONS) {
        printf("Error:Extension count exceeded maximum allowed\n");
        r = PSA_ERROR_NOT_PERMITTED;
        return r;
    }

    PSA_ADAC_LOG_INFO("host", "Found %zu certificates\n", count);
    for (i = 0; i < count; i++) {
        current_extn = extns_list[i];
        if ((current_extn)->type_id == 0x0201)
            key_type = ((certificate_header_t *) current_extn->value)->key_type;
    }
    *key_system = key_type;
    PSA_ADAC_LOG_INFO("host", "Cryptosystem detected: %d\n", key_type);
    return r;
}

psa_status_t val_get_private_key(const char *key_file, uint8_t *type, psa_key_handle_t *handle,
                                 uint8_t **key_ptr, size_t *size)
{
    int ret_val;
    psa_status_t r = PSA_SUCCESS;
    uint8_t key_type = *type;

    if (key_file == NULL) {
        printf("Error:Path not found\n");
        r = PSA_ERROR_INVALID_ARGUMENT;
        return r;
    }

    switch (key_type) {
    case ECDSA_P256_SHA256:
    case ECDSA_P521_SHA512:
    case RSA_3072_SHA256:
    case RSA_4096_SHA256:
    case ED_25519_SHA512:
    case ED_448_SHAKE256:
    case SM_SM2_SM3:
        ret_val = import_private_key(key_file, type, handle);
        if (ret_val != 0) {
            printf("Error importing private key (%s)\n", key_file);
            r = PSA_ERROR_GENERIC_ERROR;
        } else {
            key_ptr = NULL;
            size = 0;
        }
        break;

    case CMAC_AES:
    case HMAC_SHA256:
        ret_val = load_secret_key(key_file, key_type, key_ptr, size);
        if (ret_val != 0) {
            printf("Error importing secret key (%s)\n", key_file);
            r = PSA_ERROR_GENERIC_ERROR;
        } else {
            handle = NULL;
        }
        break;

    default:
        printf("Error: unsupported key type (0x%x)\n", key_type);
        r = PSA_ERROR_NOT_SUPPORTED;
    }
    return r;
}

request_packet_t *val_construct_command(uint16_t cmd_type, uint8_t *data, size_t data_size)
{
    request_packet_t *packet = NULL;

    switch (cmd_type) {
    case SDP_RESUME_BOOT_CMD:
    case SDP_LOCK_DEBUG_CMD:
    case SDP_DISCOVERY_CMD:
    case SDP_AUTH_START_CMD:
        packet = request_packet_build(cmd_type, NULL, 0);
        break;
    case SDP_AUTH_RESPONSE_CMD:
        if (data == NULL || data_size == 0) {
            printf("Error: No payload specified\n");
            break;
        }
        packet = request_packet_build((uint16_t)cmd_type, data, data_size);
        break;
    default:
        //TO DO: Callback for vendor specific command construction
        printf("Error: Unrecognized command. ID=(0x%x)\n", cmd_type);
    }
    return packet;
}

psa_status_t val_issue_command(uint32_t command, request_packet_t *packet,
                               uint8_t *data, size_t data_size)
{
    int ret_val;
    psa_status_t r = PSA_SUCCESS;

    packet = val_construct_command((uint16_t)command, data, data_size);

    if (packet == NULL) {
        printf("Command construction failed\n");
        r = PSA_ERROR_GENERIC_ERROR;
        return r;
    }

    switch (command) {
    case SDP_DISCOVERY_CMD:
        printf("Sending discovery request\n");
        break;
    case SDP_AUTH_START_CMD:
        printf("Sending challenge request\n");
        break;
    case SDP_AUTH_RESPONSE_CMD:
        printf("Sending authentication response\n");
        break;
    case SDP_RESUME_BOOT_CMD:
        printf("Sending close session command\n");
        break;
    case SDP_LOCK_DEBUG_CMD:
        printf("Sending lock debug request\n");
    default:
        //TO DO: Vendor specific message
        printf("Error: Unrecognized command. ID=(0x%x)\n", command);
        r = PSA_ERROR_NOT_SUPPORTED;
    }
    ret_val = request_packet_send(packet);
    if (ret_val < 0)
        r = PSA_ERROR_GENERIC_ERROR;

    request_packet_release(packet);
    return r;
}

response_packet_t *val_await_response(void)
{
    return response_packet_receive();
}

psa_status_t val_parse_response(uint32_t command, response_packet_t *packet)
{
    int ret_val;
    psa_status_t r = PSA_SUCCESS;
    size_t i;
    psa_tlv_t *tlv;
    psa_auth_challenge_t *challenge;

    if (packet == NULL) {
        printf("Error: Target response not obtained\n");
        r = PSA_ERROR_COMMUNICATION_FAILURE;
        return r;
    }

    switch (command) {
    case SDP_DISCOVERY_CMD:
        printf("Receiving discovery response...\n");
        for (i = 0; (i + 4) < (packet->data_count * 4);) {
            tlv = (psa_tlv_t *) (((uint8_t *)packet->data) + i);
            i += sizeof(psa_tlv_t) + tlv->length_in_bytes;
        }
        break;
    case SDP_AUTH_START_CMD:
        printf("Receiving challenge\n");
        printf("status = 0x%04x, data_count = %d\n", packet->status, packet->data_count);
        if (packet->data_count * 4 != sizeof(psa_auth_challenge_t)) {
            r = PSA_ERROR_GENERIC_ERROR;
            return r;
        }
        challenge = (psa_auth_challenge_t *) packet->data;
        PSA_ADAC_LOG_DUMP("host", "challenge", challenge->challenge_vector,
                           sizeof(challenge->challenge_vector));
        break;
    case SDP_AUTH_RESPONSE_CMD:
    case SDP_RESUME_BOOT_CMD:
    case SDP_LOCK_DEBUG_CMD:
        printf("status = 0x%04x, data_count = %d\n", packet->status, packet->data_count);
        break;
    default:
        r = PSA_ERROR_NOT_SUPPORTED;
    }
    return r;
}

psa_status_t val_sign_token(uint8_t challenge[], size_t challenge_size, uint8_t signature_type,
                            uint8_t exts[], size_t exts_size, uint8_t *fragment[],
                            size_t *fragment_size, psa_key_handle_t handle,
                            uint8_t *key, size_t key_size)
{
    psa_status_t r;

    r = psa_adac_sign_token(challenge, challenge_size, signature_type, exts, exts_size,
                           fragment, fragment_size, handle, key, key_size);
    if (r == PSA_SUCCESS) {
        PSA_ADAC_LOG_DUMP("host", "token", *fragment, *fragment_size);
    } else {
        PSA_ADAC_LOG_ERR("host", "Error signing token\n");
        r = PSA_ERROR_GENERIC_ERROR;
    }
    return r;
}

psa_status_t val_send_certificate(psa_tlv_t **extns_list, size_t extns_count)
{
    request_packet_t *request;
    response_packet_t *response;
    psa_status_t r;
    uint8_t *payload;
    size_t i, payload_size;
    psa_tlv_t *current_extn;

    for (size_t i = 0; i < extns_count; i++) {
        current_extn = extns_list[i];
        if (current_extn->type_id == 0x0201) {
            payload = (uint8_t *)current_extn;
            payload_size = current_extn->length_in_bytes + sizeof(psa_tlv_t);

            printf("Sending Certificate\n");
            r = val_issue_command(SDP_AUTH_RESPONSE_CMD, request, payload, payload_size);
            if (r != PSA_SUCCESS)
                return r;

            printf("Receiving token_authentication response\n");
            response = val_await_response();
            r = val_parse_response(SDP_AUTH_RESPONSE_CMD, response);
            if (r != PSA_SUCCESS)
                return r;

            if (response->status == SDP_NEED_MORE_DATA)
                response_packet_release(response);
        }
    }
    if (response->status != SDP_NEED_MORE_DATA) {
        PSA_ADAC_LOG_ERR("host", "Unexpected response status %x\n", response->status);
        r = PSA_ERROR_GENERIC_ERROR;
        return r;
    }
    response_packet_release(response);
    return r;
}

int val_check_cryptosystem_support(response_packet_t *packet, uint8_t key_system)
{
    int found = 0, j;
    size_t i = 0;
    psa_tlv_t *tlv;
    uint8_t *key_support_types = NULL;

    while ((i + 4) < (packet->data_count * 4)) {
        tlv = (psa_tlv_t *) (((uint8_t *)packet->data) + i);
        if (tlv->type_id == 0x0102) {
           key_support_types = tlv->value;
           for (j = 0; j < (tlv->length_in_bytes); j++) {
                if (*(key_support_types+j) == key_system) {
                    found = 1;
                    break;
                }
           }
        }
        i += sizeof(psa_tlv_t) + tlv->length_in_bytes;
    }

    if (key_support_types == NULL)
        printf("Cryptosystem Type ID not found in target's response\n");
    else if (!found)
        printf("Cryptosystem not supported by target\n");
    else
        printf("Cryptosystem supported by target\n");

    return found;
}
