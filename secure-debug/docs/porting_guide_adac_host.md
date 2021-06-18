
# Porting Guide: ADAC Test Suite
-----------------------------------------------------

## Introduction
The ADAC test suite contains the *Platform Abstraction Layer* (PAL) which abstracts platform-specific information from the tests. You must implement and port the PAL interface functions to your host platform.

This document provides the porting steps and the list of PAL APIs.

## Porting steps

### Adding a new target

  1. Create a new directory in **platform/<platform_name>**. For reference, see the existing platform **native** directory.
  2. Execute `cp -rf platform/native/ platform/<platform_name>/`.
  3. Update **platform/<platform_name>/CMakeLists.txt** appropriately to select the correct instances of PAL files for compilation.
  4. Refer to the **List of PAL APIs** section to view the list of PAL APIs that must be ported for your target platform. These API definitions are available in **val/include/pal_interfaces.h**. These APIs are written for native target platform. You can reuse the code if it works for your platform. Otherwise, you must port them for your platform-specific peripherals.
  5. The main.c requires path to the key and certificate chain that point to the host credentials. This must be provided as command-line arguments. You can provide debug link layer specific parameters as additional command-line arguments.
  6. If you want to use a pre-built debug communication interface, create a new directory under the depends directory and list your shared libraries within it. A placeholder for RDDI libraries is provided. You can then link these libraries to your platform.

## List of PAL APIs
Since the test suite is agnostic to various system platforms, you must port the following PAL APIs before building the tests. Implement these functions for your host platform. <br/>

| No | Prototype                                             | Description                                                                 | Parameters                                                              |
|----|-------------------------------------------------------|-----------------------------------------------------------------------------|-------------------------------------------------------------------------|
| 01 | int pal_print(char *str, int32_t data);               | Parses the input string and writes bytes into the message interface         | str      : Input String<br/>data     : Value for format specifier<br/>  |
| 02 | int request_packet_send(request_packet_t *packet);    | Writes the ADAC command frame into the communication buffer for transmit    | request packet<br/>                                                     |
| 03 | response_packet_t *response_packet_receive();         | Reads the ADAC command response from the communication buffer               | None<br/>                                                               |
| 04 | int pal_system_reset(void)                            | Resets the system and disconnects from device                               | None                                                                    |

## License
Arm PSA test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2021 Arm Limited and Contributors. All rights reserved.*
