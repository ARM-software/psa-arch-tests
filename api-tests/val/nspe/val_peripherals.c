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

#include "val_peripherals.h"
#include "pal_interfaces_ns.h"
#include "val_framework.h"
#include "val_client_defs.h"

/* Global */
uint32_t   is_uart_init_done = 0;

/*
    @brief    - Initialize UART.
                This is client interface API of secure partition UART INIT API.
    @param    - None
    @return   - val_status_t
*/
val_status_t val_uart_init(void)
{
    is_uart_init_done = 1;
    return pal_uart_init_ns();
}

/**
    @brief    - Print module. This is client interface API of secure partition
                val_print_sf API for nspe world
    @param    - verbosity: Print verbosity level
              - string   : Input string
              - data     : Value for format specifier
    @return   - val_status_t
**/
val_status_t val_print(print_verbosity_t verbosity, const char *string, int32_t data)
{
    if ((is_uart_init_done == 0) || (verbosity < VERBOSITY))
    {
       return VAL_STATUS_SUCCESS;
    }
#ifdef BESPOKE_PRINT_NS
    return pal_print_ns(string, data);
#else
    return val_printf(verbosity, string, data);
#endif
}

/* Watchdog APIs */
/**
    @brief    - Initializes the WatchDog Timer instance. This is client interface API of
                secure partition val_wd_timer_init_sf API for nspe world.
    @param    - timeout_type : watchdog timeout value to be programmed
                Defines to be used are WD_LOW_TIMEOUT, WD_MEDIUM_TIMEOUT and WD_HIGH_TIMEOUT
    @return   - error status
**/
val_status_t val_wd_timer_init(wd_timeout_type_t timeout_type)
{
   uint32_t time_us;

   if (timeout_type == WD_CRYPTO_TIMEOUT)
   {
       time_us =  PLATFORM_WD_TIMEOUT_IN_MICRO_SEC_CRYPTO;
   }
   else if (timeout_type == WD_LOW_TIMEOUT)
   {
       time_us =  PLATFORM_WD_TIMEOUT_IN_MICRO_SEC_LOW;
   }
   else if (timeout_type == WD_MEDIUM_TIMEOUT)
   {
       time_us = PLATFORM_WD_TIMEOUT_IN_MICRO_SEC_MEDIUM;
   }
   else
   {
       time_us = PLATFORM_WD_TIMEOUT_IN_MICRO_SEC_HIGH;
   }

   return pal_wd_timer_init_ns(time_us, PLATFORM_WD_NUM_OF_TICK_PER_MICRO_SEC);
}

val_status_t val_wd_reprogram_timer(wd_timeout_type_t timeout_type)
{
    val_status_t    status = VAL_STATUS_SUCCESS;
#ifdef WATCHDOG_AVAILABLE
    /* Disable watchdog Timer */
    val_watchdog_disable();

    /* Initialise watchdog */
    status = val_wd_timer_init(timeout_type);
    if (VAL_ERROR(status))
    {
        return status;
    }

    /* Enable watchdog Timer */
    status = val_watchdog_enable();
    if (VAL_ERROR(status))
    {
        return status;
    }
#else
	(void)timeout_type; // Argument unused if WATCHDOG_AVAILABLE is not defined
#endif

    return status;
}
