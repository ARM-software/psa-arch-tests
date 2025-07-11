/** @file
 * Copyright (c) 2019-2025, Arm Limited or its affiliates. All rights reserved.
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
#include "pal_uart.h"
#include "pal_nvmem.h"
#include "pal_wd_stm.h"

/* Regression test status reporting buffer */
uint8_t test_status_buffer[256]  = {0};

/**
    @brief    - This function initializes the UART
    @param    - Void
    @return   - SUCCESS/FAILURE
**/
int pal_uart_init_ns(void)
{
    pal_uart_stm_init((uint32_t) PLATFORM_UART_BASE);
    return PAL_STATUS_SUCCESS;
}

/**
    @brief    - This function sends a character to the UART TX FIFO.
    @param    - c  :  Input character
    @return   - SUCCESS/FAILURE
**/
int pal_print(uint8_t c)
{
    pal_uart_stm_putc(c);
    return PAL_STATUS_SUCCESS;
}

/**
    @brief           - Initializes a hardware watchdog timer
    @param           - time_us         : Time in micro seconds
                     - timer_tick_us   : Number of ticks per micro second
    @return          - SUCCESS/FAILURE
**/
int pal_wd_timer_init_ns(uint32_t time_us, uint32_t timer_tick_us)
{
    return pal_wd_stm_init((addr_t) PLATFORM_WD_BASE, time_us, timer_tick_us);
}

/**
    @brief           - Enables a hardware watchdog timer
    @param           - Void
    @return          - SUCCESS/FAILURE
**/
int pal_watchdog_enable(void)
{
    return pal_wd_stm_enable((addr_t) PLATFORM_WD_BASE);
}

/**
    @brief           - Disables a hardware watchdog timer
    @param           - Void
    @return          - SUCCESS/FAILURE
**/
int pal_watchdog_disable(void)
{
    return pal_wd_stm_disable((addr_t) PLATFORM_WD_BASE);
}

/**
    @brief    - Reads from given non-volatile address.
    @param    - offset  : Offset
                buffer  : Pointer to source address
                size    : Number of bytes
    @return   - SUCCESS/FAILURE
**/
int pal_nvm_read(uint32_t offset, void *buffer, size_t size)
{
    if (nvmem_read((addr_t) PLATFORM_NVM_BASE, offset, buffer, size))
    {
        return PAL_STATUS_SUCCESS;
    }
    else
    {
        return PAL_STATUS_ERROR;
    }
}

/**
    @brief    - Writes into given non-volatile address.
    @param    - offset  : Offset
                buffer  : Pointer to source address
                size    : Number of bytes
    @return   - SUCCESS/FAILURE
**/
int pal_nvm_write(uint32_t offset, void *buffer, size_t size)
{
    if (nvmem_write((addr_t) PLATFORM_NVM_BASE, offset, buffer, size))
    {
        return PAL_STATUS_SUCCESS;
    }
    else
    {
        return PAL_STATUS_ERROR;
    }
}

/**
 *   @brief    - Terminates the simulation at the end of all tests completion.
 *               By default, it put cpus into power down mode.
 *   @param    - void
 *   @return   - void
**/
void pal_terminate_simulation(void)
{
    /* Add logic to terminate the simluation */

    while (1)
    {
        __asm volatile("WFI");
    }
}

/**
 *   @brief    - Resets the system.
 *   @param    - void
 *   @return   - SUCCESS/FAILURE
**/
int pal_system_reset(void)
{
    /* Reset functionality is not functional on AN521 FVP */
    return PAL_STATUS_UNSUPPORTED_FUNC;
}
