
# Porting Guide - Developer APIs Architecture Test Suite
-----------------------------------------------------

## Introduction
The Architecture test suite contains a platform abstraction layer (PAL) which abstracts platform specific information from the tests.
 - The PAL layer interface functions need to be implemented/ported to the target platform.
 - The target config file must be created/updated to match the details of the target platform.

This document provides details on the porting steps and the PAL APIs.

## Porting steps

### Target configuration

You must populate your system configuration and provide it as an input to test suite. This is captured in a single static input configuration file that is named as target.cfg. This file is available at api-tests/platform/targets/<platform_name>/. <br />

An example of the input configuration file is as shown.

    // UART device info
    uart.num=1;
    uart.0.base = 0x40004000;
    uart.0.size = 0xFFF;
    uart.0.intr_id = 0xFF;
    uart.0.permission = TYPE_READ_WRITE;

    // Watchdog device info
    watchdog.num = 1;
    watchdog.0.base = 0x40008000;
    watchdog.0.size = 0xFFF;
    watchdog.0.intr_id = 0xFF;
    watchdog.0.permission = TYPE_READ_WRITE;

  More details on the structure of the input can be obtained from val/common/val_target.h.

### Adding a new target

  - Create a new directory in platform/targets/<platform_name>. For reference, see the existing platform tgt_dev_apis_mbedos_fvp_mps2_m4 directory.
  - cp -rf platform/targets/tgt_dev_apis_mbedos_fvp_mps2_m4/ platform/targets/<platform_name>/
  - Update platform/targets/<platform_name>/target.cfg with your platform detail. Refer val/common/val_target.h for structure details.
  - Update platform/targets/<platform_name>/Makefile appropriately to select correct instances of PAL files for compilation. To compile dev_apis suites, you must set PSA_IPC_IMPLEMENTED to 0. This selects the non-secure PAL instances for the driver services and eliminates IPC dependancy for dev_apis tests.
  - Refer "PAL API list" section to view list of PAL APIs that must be ported for your target platform. These APIs definitions are available in nspe/pal_\*\_intf.c. These APIs are written for tgt_dev_apis_mbedos_fvp_mps2_m4 platform. You can reuse the code if it works for your platform. Otherwise you must port them for your platform specific peripherals.
  -  The platform make file is invoked as part of test suite build tool(./setup.sh) step and it creates <build_dir>/BUILD/platform/pal_nspe.a archive.

**Note**:
Test suite needs access to the following peripherals:<br \>
  - One UART to print nspe and spe messages
  - One Watchdog timer to help recovery from any fatal error conditions
  - Non-volatile memory support to preserve test status over watchdog timer reset


## PAL API list
Since test suite is agnostic to various system targets, before building the tests, you must port below PAL NSPE APIs. These functions will require implementation for your target platform. <br />

| No | Prototype                                                                                                                   | Description                                                            | Parameters                                               |
|----|-----------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------|----------------------------------------------------------|
| 01 | int pal_spi_read(addr_t addr, uint8_t *data, uint32_t len);                                                                 | This function will read peripherals using SPI commands                 | addr : address of the peripheral<br/>data : read buffer<br/>len  : length of the read buffer in bytes<br/>                    |
| 02 | int pal_uart_init_ns(uint32_t uart_base_addr);                                                                              | This function initializes the UART                                     | uart base addr<br/>                                      |
| 03 | int pal_print_ns(char *str, uint32_t data);                                                                                 | This function parses the input string and writes bytes into UART TX FIFO| str      : Input String<br/>data     : Value for format specifier<br/>                             |
| 04 | int pal_wd_timer_init_ns(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us);                                       | Initializes an hardware watchdog timer                                 | base_addr       : Base address of the watchdog module<br/>time_us         : Time in micro seconds<br/>timer_tick_us   : Number of ticks per micro second<br/>|
| 05 | int pal_wd_timer_enable_ns(addr_t base_addr);                                                                               | Enables a hardware watchdog timer                                      | base_addr       : Base address of the watchdog module<br/>|
| 06 | int pal_wd_timer_disable_ns(addr_t base_addr);                                                                              | Disables a hardware watchdog timer                                     | base_addr  : Base address of the watchdog module<br/>    |
| 07 | int pal_nvmem_read_ns(addr_t base, uint32_t offset, void *buffer, int size);                                                | Reads from given non-volatile address.                                 | base    : Base address of nvmem<br/>offset  : Offset<br/>buffer  : Pointer to source address<br/>size    : Number of bytes<br/>                     |
| 08 | int pal_nvmem_write_ns(addr_t base, uint32_t offset, void *buffer, int size);                                               | Writes into given non-volatile address.                                | base    : Base address of nvmem<br/>offset  : Offset<br/>buffer  : Pointer to source address<br/>size    : Number of bytes<br/>                     |
| 09 | int32_t pal_crypto_function(int type, va_list valist);                                                                     | This API will call the requested crypto function                       | type    : function code<br/>valist  : variable argument list<br/>                             |
| 10 | uint32_t pal_its_function(int type, va_list valist);                                                                     | This API will call the requested internal trusted storage  function                       | type    : function code<br/>valist  : variable argument list<br/>                             |
| 11 | uint32_t pal_ps_function(int type, va_list valist);                                                                     | This API will call the requested protected storage  function                       | type    : function code<br/>valist  : variable argument list<br/>                             |

## License
Arm PSA test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2019, Arm Limited and Contributors. All rights reserved.*
