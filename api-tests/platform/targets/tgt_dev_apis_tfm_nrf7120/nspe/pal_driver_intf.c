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

#include "pal_common.h"
#include "pal_nvmem.h"

#include "nrf_wdt.h"

/* Regression test status reporting buffer */
uint8_t test_status_buffer[256]  = {0};

/**
    @brief    - This function initializes the UART
    @param    - Void
    @return   - SUCCESS/FAILURE
**/
int pal_uart_init_ns(void)
{
    return PAL_STATUS_SUCCESS;
}

/**
    @brief    - This function parses the input string and writes bytes into UART TX FIFO
    @param    - str      : Input String
              - data     : Value for format specifier
    @return   - SUCCESS/FAILURE
**/
int pal_print_ns(const char *str, int32_t data)
{
    tfm_log_printf(str, data);
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
    (void)timer_tick_us;
    return nrf_wdt_init((addr_t) PLATFORM_WD_BASE, time_us);
}

/**
    @brief           - Enables a hardware watchdog timer
    @param           - Void
    @return          - SUCCESS/FAILURE
**/
int pal_watchdog_enable(void)
{
    return nrf_wdt_enable((addr_t) PLATFORM_WD_BASE);
}

/**
    @brief           - Disables a hardware watchdog timer
    @param           - Void
    @return          - SUCCESS/FAILURE
**/
int pal_watchdog_disable(void)
{
    return nrf_wdt_disable((addr_t) PLATFORM_WD_BASE);
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
    addr_t base = PLATFORM_NVM_BASE;
    if (base != 0) {
        /* Unexpected base address */
        return PAL_STATUS_ERROR;
    }
    base = pal_nvmem_get_addr();
    return nvmem_read(base, offset, buffer, size) ? PAL_STATUS_SUCCESS
                                                  : PAL_STATUS_ERROR;
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
    addr_t base = PLATFORM_NVM_BASE;
    if (base != 0) {
        /* Unexpected base address */
        return PAL_STATUS_ERROR;
    }
    base = pal_nvmem_get_addr();
    return nvmem_write(base, offset, buffer, size) ? PAL_STATUS_SUCCESS
                                                   : PAL_STATUS_ERROR;
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
    return tfm_platform_system_reset();
}
