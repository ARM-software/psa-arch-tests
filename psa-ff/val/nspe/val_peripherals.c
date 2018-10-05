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

#include "val_peripherals.h"
#include "val_target.h"
#include "pal_interfaces_ns.h"
#include "val_framework.h"
#include "val_client_defs.h"

/* Global */
uint32_t   is_uart_init_done = 0;

/**
    @brief    - This API will read the necessary target config info
                and pass it to driver partition to initialise the driver partition
                global variables
    @param    - void
    @return   - error status
**/
val_status_t val_target_init(void)
{
    target_param_t          target_param;
    val_status_t            status = VAL_STATUS_SUCCESS;
    psa_handle_t            handle = 0;
    psa_status_t            status_of_call = PSA_SUCCESS;
    soc_peripheral_desc_t   *uart_desc;
    soc_peripheral_desc_t   *soc_per_desc;
    memory_desc_t           *memory_desc;

    status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL,
                                    SOC_PERIPHERAL_UART, 0),
                                   (uint8_t **)&uart_desc,
                                   (uint32_t *)sizeof(soc_peripheral_desc_t));
    if (VAL_ERROR(status))
    {
         return status;
    }

    target_param.uart_base_addr = uart_desc->base;

    status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL,
                                    SOC_PERIPHERAL_WATCHDOG, 0),
                                    (uint8_t **)&soc_per_desc,
                                    (uint32_t *)sizeof(soc_peripheral_desc_t));
    if (VAL_ERROR(status))
    {
         return status;
    }
    target_param.wd_base_addr = soc_per_desc->base;
    target_param.wd_time_us_low = soc_per_desc->timeout_in_micro_sec_low;
    target_param.wd_time_us_medium = soc_per_desc->timeout_in_micro_sec_medium;
    target_param.wd_time_us_high = soc_per_desc->timeout_in_micro_sec_high;
    target_param.wd_timer_tick_us = soc_per_desc->num_of_tick_per_micro_sec;

    status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_NVMEM, 0),
                                   (uint8_t **)&memory_desc,
                                   (uint32_t *)sizeof(memory_desc_t));

    target_param.nvmem_base_addr = memory_desc->start;

    if (VAL_ERROR(status))
    {
         return status;
    }

    psa_invec invec[1] = {{&target_param, sizeof(target_param)}};
    handle = psa_connect(DRIVER_TARGET_INIT_SID, 0);
    if (handle < 0)
    {
        return VAL_STATUS_CONNECTION_FAILED;
    }
    else
    {
        status_of_call = psa_call(handle, invec, 1, NULL, 0);
        if (status_of_call != PSA_SUCCESS)
        {
            psa_close(handle);
            return VAL_STATUS_CALL_FAILED;
        }
    }
   psa_close(handle);
   return VAL_STATUS_SUCCESS;
}

/*
    @brief    - Initialize UART.
                This is client interface API of secure partition UART INIT API.
    @param    - None
    @return   - val_status_t
*/
val_status_t val_uart_init(void)
{
    psa_handle_t            print_handle = 0;
    psa_status_t            status_of_call = PSA_SUCCESS;
    uint32_t                uart_init_sign = UART_INIT_SIGN;
    uint32_t                verbosity = VERBOSE;

    psa_invec data[3] = {{&uart_init_sign, sizeof(uart_init_sign)},
                         {&verbosity, sizeof(verbosity)}};

    print_handle = psa_connect(DRIVER_UART_SID, 0);
    if (print_handle < 0)
    {
        return(VAL_STATUS_CONNECTION_FAILED);
    }

    status_of_call = psa_call(print_handle, data, 3, NULL, 0);
    if (status_of_call != PSA_SUCCESS)
    {
        return(VAL_STATUS_CALL_FAILED);
    }

    is_uart_init_done = 1;
    psa_close(print_handle);
    return VAL_STATUS_SUCCESS;
}

