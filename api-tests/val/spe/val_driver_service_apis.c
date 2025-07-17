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


#include "val_driver_service_apis.h"
#include "val_common_log.h"

print_verbosity_t  g_print_level = INFO;
static int is_uart_init_done;
const uint32_t total_tests;

/* Use raw print for driver partition */
#ifdef DRIVER_PARTITION_INCLUDE
#define val_print(x, y, z)                        \
       do {                                       \
           if (x >= VERBOSITY)                    \
              val_print_sf(x, y, z);              \
       } while (0)
#else
__UNUSED STATIC_DECLARE val_status_t val_print
                        (print_verbosity_t verbosity, char *string, int32_t data);
#endif

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
    is_uart_init_done = 1;
    return VAL_STATUS_SUCCESS;
}
/*
    @brief     - This function parses the input string and writes byte by byte to
                 print the input string
    @param    - verbosity: Print verbosity level
              - string   : Input string
              - data     : Value for format specifier
    @return    - error status
 */
val_status_t val_print_sf(print_verbosity_t verbosity, const char *string, int32_t data)
{
    if ((is_uart_init_done == 0) || (verbosity < VERBOSITY))
    {
       return VAL_STATUS_SUCCESS;
    }
#ifdef BESPOKE_PRINT_S
    return pal_print_s(string, data);
#else
    return val_printf(verbosity, string, data);
#endif
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

/**
    @brief    - This function sets the given boot.state value to corresponding
                boot NVMEM location
    @param    - state: boot_state_t
    @return   - val_status_t
**/
val_status_t val_driver_private_set_boot_flag_fn(boot_state_t state)
{
   boot_t           boot;
   val_status_t     status;

   boot.state = state;
   status = val_nvmem_write_sf(PLATFORM_NVM_BASE,
                               VAL_NVM_OFFSET(NVM_BOOT),
                               &boot,
                               sizeof(boot_t));
   if (VAL_ERROR(status))
   {
       val_print_sf(ERROR, "val_nvmem_write_sf failed Error=0x%x\n", status);
       return status;
   }
   return status;
}

/**
    @brief    - This function initialises the driver reserved mmio region
    @param    - void
    @return   - val_status_t
**/
val_status_t val_init_driver_memory(void)
{
   /* Init driver mmio space to 0 to avoid uninit access */
   memset((uint32_t *)PLATFORM_DRIVER_PARTITION_MMIO_START, 0,
                (PLATFORM_DRIVER_PARTITION_MMIO_END - PLATFORM_DRIVER_PARTITION_MMIO_START + 1));

   return VAL_STATUS_SUCCESS;
}

/**
    @brief    - This function returns the driver reserved mmio region base
    @param    - base pointer
    @return   - val_status_t
**/
val_status_t val_get_driver_mmio_addr(addr_t *base_addr)
{
   *base_addr = PLATFORM_DRIVER_PARTITION_MMIO_START;
   return VAL_STATUS_SUCCESS;
}

/**
    @brief   - Trigger interrupt for irq signal assigned to driver partition
               before return to caller.
    @param   - void
    @return  - void
**/
void val_generate_interrupt(void)
{
    pal_generate_interrupt();
}

/**
    @brief   - Disable interrupt that was generated using val_generate_interrupt API.
    @param   - void
    @return  - void
**/
void val_disable_interrupt(void)
{
    pal_disable_interrupt();
}
