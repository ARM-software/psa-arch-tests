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
#ifndef _PAL_INTERFACES_H_
#define _PAL_INTERFACES_H_

#include <stdarg.h>
#include <psa_adac.h>

/**
 *   @brief    - This function parses the input string and writes bytes into logger TX FIFO
 *   @param    - str      : Input String
 *             - data     : Value for format specifier
 *   @return   - SUCCESS/FAILURE
**/

int pal_print(const char *str, int32_t data);

/**
 *   @brief    - Terminates the simulation at the end of all tests completion.
 *               By default, it put cpus into power down mode.
 *   @param    - void
 *   @return   - void
**/
void pal_terminate_simulation(void);

/**
 *   @brief    - Resets the system.
 *   @param    - void
 *   @return   - SUCCESS/FAILURE
**/
int pal_system_reset(void);

request_packet_t *request_packet_lock(size_t *max_data_size);

/**
 *   @brief    - Reserve the communication buffer memory for receive packet.
 *   @param    - max_data_size      Valid size of command frame
 *   @return   - Pointer to the command frame to be read
**/
response_packet_t *response_packet_lock(size_t *max_data_size);

/**
 *   @brief    - Release the lock held by transmit packet.
 *   @param    - packet      Most recent command frame sent
 *   @return   - SUCCESS/FAILURE
**/
int request_packet_release(request_packet_t *packet);

/**
 *   @brief    - Release the lock held by receive packet.
 *   @param    - packet      Most recent response packet received
 *   @return   - SUCCESS/FAILURE
**/
int response_packet_release(response_packet_t *packet);

/**
 *   @brief    - Construct the Request packet for the specified ADAC command.
 *   @param    - command      ADAC command
 *               data         Pointer to payload
 *               data_size    Size of the command payload
 *   @return   - Pointer to the command frame to be written
**/
request_packet_t *request_packet_build(uint16_t command, uint8_t *data, size_t data_size);

/**
 *   @brief    - Write the Request packet into the communication buffer for transmit.
 *   @param    - packet       Request packet built for dispatch
 *   @return   - SUCCESS/FAILURE
**/
int request_packet_send(request_packet_t *packet);

/**
 *   @brief    - Read the Response packet from the communication buffer.
 *   @param    - None
 *   @return   - Response packet received from target.
**/
response_packet_t *response_packet_receive();

#endif