/**
    @brief    - Print module. This is client interface API of secure partition
                val_print_sf API for nspe world
    @param    - verbosity: Print verbosity level
              - string   : Input string
              - data     : Value for format specifier
    @return   - val_status_t
**/
val_status_t val_print(print_verbosity_t verbosity, char *string, uint32_t data)
{
    int             string_len = 0;
    char            *p = string;
    psa_handle_t    print_handle = 0;
    psa_status_t    status_of_call = PSA_SUCCESS;
    val_status_t    status = VAL_STATUS_SUCCESS;

    if ((is_uart_init_done == 0) || (verbosity < VERBOSE))
    {
       return 0;
    }
    while (*p != '\0')
    {
        string_len++;
        p++;
    }

    psa_invec data1[3] = {{&verbosity, 4}, {string, string_len+1}, {&data, 4}};
    print_handle = psa_connect(DRIVER_UART_SID, 0);

    if (print_handle < 0)
    {
        return VAL_STATUS_CONNECTION_FAILED;
    }
    else
    {
        status_of_call = psa_call(print_handle, data1, 3, NULL, 0);
        if (status_of_call != PSA_SUCCESS)
        {
            status = VAL_STATUS_CALL_FAILED;
        }
    }
    psa_close(print_handle);
    return status;
}

/**
    @brief    - This API will read from slave address via SPI
    @param    - addr : Slave address
                data : value read from Slave address
                len  : length of data to be read in bytes
    @return   - error status
**/
val_status_t val_spi_read(addr_t addr, uint8_t *data, uint32_t len)
{
        return pal_spi_read(addr, data, len);
}

/* Watchdog APIs */
/**
    @brief    - Initializes the WatchDog Timer instance. This is client interface API of
                secure partition val_wd_timer_init_sf API for nspe world.
    @param    timeout: watchdog timeout value to be programmed
              Defines to be used are WD_LOW_TIMEOUT, WD_MEDIUM_TIMEOUT and WD_HIGH_TIMEOUT
    @return   - error status
**/
val_status_t val_wd_timer_init(wd_timeout_type_t timeout_type)
{
    wd_param_t      wd_param;
    psa_handle_t    handle = 0;
    psa_status_t    status_of_call = PSA_SUCCESS;

    wd_param.wd_fn_type = WD_INIT_SEQ;
    wd_param.wd_timeout_type = timeout_type;
    psa_invec invec[1] = {{&wd_param, sizeof(wd_param)}};

    handle = psa_connect(DRIVER_WATCHDOG_SID, 0);
    if (handle < 0)
    {
        return VAL_STATUS_CONNECTION_FAILED;
    }
    else
    {
        status_of_call = psa_call(handle, invec, 1, NULL, 0);
        if (status_of_call != PSA_SUCCESS)
        {
            psa_close(handle);
            return VAL_STATUS_CALL_FAILED;
        }
    }
   psa_close(handle);
   return VAL_STATUS_SUCCESS;
}

/**
    @brief    - Enable WatchDog Timer instance. This is client interface API of
                secure partition val_wd_timer_enable_sf API for nspe world.
    @return   - error status
**/
val_status_t val_wd_timer_enable(void)
{
    wd_param_t      wd_param;
    psa_handle_t    handle = 0;
    psa_status_t    status_of_call = PSA_SUCCESS;

    wd_param.wd_fn_type = WD_ENABLE_SEQ;
    psa_invec invec[1] = {{&wd_param, sizeof(wd_param)}};

    handle = psa_connect(DRIVER_WATCHDOG_SID, 0);
    if (handle < 0)
    {
        return VAL_STATUS_CONNECTION_FAILED;
    }else
    {
        status_of_call = psa_call(handle, invec, 1, NULL, 0);
        if (status_of_call != PSA_SUCCESS)
        {
            psa_close(handle);
            return VAL_STATUS_CALL_FAILED;
        }
    }
   psa_close(handle);
   return VAL_STATUS_SUCCESS;
}

/**
    @brief    - Disable Watch Dog Timer instance. This is client interface API of
                secure partition val_wd_timer_disable_sf API for nspe world.
    @return   - error status
**/
val_status_t val_wd_timer_disable(void)
{
    wd_param_t      wd_param;
    psa_handle_t    handle = 0;
    psa_status_t    status_of_call = PSA_SUCCESS;

    wd_param.wd_fn_type = WD_DISABLE_SEQ;
    psa_invec invec[1] = {{&wd_param, sizeof(wd_param)}};

    handle = psa_connect(DRIVER_WATCHDOG_SID, 0);
    if (handle < 0)
    {
        return VAL_STATUS_CONNECTION_FAILED;
    }
    else
    {
        status_of_call = psa_call(handle, invec, 1, NULL, 0);
        if (status_of_call != PSA_SUCCESS)
        {
            psa_close(handle);
            return VAL_STATUS_CALL_FAILED;
        }
    }
   psa_close(handle);
   return VAL_STATUS_SUCCESS;
}

