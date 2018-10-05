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
#ifndef _PAL_INTERFACES_NS_H_
#define _PAL_INTERFACES_NS_H_

#include "val.h"
#include <stdarg.h>

/**
    @brief    - This function will read peripherals using SPI commands
    @param    - addr : address of the peripheral
                data : read buffer
                len  : length of the read buffer in bytes
    @return   - error status
**/
int pal_spi_read(addr_t addr, uint8_t *data, uint32_t len);

/* Target Config API */
/**
    @brief    - provides the database source location.
    @param    - void
    @return   - Returns base address of database
**/
void *pal_target_get_cfg_start(void);

/**
    @brief    - This API will call the requested crypto function
    @param    - type    : function code
                valist  : variable argument list
    @return   - error status
**/
uint32_t pal_crypto_function(int type, va_list valist);
#endif
