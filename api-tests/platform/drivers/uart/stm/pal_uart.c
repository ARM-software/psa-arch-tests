/** @file
 * Copyright (c) 2018-2025, Arm Limited or its affiliates. All rights reserved.
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

volatile uint32_t g_uart;

/**
    @brief    - This function initializes the UART
**/
void pal_uart_stm_init(uint32_t uart_base_addr)
{
    g_uart = uart_base_addr;

}

/**
    @brief    - This function checks for empty TX FIFO
**/
static int pal_uart_stm_is_tx_empty(void)
{
    /* Note: Check for empty TX FIFO */
/*__STATIC_INLINE uint32_t LL_USART_IsActiveFlag_TXE_TXFNF(USART_TypeDef *USARTx)*/
/*{*/
/*  return ((READ_BIT(USARTx->ISR, USART_ISR_TXE_TXFNF) == (USART_ISR_TXE_TXFNF)) ? 1UL : 0UL);*/
/*}*/

    return ((((USART_TypeDef *) g_uart)->ISR & USART_ISR_TXE_TXFNF));
}

/**
    @brief    - This function checks for empty TX FIFO and writes to FIFO register
**/
void pal_uart_stm_putc(uint8_t c)
{
    /* ensure TX buffer to be empty */
    while (!pal_uart_stm_is_tx_empty())
		;

    /* write the data (upper 24 bits are reserved) */
    ((USART_TypeDef *) g_uart)->TDR = c;
}

/**
    @brief    - This function checks for TX interrupt triggered or not
**/
static int pal_uart_stm_is_tx_irq_triggerd(void)
{
    return (((USART_TypeDef *) g_uart)->ISR & USART_ISR_TC);
}

/**
    @brief    - This function triggers the UART TX interrupt
**/
void pal_uart_stm_generate_irq(void)
{
    /* Enable TX interrupt */
    ((USART_TypeDef *) g_uart)->CR1 |= USART_CR1_TCIE;

    /* Fill the TX buffer to generate TX IRQ */
    pal_uart_stm_putc(' ');
    pal_uart_stm_putc(' ');
    /* Loop until TX interrupt trigger */
    while (!pal_uart_stm_is_tx_irq_triggerd())
		;
}

/**
    @brief    - This function disable the UART TX interrupt
**/
void pal_uart_stm_disable_irq(void)
{
    /* Clear TX interrupt */
    ((USART_TypeDef *) g_uart)->ICR = USART_ICR_TCCF;

    /* Disable TX interrupt */
    ((USART_TypeDef *) g_uart)->CR1 &= ~USART_CR1_TCIE;
}
