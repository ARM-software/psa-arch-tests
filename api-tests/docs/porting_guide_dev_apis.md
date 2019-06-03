
# Porting Guide: Developer APIs Architecture Test Suite
-----------------------------------------------------

## Introduction
The architecture test suite contains the *Platform Abstraction Layer* (PAL) which abstracts platform-specific information from the tests. You must implement and port the PAL interface functions to your target platform. Create and update the target configuration file to match the details of this target platform.

This document provides the porting steps and the list of PAL APIs.

## Porting steps

### Target configuration

You must populate your system configuration and provide it as an input to the test suite. The system configuration is captured in a single static input configuration file called **target.cfg**, available at **api-tests/platform/targets/<platform_name>/**. <br />

An example input configuration file is as shown.

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

  For details on the structure of the input, refer to **val/common/val_target.h**.

### Adding a new target

  1. Create a new directory in **platform/targets/<platform_name>**. For reference, see the existing platform **tgt_dev_apis_mbedos_fvp_mps2_m4** directory.
  2. Execute `cp -rf platform/targets/tgt_dev_apis_mbedos_fvp_mps2_m4/ platform/targets/<platform_name>/`.
  3. Update **platform/targets/<platform_name>/target.cfg** with your platform details. Refer to **val/common/val_target.h** for structure details.
  4. Update **platform/targets/<platform_name>/Makefile** appropriately to select the correct instances of PAL files for compilation. To compile **dev_apis** suites, you must set **PSA_IPC_IMPLEMENTED** to 0. This selects the Non-secure PAL instances for the driver services and eliminates IPC dependency for dev_apis tests.
  5. Refer to the **List of PAL APIs** section to view the list of PAL APIs that must be ported for your target platform. These API definitions are available in **nspe/<suite_name>/pal_\*\_intf.c**. These APIs are written for tgt_dev_apis_mbedos_fvp_mps2_m4 platform. You can reuse the code if it works for your platform. Otherwise, you must port them for your platform-specific peripherals.
  6. Update Crypto configuration file **nspe/crypto/pal_crypto_config.h** to enable or disable Crypto features selectively for the Crypto test suite.

**Note**:
- The platform makefile is invoked as part of test suite build tool (**./setup.sh**) step and it creates **<build_dir>/BUILD/platform/pal_nspe.a** archive.
- The test suite requires access the following peripherals:
  - One UART to print NSPE and SPE messages
  - One Watchdog timer to help recover from any fatal error conditions
  - Non-volatile memory support to preserve test status over watchdog timer reset


## List of PAL APIs
Since the test suite is agnostic to various system targets, you must port the following PAL NSPE APIs before building the tests. Implement these functions for your target platform. <br />

| No | Prototype                                                                                                                   | Description                                                            | Parameters                                               |
|----|-----------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------|----------------------------------------------------------|
| 01 | int pal_spi_read(addr_t addr, uint8_t *data, uint32_t len);                                                                 | Reads peripherals using SPI commands                 | addr : Address of the peripheral<br/>data : Read buffer<br/>len  : Length of the read buffer in bytes<br/>                    |
| 02 | int pal_uart_init_ns(uint32_t uart_base_addr);                                                                              | Initializes the UART                                     | UART base address<br/>                                      |
| 03 | int pal_print_ns(char *str, int32_t data);                                                                                 | Parses the input string and writes bytes into UART TX FIFO| str      : Input String<br/>data     : Value for format specifier<br/>                             |
| 04 | int pal_wd_timer_init_ns(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us);                                       | Initializes a hardware watchdog timer                                 | base_addr       : Base address of the watchdog module<br/>time_us         : Time in micro seconds<br/>timer_tick_us   : Number of ticks per micro second<br/>|
| 05 | int pal_wd_timer_enable_ns(addr_t base_addr);                                                                               | Enables a hardware watchdog timer                                      | base_addr       : Base address of the watchdog module<br/>|
| 06 | int pal_wd_timer_disable_ns(addr_t base_addr);                                                                              | Disables a hardware watchdog timer                                     | base_addr  : Base address of the watchdog module<br/>    |
| 07 | int pal_nvmem_read_ns(addr_t base, uint32_t offset, void *buffer, int size);                                                | Reads from the given non-volatile address.                                 | base    : Base address of nvmem<br/>offset  : Offset<br/>buffer  : Pointer to source address<br/>size    : Number of bytes<br/>                     |
| 08 | int pal_nvmem_write_ns(addr_t base, uint32_t offset, void *buffer, int size);                                               | Writes into given non-volatile address.                                | base    : Base address of nvmem<br/>offset  : Offset<br/>buffer  : Pointer to source address<br/>size    : Number of bytes<br/>                     |
| 09 | int32_t pal_crypto_function(int type, va_list valist);                                                                     | Calls the requested Crypto function                       | type    : Function code<br/>valist  : variable argument list<br/>                             |
| 10 | uint32_t pal_its_function(int type, va_list valist);                                                                     | Calls the requested Internal Trusted Storage  function                       | type    : Function code<br/>valist  : Variable argument list<br/>                             |
| 11 | uint32_t pal_ps_function(int type, va_list valist);                                                                     | Calls the requested Protected Storage  function                       | type    : Function code<br/>valist  : Variable argument list<br/>                             |
| 12 | int32_t pal_attestation_function(int type, va_list valist);                                                                | Calls the requested Initial Attestation  function                       | type    : Function code<br/>valist  : Variable argument list<br/>                             |

## License
Arm PSA test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2019, Arm Limited and Contributors. All rights reserved.*
