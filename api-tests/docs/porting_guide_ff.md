
# Porting Guide: PSA-FF Architecture Test Suite
-----------------------------------------------------

## Introduction
The architecture test suite contains a *Platform Abstraction Layer* (PAL) which abstracts platform-specific information from the tests. You must implement and port the PAL interface functions to your target platform. Create and update the target configuration file to match the details of this target platform.

This document provides the porting steps and the list of PAL APIs.

## Porting steps

### Target configuration

You must populate your system configuration and provide it as an input to the test suite. The system configuration is captured in an input configuration header file called **pal_config.h**, available at **api-tests/platform/targets/<platform_name>/nspe/**. <br />

An example input configuration header is as shown:

    // UART device info
    #define UART_NUM              1
    #define UART_0_BASE           0x40004000
    #define UART_0_SIZE           0xFFF
    #define UART_0_INTR_ID        0xFF
    #define UART_0_PERMISSION     TYPE_READ_WRITE

    // Watchdog device info
    #define WATCHDOG_NUM          1
    #define WATCHDOG_0_BASE       0x40008000
    #define WATCHDOG_0_SIZE       0xFFF
    #define WATCHDOG_0_INTR_ID    0xFF
    #define WATCHDOG_0_PERMISSION TYPE_READ_WRITE

   For details on the structure of the input, refer to **api-tests/platform/targets/tgt_ff_tfm_an521/nspe/pal_config.h**.

### Adding a new target

  1. Create a new directory in **platform/targets/<platform_name>**. For reference, see the existing platform **tgt_ff_tfm_an521** directory.
  2. Execute `cp -rf platform/targets/tgt_ff_tfm_an521/ platform/targets/<platform_name>/`.
  3. Update **platform/targets/<platform_name>/nspe/pal_config.h** with your target platform details. Refer to **api-tests/platform/targets/tgt_ff_tfm_an521/nspe/pal_config.h** for structure details.
  4. Update the platform information available in manifest files located in  **platform/manifests/** directory with your platform information. The platform details must match the device details provided in the pal_config.h file.
  5. Update **platform/targets/<platform_name>/target.cmake** appropriately to select the correct instances of PAL files for compilation.
  6. Refer to the **List of PAL APIs** section to view the list of PAL APIs that must be ported for your target platform. These API definitions are available in **nspe/<suite_name>/pal_\*\_intf.c** and **spe/pal_\*\_intf.c** files. These APIs are written for **tgt_ff_tfm_an521** platform. You can reuse the code if it works for your platform. Otherwise, you must port them for your platform-specific peripherals.

**Note**:
- The test suite requires access to the peripherals mentioned below. For IPC suite, driver functionalities are implemented as RoT-services in driver partition. Other Secure partitions and Non-secure code calls to these RoT-services to get appropriate driver services.
  - One UART to print Test NSPE or SPE messages and to cover secure partition interrupt handling scenarios
  - One Watchdog timer to help recover from any fatal error conditions
  - Non-volatile memory support to preserve test status over watchdog timer reset. Each byte of this region must be initialised to FF at power on reset.


## List of PAL APIs

Since the test suite is agnostic to various system targets, you must port the following PAL NSPE APIs before building the tests. Implement these functions for your target platform. <br />

The following is the list of PAL APIs used in SPE: <br />

| No | Prototype                                                                         | Description                                                                       | Parameters                                               |
|----|-----------------------------------------------------------------------------------|-----------------------------------------------------------------------------------|----------------------------------------------------------|
| 01 | int pal_uart_init_ns(void);                                                       | Initializes the UART                                                              | None<br/>                                                |
| 02 | int pal_print(uint8_t c);                                                         | Sends a character to the UART TX FIFO                                             | c  :  Input character<br/>                                             |
| 03 | int pal_print_ns(const char *str, int32_t data);                                  | Parses the input string and writes byte by byte to print (Non-secure side)        | str            : Input String<br/>data           : Value for Format specifier<br/>                       |
| 04 | void pal_print_s(const char *str, int32_t data);                                  | Parses the input string and writes byte by byte to print (Secure side)            | str            : Input String<br/>data           : Value for Format specifier<br/>                       |
| 05 | int  pal_wd_timer_init(uint32_t time_us, uint32_t timer_tick_us);                 | Initializes a hardware watchdog timer                                             | time_us         : Time in micro seconds<br/>timer_tick_us   : Number of ticks per micro second<br/>|
| 06 | int pal_watchdog_enable(void);                                                    | Enables a hardware watchdog timer                                                 | None<br/>|
| 07 | int pal_watchdog_disable(void);                                                   | Disables a hardware watchdog timer                                                | None<br/>    |
| 08 | int  pal_wd_timer_is_enabled(addr_t base_addr);                                   | Checks whether hardware watchdog timer is enabled                                 | base_addr     : Base address of the watchdog module<br/> |
| 09 | int pal_nvm_read(uint32_t offset, void *buffer, size_t size);                     | Reads from the given non-volatile address.                                        | offset  : Offset<br/>buffer  : Pointer to source address<br/>size    : Number of bytes<br/>                     |
| 10 | int pal_nvm_write(uint32_t offset, void *buffer, size_t size);                    | Writes into given non-volatile address.                                           | offset  : Offset<br/>buffer  : Pointer to source address<br/>size    : Number of bytes<br/>                     |
| 11 | void pal_generate_interrupt(void);                                                | Trigger interrupt for IRQ signal assigned to driver partition                     | None |
| 12 | void pal_disable_interrupt(void);                                                 | Disable the interrupt that was generated using pal_generate_interrupt API.        | None |

## License
Arm PSA test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2018-2025, Arm Limited and Contributors. All rights reserved.*
