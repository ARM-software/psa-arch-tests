
# Porting Guide - PSA FF Architecture Test Suite
-----------------------------------------------------

## Introduction
The PSA FF Architecture test suite contains a platform abstraction layer (PAL) which abstracts platform specific information from the tests.
 - The PAL layer interface functions need to be implemented/ported to the target platform.
 - The target config file must be created/updated to match the details of the target platform.

This document provides details on the porting steps and the PAL APIs.

## Porting steps

### Target configuration

  You must populate your system configuration and provide it as an input to test suite.

This is captured in a single static input configuration file that is named as target.cfg. This file is available at api-tests/platform/targets/<platform_name>/. <br />

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

**Note**:
  Test suite needs access to the following peripherals and the services of these peripherals are implemented as RoT services in the driver partition.
  - One UART to print nspe and spe messages
  - One Watchdog timer to help recovery from any fatal error conditions
  - Non-volatile memory support to preserve test status over watchdog timer reset

### Create a new target

  Since PSA FF test suite is agnostic to various system targets, before building the tests, you must port the files mentioned in the following steps.

**Procedure**
----------------

  - Create a new directory in platform/targets/<platform_name>. For reference, see the existing platform fvp_mps2_cm4_mbedos directory.
  - cp -rf platform/targets/fvp_mps2_cm4_mbedos/ platform/targets/<platform_name>/
  - Update platform/targets/<platform_name>/target.cfg with your platform detail. Refer val/common/val_target.h for structure details.
  - Update mmio_regions information available in platform/targets/<platform_name>/manifests/common/driver_partition_psa.json manifest file with your platform information. mmio_regions detail must match with device detail provided in the target.cfg file.
  - The test suite specified "sid" and partition "id" are provided in manifest files available in platform/manifests/ directory. You can update them if they are clashing with your system defined sid and partition-id values. You may need to update platform/targets/<platform_name>/nspe/pal_sid.h file for any change in test suite provided SID values.
  - Update platform/nspe/Makefile appropriately to select correct instances of PAL files for compilation. This make file is invoked as part of test suite build tool(./setup.sh) step and it creates <build_dir>/BUILD/platform/pal_nspe.a archive and respective object for SPE files at <build_dir>/BUILD/platform/spe/\*\_driver_sp.o. Later, these SPE objects are used by spbuild.mk to create appropriate SPE partition archive file.
  - Refer "PAL API list" section to view list of PAL API that must be ported for your target platform. These APIs definitions are available in nspe/pal_\*\_intf.c and spe/pal_\*_intf.c files. These APIs are written for fvp_mps2_cm4_mbedos platform. You can reuse the code if it works for your platform. Otherwise you must port them for your platform specific peripherals.

## PAL API list
  These functions will require implementation for the target platform. <br />

- Following are the list of PAL APIs used in NSPE: <br />

| No | Prototype                                                                                                                   | Description                                                            | Parameters                                               |
|----|-----------------------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------|----------------------------------------------------------|
| 01 | int pal_spi_read(addr_t addr, uint8_t *data, uint32_t len);                                                                 | This function will read peripherals using SPI commands                 | addr : address of the peripheral<br/>data : read buffer<br/>len  : length of the read buffer in bytes<br/>                    |
| 02 | uint32_t pal_ipc_framework_version(void);                                                                                   | Retrieve the version of the PSA Framework API that is implemented.     | void<br/>                                                |
| 03 | uint32_t pal_ipc_version(uint32_t sid);                                                                                     | Retrieve the minor version of a Root of Trust Service by its SID.      | sid The Root of Trust Service ID<br/>                    |
| 04 | psa_handle_t pal_ipc_connect(uint32_t sid, uint32_t minor_version);                                                         | Connect to given sid.                                                  | sid : RoT service id<br/>minor_version : minor_version of RoT service<br/>                                |
| 05 | psa_status_t pal_ipc_call(psa_handle_t handle, const psa_invec *in_vec, size_t in_len,  psa_outvec *out_vec, size_t out_len);| Call a connected Root of Trust Service.                                | handle:   Handle for the connection.<br/>in_vec:   Array of psa_invec structures.<br/>in_len:   Number of psa_invec structures in in_vec.<br/>out_vec:  Array of psa_outvec structures for optional Root of Trust Service response.<br/>out_len:  Number of psa_outvec structures in out_vec.<br/>                |
| 06 | void pal_ipc_close(psa_handle_t handle);                                                                                    | Close a connection to a Root of Trust Service.                         | handle Handle for the connection.<br/>                   |
| 07 | int pal_uart_init_ns(uint32_t uart_base_addr);                                                                              | This function initializes the UART                                     | uart base addr<br/>                                      |
| 08 | int pal_print_ns(char *str, uint32_t data);                                                                                 | This function parses the input string and writes bytes into UART TX FIFO| str      : Input String<br/>data     : Value for format specifier<br/>                             |
| 09 | int pal_wd_timer_init_ns(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us);                                       | Initializes an hardware watchdog timer                                 | base_addr       : Base address of the watchdog module<br/>time_us         : Time in micro seconds<br/>timer_tick_us   : Number of ticks per micro second<br/>|
| 10 | int pal_wd_timer_enable_ns(addr_t base_addr);                                                                               | Enables a hardware watchdog timer                                      | base_addr       : Base address of the watchdog module<br/>|
| 11 | int pal_wd_timer_disable_ns(addr_t base_addr);                                                                              | Disables a hardware watchdog timer                                     | base_addr  : Base address of the watchdog module<br/>    |
| 12 | int pal_nvmem_read_ns(addr_t base, uint32_t offset, void *buffer, int size);                                                | Reads from given non-volatile address.                                 | base    : Base address of nvmem<br/>offset  : Offset<br/>buffer  : Pointer to source address<br/>size    : Number of bytes<br/>                     |
| 13 | int pal_nvmem_write_ns(addr_t base, uint32_t offset, void *buffer, int size);                                               | Writes into given non-volatile address.                                | base    : Base address of nvmem<br/>offset  : Offset<br/>buffer  : Pointer to source address<br/>size    : Number of bytes<br/>                     |
| 14 | uint32_t pal_crypto_function(int type, va_list valist);                                                                     | This API will call the requested crypto function                       | type    : function code<br/>valist  : variable argument list<br/>                             |

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
Arm PSA FF Architecture test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2018, Arm Limited and Contributors. All rights reserved.*
