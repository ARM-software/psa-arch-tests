/** @file
 * Copyright (c) 2019-2025, Arm Limited or its affiliates. All rights reserved.
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

#include "pal_crypto_config.h"
#include "pal_attestation_config.h"
#include "pal_storage_config.h"

/*==========================  PLATFORM CONFIGURATIONS START  ==========================*/

// UART device info
#define UART_NUM                               1
#define UART_0_BASE                            0    // Unused value

// Watchdog device info
#define WATCHDOG_NUM                           1

#define WATCHDOG_0_BASE                        0x40018000   // Unused value
#define WATCHDOG_0_NUM_OF_TICK_PER_MICRO_SEC   1
// The same value should be used for all timeout durations, as the nRF91
// WDT cannot be reconfigured once it has been started.
#define WATCHDOG_0_TIMEOUT_IN_MICRO_SEC_LOW    500000000    // 500 secs
#define WATCHDOG_0_TIMEOUT_IN_MICRO_SEC_MEDIUM 500000000
#define WATCHDOG_0_TIMEOUT_IN_MICRO_SEC_HIGH   500000000
#define WATCHDOG_0_TIMEOUT_IN_MICRO_SEC_CRYPTO 500000000

// Range of 1KB Non-volatile memory to preserve data over reset. Ex, NVRAM and FLASH
#define NVMEM_NUM                              1

#define NVMEM_0_START                          0    // Unused value
#define NVMEM_0_END                            0x3ff

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
#define PLATFORM_OVERRIDE_ATTEST_PK

/* Enable custom printing for Non-secure side */
#define BESPOKE_PRINT_NS

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

#ifdef CRYPTO
/* psa/crypto.h: Contains the PSA Crypto API elements */
#include "psa/crypto.h"
#endif

#if defined(INTERNAL_TRUSTED_STORAGE) || defined(STORAGE)
/* psa/internal_trusted_storage.h: Contains the PSA ITS API elements */
#include "psa/internal_trusted_storage.h"
#endif

#if defined(PROTECTED_STORAGE) || defined(STORAGE)
/* psa/protected_storage.h: Contains the PSA PS API elements */
#include "psa/protected_storage.h"
#endif

#ifdef INITIAL_ATTESTATION
/* psa/initial_attestation.h: Contains the PSA Initial Attestation API elements */
#include "psa/initial_attestation.h"
#endif

extern int tfm_log_printf(const char *, ...);
extern int32_t tfm_platform_system_reset(void);

/* Get the address of a free, word-aligned, 1K memory area. */
extern uint32_t pal_nvmem_get_addr(void);

#endif /* _PAL_CONFIG_H_ */
