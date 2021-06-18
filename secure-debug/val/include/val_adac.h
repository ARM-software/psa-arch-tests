/** @file
 * Copyright (c) 2021, Arm Limited or its affiliates. All rights reserved.
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

#ifndef _VAL_ADAC_H_
#define _VAL_ADAC_H_
#include <psa_adac.h>

/** \brief Token header
 *
 */
typedef struct {
    uint16_t type;
    uint8_t *data;
    size_t size;
} adac_command_frame_t;

void val_adac_host_init(void);
psa_status_t val_load_certificate_chain(const char *chain_file, uint8_t **chain,
                                        size_t *chain_size);
psa_status_t val_infer_cryptosystem(uint32_t *chain, size_t chain_size, psa_tlv_t **extns_list,
                                size_t *extns_count, uint8_t *key_system);
psa_status_t val_get_private_key(const char *key_file, uint8_t *type, psa_key_handle_t *handle,
                                 uint8_t **key_ptr, size_t *size);
request_packet_t *val_construct_command(uint16_t command, uint8_t *data, size_t data_size);
psa_status_t val_issue_command(uint32_t command, request_packet_t *packet,
                               uint8_t *data, size_t data_size);
response_packet_t *val_await_response(void);
psa_status_t val_parse_response(uint32_t command, response_packet_t *packet);

psa_status_t val_sign_token(uint8_t challenge[], size_t challenge_size, uint8_t signature_type,
                            uint8_t exts[], size_t exts_size, uint8_t *fragment[],
                            size_t *fragment_size, psa_key_handle_t handle,
                            uint8_t *key, size_t key_size);
psa_status_t val_send_certificate(psa_tlv_t **extns_list, size_t extns_count);
int val_check_cryptosystem_support(response_packet_t *packet, uint8_t key_system);

#endif /* _VAL_ADAC_H_ */
