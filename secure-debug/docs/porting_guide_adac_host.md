
# Porting Guide: ADAC Test Suite
-----------------------------------------------------

## Introduction
The ADAC test suite contains the *Platform Abstraction Layer* (PAL) which abstracts platform-specific information from the tests. You must implement and port the PAL interface functions to your host platform.

This document provides the porting steps and the list of PAL APIs.

## Porting steps

### Adding a new target

  1. Create a new directory in **platform/hosts/<platform_name>**. For reference, see the existing platform **emulation** directory.
  2. Execute `cp -rf platform/hosts/emulation/ platform/hosts/<platform_name>/`. Create the link layer directory for each link layer you want to support for your platform.
  3. Update **platform/<platform_name>/CMakeLists.txt** appropriately to select the correct instances of PAL files for compilation.
  4. Refer to the **List of PAL APIs** section to view the list of PAL APIs that must be ported for your target platform. These API definitions are available in **val/include/pal_interfaces.h**. These APIs are written for the virtual target **emulation**. You can reuse the code if it works for your platform. Otherwise, you must port them for your platform-specific peripherals.
  5. The main.c requires path to the key and certificate chain that point to the host credentials. This must be provided as command-line arguments. You can provide debug link layer specific parameters as additional command-line arguments.

## List of PAL APIs
Since the test suite is agnostic to various system platforms, you must port the following PAL APIs before building the tests. Implement these functions for your host platform. <br/>

| No | Prototype                                             | Description                                                                 | Parameters                                                              |
|----|-------------------------------------------------------|-----------------------------------------------------------------------------|-------------------------------------------------------------------------|
| 01 | int pal_print(char *str, int32_t data);               | Parses the input string and writes bytes into the message interface         | str: Input String<br/>data: Value for format specifier<br/>             |
| 02 | int pal_system_reset(void)                            | Resets the system and disconnects from device                               | None                                                                    |
| 03 | int pal_msg_interface_init(void *ctx)                 | Initializes the message interface for debugging information                 | Handle for the communication interface                                  |
| 04 | int pal_msg_interface_free(void *ctx)                 | Releases the message interface                                              | Handle for the communication interface                                  |
| 05 | int pal_message_send(uint8_t buffer[], size_t size)   | Sends the request data on the debug link interface to the device            | buffer_ptr for payload, transfer size in bytes                          |
| 06 | int pal_message_receive(uint8_t buffer[], size_t size)| Reads the response data from the device                                     | buffer_ptr for payload, transfer size in bytes                          |

## License
Arm PSA test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2022 Arm Limited and Contributors. All rights reserved.*