/**
    @brief    - Checks if watchdog enabled. This is client interface API of
                secure partition val_is_wd_timer_enabled_sf API for nspe world.
    @return   - error status
**/
val_status_t val_is_wd_timer_enabled(void)
{
    wd_param_t      wd_param;
    psa_handle_t    handle = 0;
    psa_status_t    status_of_call = PSA_SUCCESS;

    wd_param.wd_fn_type = WD_STATUS_SEQ;
    psa_invec invec[1] = {{&wd_param, sizeof(wd_param)}};

    handle = psa_connect(DRIVER_WATCHDOG_SID, 0);
    if (handle < 0)
    {
        return VAL_STATUS_CONNECTION_FAILED;
    }
    else
    {
        status_of_call = psa_call(handle, invec, 1, NULL, 0);
        if (status_of_call != PSA_SUCCESS)
        {
            psa_close(handle);
            return VAL_STATUS_CALL_FAILED;
        }
    }
   psa_close(handle);
   return VAL_STATUS_SUCCESS;
}

/*
    @brief     - Reads 'size' bytes from Non-volatile memory at a given. This is client interface
                API of secure partition val_nvmem_read_sf API for nspe world.
                'base + offset' into given buffer.
               - offset    : Offset from NV MEM base address
               - buffer    : Pointer to source address
               - size      : Number of bytes
    @return    - val_status_t
*/
val_status_t val_nvmem_read(uint32_t offset, void *buffer, int size)
{
    nvmem_param_t   nvmem_param;
    psa_handle_t    handle = 0;
    psa_status_t    status_of_call = PSA_SUCCESS;

    nvmem_param.nvmem_fn_type = NVMEM_READ;
    nvmem_param.offset = offset;
    nvmem_param.size = size;
    psa_invec invec[1] = {{&nvmem_param, sizeof(nvmem_param)}};
    psa_outvec outvec[1] = {{buffer, size}};

    handle = psa_connect(DRIVER_NVMEM_SID, 0);
    if (handle < 0)
    {
        return VAL_STATUS_CONNECTION_FAILED;
    }
    else
    {
        status_of_call = psa_call(handle, invec, 1, outvec, 1);
        if (status_of_call != PSA_SUCCESS)
        {
            psa_close(handle);
            return VAL_STATUS_CALL_FAILED;
        }
    }
   psa_close(handle);
   return VAL_STATUS_SUCCESS;
}

/*
    @brief     - Writes 'size' bytes from buffer into non-volatile memory at a given
                 'base + offset'.  This is client interface API of secure partition
                 val_nvmem_write_sf API for nspe world.
               - offset    : Offset
               - buffer    : Pointer to source address
               - size      : Number of bytes
    @return    - val_status_t
*/
val_status_t val_nvmem_write(uint32_t offset, void *buffer, int size)
{
    nvmem_param_t   nvmem_param;
    psa_handle_t    handle = 0;
    psa_status_t    status_of_call = PSA_SUCCESS;

    nvmem_param.nvmem_fn_type = NVMEM_WRITE;
    nvmem_param.offset = offset;
    nvmem_param.size = size;
    psa_invec invec[2] = {{&nvmem_param, sizeof(nvmem_param)}, {buffer, size}};

    handle = psa_connect(DRIVER_NVMEM_SID, 0);
    if (handle < 0)
    {
        return VAL_STATUS_CONNECTION_FAILED;
    }
    else
    {
        status_of_call = psa_call(handle, invec, 2, NULL, 0);
        if (status_of_call != PSA_SUCCESS)
        {
            psa_close(handle);
            return VAL_STATUS_CALL_FAILED;
        }
    }
   psa_close(handle);
   return VAL_STATUS_SUCCESS;
}
