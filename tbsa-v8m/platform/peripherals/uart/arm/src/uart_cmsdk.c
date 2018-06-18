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

#include "uart_cmsdk.h"

addr_t g_uart;

/**
    @brief    - This function initializes the UART
**/
void uart_init(addr_t uart_base_addr)
{
    g_uart = uart_base_addr;

    /* Disable RX and TX interrupts, disable overrun interrupts, enable TX,
    * disable RX */
    ((uart_t *) g_uart)->CTRL = CMSDK_UART_CTRL_TXEN_Msk;
    /* However, the UART resets to a BAUDDIV of 0, which is invalid. Set it to
    * the minimum valid number. */
    ((uart_t *) g_uart)->BAUDDIV = 16;
}

/**
    @brief    - This function checks for empty TX FIFO
**/
static int uart_is_tx_empty(void)
{
    /* Note: Check for empty TX FIFO */
    return (!((uart_t *) g_uart)->STATE & CMSDK_UART_STATE_TXBF_Msk);
}

/**
    @brief    - This function checks for empty TX FIFO and writes to FIFO register
**/
void uart_putc(uint8_t c)
{
    /* ensure TX buffer to be empty */
    while(!uart_is_tx_empty());

    /* write the data (upper 24 bits are reserved) */
    ((uart_t *) g_uart)->DATA = c;
}
