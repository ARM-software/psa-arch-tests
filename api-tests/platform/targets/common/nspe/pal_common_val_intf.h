/** @file
 * Copyright (c) 2025, Arm Limited or its affiliates. All rights reserved.
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

#ifndef _PAL_COMMON_VAL_INTF_H_
#define _PAL_COMMON_VAL_INTF_H_

#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>

#define LOG_BUFFER_SIZE 8192

extern uint8_t test_status_buffer[256];
#define PLATFORM_SHARED_REGION_BASE (void*)test_status_buffer

/**
 *   @brief    - This function sends a character to the UART TX FIFO.
 *   @param    - c  :  Input character
 *   @return   - SUCCESS/FAILURE
**/
int pal_print(uint8_t c);

/**
 *   @brief           - Enables a hardware watchdog timer
 *   @param           - Void
 *   @return          - SUCCESS/FAILURE
**/
int pal_watchdog_enable(void);

/**
 *   @brief           - Disables a hardware watchdog timer
 *   @param           - Void
 *   @return          - SUCCESS/FAILURE
**/
int pal_watchdog_disable(void);

/**
 *   @brief    - Reads from given non-volatile address.
 *   @param    - offset  : Offset
 *               buffer  : Pointer to source address
 *               size    : Number of bytes
 *   @return   - SUCCESS/FAILURE
**/
int pal_nvm_read(uint32_t offset, void *buffer, size_t size);

/**
 *   @brief    - Writes into given non-volatile address.
 *   @param    - offset  : Offset
 *               buffer  : Pointer to source address
 *               size    : Number of bytes
 *   @return   - SUCCESS/FAILURE
**/
int pal_nvm_write(uint32_t offset, void *buffer, size_t size);


#define pal_uart_putc(x) pal_print(x)

#endif /* _PAL_COMMON_VAL_INTF_H_ */
