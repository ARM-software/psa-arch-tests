/** @file
 * Copyright (c) 2018-2025, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 * Copyright 2023 NXP
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
#ifndef _PAL_INTERFACES_NS_H_
#define _PAL_INTERFACES_NS_H_

#include "pal_common.h"
#include "pal_common_val_intf.h"
#include <stdarg.h>

/**
    @brief    - This function initializes the UART
    @param    - Void
    @return   - SUCCESS/FAILURE
**/
int pal_uart_init_ns(void);

/**
    @brief    - This function parses the input string and writes bytes into UART TX FIFO
    @param    - str      : Input String
              - data     : Value for format specifier
    @return   - SUCCESS/FAILURE
**/
int pal_print_ns(const char *str, int32_t data);

/**
 *   @brief           - Initializes a hardware watchdog timer
 *   @param           - time_us         : Time in micro seconds
 *                    - timer_tick_us   : Number of ticks per micro second
 *   @return          - SUCCESS/FAILURE
**/
int pal_wd_timer_init_ns(uint32_t time_us, uint32_t timer_tick_us);

/**
 *   @brief    - This API will call the requested crypto function
 *   @param    - type    : function code
 *               valist  : variable argument list
 *   @return   - error status
**/
int32_t pal_crypto_function(int type, va_list valist);

/**
 *   @brief    - This API will call the requested internal trusted storage function
 *   @param    - type    : function code
 *               valist  : variable argument list
 *   @return   - error status
**/
uint32_t pal_its_function(int type, va_list valist);

/**
 *   @brief    - This API will call the requested protected storage function
 *   @param    - type    : function code
 *               valist  : variable argument list
 *   @return   - error status
**/
uint32_t pal_ps_function(int type, va_list valist);

/**
 *   @brief    - This API will call the requested attestation function
 *   @param    - type    : function code
 *               valist  : variable argument list
 *   @return   - error status
**/
int32_t pal_attestation_function(int type, va_list valist);

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

/**
 *   @brief    - Sets the custom test list buffer
 *   @param    - custom_test_list : Custom test list buffer
     @return   - void
**/
void pal_set_custom_test_list(char *custom_test_list);

/**
 *   @brief    - Tells if a test is enabled on platform
 *   @param    - test_id : Test ID
 *   @return   - TRUE/FALSE
**/
bool_t pal_is_test_enabled(test_id_t test_id);

/**
 *   @brief    - initialize anything relavent to a platform
 *   @return   - TRUE/FALSE
**/
unsigned int pal_platform_init(void);
#endif
