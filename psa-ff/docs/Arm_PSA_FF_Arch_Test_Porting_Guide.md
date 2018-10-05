
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

This is captured in a single static input configuration file that is named as target.cfg. This file is available at psa-ff/platform/targets/<platform_name>/. <br />

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

  More details on the structure of the input can be obtained from val/nspe/val_target.h.

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
  - Copy platform/targets/fvp_mps2_cm4_mbedos/target.cfg file into your <platform_name> folder and update it with your platform detail. Refer val/nspe/val_target.h for structure details.
  - Update mmio_regions information available in platform/manifests/common/driver_partition_psa.json manifest file with your platform information. mmio_regions detail must match with device detail provided in the target.cfg file.
  - The test suite specified "sid" and partition "id" are provided in manifest files available in platform/manifests/ directory. You can update them if they are clashing with your system defined sid and partition-id values. You may need to update platform/nspe/pal_sid.h file for any change in test suite provided SID values.
  - Refer "PAL API list" section to view list of PAL API that must be ported for your target platform. These APIs definitions are available in nspe/pal_\*\_intf.c and spe/pal_\*_intf.c files. These APIs are written for fvp_mps2_cm4_mbedos platform. You can reuse the code if it works for your platform. Otherwise you must port them for your platform specific peripherals.
  - Update platform/nspe/Makefile appropriately for platform/nspe/ code compilation. This make file is invoked as part of test suite build tool(./setup.sh) step and it creates <build_dir>/BUILD/platform/pal_nspe.a archive.
  - The code available in platform/spe/ is part of driver partition and it must be compiled by your partition build tool by processing the platform/manifests/common/driver_partition_psa.json manifest file. See "Compiling compliance partition sources" section for more detail.

## PAL API list
  These functions will require implementation for the target platform. <br />

- Following are the list of PAL APIs used in NSPE: <br />

| No | Prototype                                                 | Description                                          | Parameters                          |
|----|-----------------------------------------------------------|------------------------------------------------------|-------------------------------------|
| 01 | int pal_spi_read(addr_t addr, uint8_t *data, uint32_t len);| This function will read peripherals using SPI commands| addr : address of the peripheral<br/>data : read buffer<br/>len  : length of the read buffer in bytes<br/>|
| 02 | void *pal_target_get_cfg_start(void);                     | provides the database source location.               | void <br/>                          |
| 03 | uint32_t pal_crypto_function(int type, va_list valist);   | This API will call the requested crypto function     | type    : function code<br/>valist  : variable argument list<br/>        |

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
| 08 | int  pal_nvmem_read(addr_t base, uint32_t offset, void *buffer, int size);        | Reads 'size' bytes from non-volatile memory at a given                            | base      : Base address of NV MEM<br/>offset    : Offset<br/>buffer    : Pointer to source address<br/>size      : Number of bytes<br/>                  |

## Compiling compliance partition sources

- Your build tool must conform to manifest rules specified in PSA FF specification. Refer PSA FF specification for more details.
- The compliance required partition manifest files are available in platform/manifests/common and platform/manifests/ipc directory. Use these paths to search and process compliance provided partition manifest files using your build tool.
- Compliance partition source code requires your build tool to gererate following header files when your process the manifest files and those must be available when compiling partition source code: <br />
    - A private header file with \<manifestfilename.h\> naming convention per manifest containing macros that maps names to signals<br />
    - A global header file with \<psa_sid.h\> as name that contains SIDs macros of all manifests<br />
- Your build tool must provide the implementation of PSA defined psa_client.h and psa_service.h header files to partition sources.
- Compliance partition source code includes header files with path relative to psa-ff directory. Therefore, when you compile the sources, your build tool must supply the full path of psa-ff directory to the compiler header file seach path.
- You must integrate compliance partition code with your software stack containing SPM so that partition code get access to PSA defined client and RoT service APIs.


## License
Arm PSA FF Architecture test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2018, Arm Limited and Contributors. All rights reserved.*
