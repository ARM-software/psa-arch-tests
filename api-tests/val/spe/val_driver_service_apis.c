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


#include "val_driver_service_apis.h"

print_verbosity_t  g_print_level = PRINT_INFO;

/* UART APIs */
/*
    @brief    - Initialise the UART
    @param    - uart_base_addr: address of the uart base
    @param    - print_level: g_print_level verbosity
    @return   - val_status_t
*/
val_status_t val_uart_init_sf(addr_t uart_base_addr)
{
    pal_uart_init(uart_base_addr);
    return VAL_STATUS_SUCCESS;
}
/*
    @brief     - This function parses the input string and writes byte by byte to
                 print the input string
    @param     - pointer         : Input String
               - data           : Value for Format specifier
    @return    - error status
 */
val_status_t val_print_sf(char *string, uint32_t data)
{
    pal_print(string, data);
    return VAL_STATUS_SUCCESS;
}

/* Watchdog APIs */
/**
    @brief    - Initializes the WatchDog Timer instance
    @param    - base address of the given timer instance
              - time_us         : Time in micro seconds
              - timer_tick_us   : Number of ticks per micro second
    @return   - error status
**/
val_status_t val_wd_timer_init_sf(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us)
{
    return pal_wd_timer_init(base_addr, time_us, timer_tick_us);
}

/**
    @brief    - Enable WatchDog Timer instance
    @param    - base address of the given timer instance
    @return   - error status
**/
val_status_t val_wd_timer_enable_sf(addr_t base_addr)
{
    return pal_wd_timer_enable(base_addr);
}

/**
    @brief    - Disable Watch Dog Timer instance
    @param    - base address of the given timer instance
    @return   - error status
**/
val_status_t val_wd_timer_disable_sf(addr_t base_addr)
{
    return pal_wd_timer_disable(base_addr);
}

/**
    @brief    - Checks if watchdog enabled
    @param    - base address of the given timer instance
    @return   - error status
**/
val_status_t val_is_wd_timer_enabled_sf(addr_t base_addr)
{
    if (pal_wd_timer_is_enabled(base_addr))
    {
        return VAL_STATUS_SUCCESS;
    }
    else
    {
        return VAL_STATUS_ERROR;
    }
}

/*
    @brief     - Reads 'size' bytes from Non-volatile memory at a given 'base + offset'
                 into given buffer.
               - offset    : Offset from NV MEM base address
               - buffer    : Pointer to source address
               - size      : Number of bytes
    @return    - val_status_t
*/
val_status_t val_nvmem_read_sf(addr_t base, uint32_t offset, void *buffer, int size)
{
    if (pal_nvmem_read(base, offset, buffer, size))
    {
        return VAL_STATUS_SUCCESS;
    }
    else
    {
        return VAL_STATUS_ERROR;
    }
}

/*
    @brief     - Writes 'size' bytes from buffer into non-volatile memory at a given 'base + offset'
               - offset    : Offset
               - buffer    : Pointer to source address
               - size      : Number of bytes
    @return    - val_status_t
*/
val_status_t val_nvmem_write_sf(addr_t base, uint32_t offset, void *buffer, int size)
{
    if (pal_nvmem_write(base, offset, buffer, size))
    {
        return VAL_STATUS_SUCCESS;
    }
    else
    {
        return VAL_STATUS_ERROR;
    }
}
