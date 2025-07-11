/** @file
 * Copyright (c) 2019-2025, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Copyright (c) 2021 Nordic Semiconductor ASA.
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

#ifndef _PAL_CONFIG_H_
#define _PAL_CONFIG_H_

/*==========================  PLATFORM CONFIGURATIONS START  ==========================*/

// UART device info
#define UART_NUM                               1
#define UART_0_BASE                            0    // Unused value

// Watchdog device info
#define WATCHDOG_NUM                           1

#define WATCHDOG_0_BASE                        0x50018000   // Secure WDT0
#define WATCHDOG_0_NUM_OF_TICK_PER_MICRO_SEC   0         // Unused
#define WATCHDOG_0_TIMEOUT_IN_MICRO_SEC_LOW    1000000   // 1.0 secs
#define WATCHDOG_0_TIMEOUT_IN_MICRO_SEC_MEDIUM 2000000   // 2 secs
#define WATCHDOG_0_TIMEOUT_IN_MICRO_SEC_HIGH   5000000   // 5 secs
#define WATCHDOG_0_TIMEOUT_IN_MICRO_SEC_CRYPTO 18000000  // 18 secs

// Range of 1KB Non-volatile memory to preserve data over reset. Ex, NVRAM and FLASH
#define NVMEM_NUM                              1

#define NVMEM_0_START                          0    // Unused value
#define NVMEM_0_END                            0x3ff

// ###################################################################
// Following Target configuration parameters are required for IPC tests
// only. Avoid updating them if you are running dev_apis tests.
// ###################################################################

// Assign free memory range for isolation testing. Choose the addresses
// for these memory regions such that it follows below condition:
// nspe_mmio.0.start < server_partition_mmio.0.start < driver_partition_mmio.0.start.

#define NSPE_MMIO_NUM                      1

#define NSPE_MMIO_0_START                  0x2007DF00
#define NSPE_MMIO_0_END                    0x2007E000
#define NSPE_MMIO_0_PERMISSION             TYPE_READ_WRITE

#define SERVER_PARTITION_MMIO_NUM          1

#define SERVER_PARTITION_MMIO_0_START      0x2007E000
#define SERVER_PARTITION_MMIO_0_END        0x2007E100
#define SERVER_PARTITION_MMIO_0_PERMISSION TYPE_READ_WRITE

#define DRIVER_PARTITION_MMIO_NUM          1

#define DRIVER_PARTITION_MMIO_0_START      0x2007E100
#define DRIVER_PARTITION_MMIO_0_END        0x2007E200
#define DRIVER_PARTITION_MMIO_0_PERMISSION TYPE_READ_WRITE

/*==========================  PLATFORM CONFIGURATIONS END  ============================*/


/* Define PSA test suite dependent macros for non-cmake build */
#if !defined(PSA_CMAKE_BUILD)

/* Print verbosity = TEST */
#define VERBOSITY 3

/* NSPE or SPE VAL build? */
#define VAL_NSPE_BUILD

/* NSPE or SPE TEST build? */
#define NONSECURE_TEST_BUILD

/* If not defined, skip watchdog programming */
#define WATCHDOG_AVAILABLE

/* Are Dynamic memory APIs available to secure partition? */
#define SP_HEAP_MEM_SUPP

/* PSA Isolation level supported by platform */
#define PLATFORM_PSA_ISOLATION_LEVEL 3
#endif /* PSA_CMAKE_BUILD */

/* Version of crypto spec used in attestation */
#define CRYPTO_VERSION_BETA3

/* Use hardcoded public key */
//#define PLATFORM_OVERRIDE_ATTEST_PK

/* Enable custom printing for Non-secure side */
#define BESPOKE_PRINT_NS

/* Enable custom printing for Secure side */
#define BESPOKE_PRINT_S

/* UART base address assigned */
#define PLATFORM_UART_BASE UART_0_BASE

/* Watchdog device configurations assigned */
#define PLATFORM_WD_BASE                        WATCHDOG_0_BASE
#define PLATFORM_WD_NUM_OF_TICK_PER_MICRO_SEC   WATCHDOG_0_NUM_OF_TICK_PER_MICRO_SEC
#define PLATFORM_WD_TIMEOUT_IN_MICRO_SEC_LOW    WATCHDOG_0_TIMEOUT_IN_MICRO_SEC_LOW
#define PLATFORM_WD_TIMEOUT_IN_MICRO_SEC_MEDIUM WATCHDOG_0_TIMEOUT_IN_MICRO_SEC_LOW
#define PLATFORM_WD_TIMEOUT_IN_MICRO_SEC_HIGH   WATCHDOG_0_TIMEOUT_IN_MICRO_SEC_LOW
#define PLATFORM_WD_TIMEOUT_IN_MICRO_SEC_CRYPTO WATCHDOG_0_TIMEOUT_IN_MICRO_SEC_CRYPTO

/* Non-volatile memory base address assigned */
#define PLATFORM_NVM_BASE NVMEM_0_START


/* Non-secure partition Memory Mapped I/O base address assigned */
#define PLATFORM_NSPE_MMIO_START NSPE_MMIO_0_START

/* Server partition Memory Mapped I/O base address assigned */
#define PLATFORM_SERVER_PARTITION_MMIO_START SERVER_PARTITION_MMIO_0_START

/* Driver partition Memory Mapped I/O configurations assigned */
#define PLATFORM_DRIVER_PARTITION_MMIO_START DRIVER_PARTITION_MMIO_0_START
#define PLATFORM_DRIVER_PARTITION_MMIO_END   DRIVER_PARTITION_MMIO_0_END

/*
 * Include of PSA defined Header files
 */
#ifdef IPC
/* psa/client.h: Contains the PSA Client API elements */
#include "psa/client.h"

/*
 * psa_manifest/sid.h:  Macro definitions derived from manifest files that map from RoT Service
 * names to Service IDs (SIDs). Partition manifest parse build tool must provide the implementation
 * of this file.
*/
#include "psa_manifest/sid.h"

/*
 * psa_manifest/pid.h: Secure Partition IDs
 * Macro definitions that map from Secure Partition names to Secure Partition IDs.
 * Partition manifest parse build tool must provide the implementation of this file.
*/
#include "psa_manifest/pid.h"
#endif

#endif /* _PAL_CONFIG_H_ */
