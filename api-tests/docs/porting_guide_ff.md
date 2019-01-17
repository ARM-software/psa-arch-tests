
# Porting Guide - PSA-FF Architecture Test Suite
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

  - Create a new directory in platform/targets/<platform_name>. For reference, see the existing platform tgt_ff_mbedos_fvp_mps2_m4 directory.
  - cp -rf platform/targets/tgt_ff_mbedos_fvp_mps2_m4/ platform/targets/<platform_name>/
  - Update platform/targets/<platform_name>/target.cfg with your platform detail. Refer val/common/val_target.h for structure details.
  - Update the platform information available in manifest files located in  platform/targets/<platform_name>/manifests/ directory with your platform information. The platform detail must match with device detail provided in the target.cfg file.
  - Update platform/nspe/Makefile appropriately to select correct instances of PAL files for compilation. To compile IPC tests, you must set PSA_IPC_IMPLEMENTED to 1 and remaining Developer APIs related variables to 0. This selects the secure PAL instances for the driver services and eliminates dev_apis dependancy for IPC tests.
  - Refer "PAL API list" section to view list of PAL APIs that must be ported for your target platform. These APIs definitions are available in nspe/pal_\*\_intf.c and spe/pal_\*_intf.c files. These APIs are written for tgt_ff_mbedos_fvp_mps2_m4 platform. You can reuse the code if it works for your platform. Otherwise you must port them for your platform specific peripherals.
  - The platform make file is invoked as part of test suite build tool(./setup.sh) step and it creates <build_dir>/BUILD/platform/pal_nspe.a archive for NPSE files and respective object for SPE files at <build_dir>/BUILD/platform/spe/\*\_driver_sp.o. Later, these SPE objects are used by spbuild.mk to create appropriate SPE partition archive file.

**Note**:
  Test suite needs access to the following peripherals. When PSA_IPC_IMPLEMENTED is set to 1, driver functionalities are implemented as RoT-services in driver partition. Other Secure partitions and non-secure code calls to these RoT-services to get appropriate driver services.
  - One UART to print nspe and spe messages
  - One Watchdog timer to help recovery from any fatal error conditions
  - Non-volatile memory support to preserve test status over watchdog timer reset


## PAL API list
Since Test suite is agnostic to various system targets, before building the tests, you must port below PAL APIs. These functions will require implementation for your target platform. <br />

- Following are the list of PAL APIs used in NSPE: <br />

| No | Prototype                                                                                                                   | Description                                                            | Parameters                                               |
|----|-----------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------|----------------------------------------------------------|
| 01 | int pal_spi_read(addr_t addr, uint8_t *data, uint32_t len);                                                                 | This function will read peripherals using SPI commands                 | addr : address of the peripheral<br/>data : read buffer<br/>len  : length of the read buffer in bytes<br/>                    |

- Following are the list of PAL APIs used in SPE: <br />

| No | Prototype                                                                         | Description                                                                       | Parameters                                               |
|----|-----------------------------------------------------------------------------------|-----------------------------------------------------------------------------------|----------------------------------------------------------|
| 01 | void pal_uart_init(addr_t uart_base_addr);                                        | This function initializes the uart                                                | uart_base_addr : Base address of the UART<br/>           |
| 02 | void pal_print(char *str, uint32_t data);                                         | This function parses the input string and writes byte by byte to print            | str            : Input String<br/>data           : Value for Format specifier<br/>                       |
| 03 | int  pal_wd_timer_init(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us);| Initializes an hardware watchdog timer                                            | base_addr       : Base address of the watchdog module<br/>time_us         : Time in micro seconds<br/>timer_tick_us   : Number of ticks per micro second<br/>|
| 04 | int  pal_wd_timer_enable(addr_t base_addr);                                       | Enables a hardware watchdog timer                                                 | base_addr     : Base address of the watchdog module<br/> |
| 05 | int  pal_wd_timer_disable(addr_t base_addr);                                      | Disables a hardware watchdog timer                                                | base_addr     : Base address of the watchdog module<br/> |
| 06 | int  pal_wd_timer_is_enabled(addr_t base_addr);                                   | Checks whether hardware watchdog timer is enabled                                 | base_addr     : Base address of the watchdog module<br/> |
| 07 | int  pal_nvmem_write(addr_t base, uint32_t offset, void *buffer, int size);       | Writes 'size' bytes from buffer into non-volatile memory at a given 'base + offset'| base      : Base address of NV MEM<br/>offset    : Offset<br/>buffer    : Pointer to source address<br/>size      : Number of bytes<br/>                  |
| 08 | int  pal_nvmem_read(addr_t base, uint32_t offset, void *buffer, int size);       | Reads 'size' bytes from non-volatile memory at a given                            | base      : Base address of NV MEM<br/>offset    : Offset<br/>buffer    : Pointer to source address<br/>size      : Number of bytes<br/>                  |

## License
Arm PSA test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2019, Arm Limited and Contributors. All rights reserved.*
