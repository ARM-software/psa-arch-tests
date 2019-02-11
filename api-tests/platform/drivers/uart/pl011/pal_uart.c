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

#include "pal_uart.h"

volatile uint32_t uart;

/**
    @brief    - This function initializes the UART
    @param    - uart_base_addr: Base address of UART
**/
void pal_uart_pl011_init(uint32_t uart_base_addr)
{
    uart = uart_base_addr;
}

/**
    @brief    - This function checks for empty TX FIFO
**/
static int pal_uart_is_tx_empty(void)
{
    if ((((uart_gt *)uart)->uartcr & UART_PL011_UATRCR_EN_MASK) &&
        /* UART is enabled */
        (((uart_gt *)uart)->uartcr & UART_PL011_UARTCR_TX_EN_MASK) &&
        /* Transmit is enabled */
        ((((uart_gt *)uart)->uartfr & UART_PL011_UARTFR_TX_FIFO_FULL) == 0))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
    @brief    - This function checks for empty TX FIFO and writes to FIFO register
**/
static void pal_uart_putc(uint8_t c)
{
    const uint8_t pdata = (uint8_t)c;

    /* ensure TX buffer to be empty */
    while(!pal_uart_is_tx_empty());

    /* write the data (upper 24 bits are reserved) */
    ((uart_gt *)uart)->uartdr = pdata;
}

/**
    @brief    - This function parses the input string and writes bytes into UART TX FIFO
    @param    - str      : Input String
              - data     : Value for format specifier
**/
void pal_uart_pl011_print(char *str, uint32_t data)
{
    uint8_t j, buffer[16];
    int8_t  i = 0;

    for (; *str != '\0'; ++str)
    {
        if (*str == '%')
        {
            ++str;
            if (*str == 'd')
            {
                while (data != 0)
                {
                    j         = data % 10;
                    data      = data /10;
                    buffer[i] = j + 48;
                    i        += 1;
                }
            }
            else if (*str == 'x' || *str == 'X')
            {
                while (data != 0)
                {
                    j         = data & 0xf;
                    data    >>= 4;
                    buffer[i] = j + ((j > 9) ? 55 : 48);
                    i        += 1;
                }
            }
            if (i > 0)
            {
                while (i > 0)
                {
                    pal_uart_putc(buffer[--i]);
                }
            }
            else
            {
                pal_uart_putc(48);
            }
        }
        else
        {
            pal_uart_putc(*str);

            if (*str == '\n')
            {
                pal_uart_putc('\r');
            }
        }
    }
}
