/** @file
 * Copyright (c) 2019, Arm Limited or its affiliates. All rights reserved.
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

/*
 * List of macros used by test suite
 */
#if !defined(PSA_IPC_IMPLEMENTED)
#define PSA_IPC_IMPLEMENTED 0
#endif

#if !defined(PSA_CRYPTO_IMPLEMENTED)
#define PSA_CRYPTO_IMPLEMENTED 0
#endif

#if !defined(PSA_INTERNAL_TRUSTED_STORAGE_IMPLEMENTED)
#define PSA_INTERNAL_TRUSTED_STORAGE_IMPLEMENTED 0
#endif

#if !defined(PSA_PROTECTED_STORAGE_IMPLEMENTED)
#define PSA_PROTECTED_STORAGE_IMPLEMENTED 0
#endif

#if !defined(PSA_INITIAL_ATTESTATION_IMPLEMENTED)
#define PSA_INITIAL_ATTESTATION_IMPLEMENTED 0
#endif

#if (PSA_IPC_IMPLEMENTED == 0) && \
    (PSA_CRYPTO_IMPLEMENTED == 0) && \
    (PSA_INTERNAL_TRUSTED_STORAGE_IMPLEMENTED == 0) && \
    (PSA_PROTECTED_STORAGE_IMPLEMENTED == 0) && \
    (PSA_INITIAL_ATTESTATION_IMPLEMENTED == 0)
#error "You must define at least one of these macros to run test suite"
#endif

#if !defined(VERBOSE)
#define VERBOSE 4 /* Print verbosity = ERROR */
#endif

#if (!defined(VAL_NSPE_BUILD) && !defined(SPE_BUILD))
#define VAL_NSPE_BUILD 1
#endif

#if (!defined(NONSECURE_TEST_BUILD) && !defined(SPE_BUILD))
#define NONSECURE_TEST_BUILD 1
#endif

#if !defined(TEST_COMBINE_ARCHIVE)
//#define TEST_COMBINE_ARCHIVE /* Test dispatcher code selection */
#endif

/*
 * Include of PSA defined Header files
 */

#if PSA_IPC_IMPLEMENTED
/* psa/client.h: Contains the PSA Client API elements */
#include "psa/client.h"

/*
 * psa_manifest/sid.h:  Macro definitions derived from manifest files that map from RoT Service
 * names to Service IDs (SIDs). Partition manifest parse build tool must provide the implementation
 * of this file.
*/
#include "psa_manifest/sid.h"
#endif

#if PSA_CRYPTO_IMPLEMENTED
/* psa/crypto.h: Contains the PSA Crypto API elements */
#include "psa/crypto.h"
#endif

#if PSA_INTERNAL_TRUSTED_STORAGE_IMPLEMENTED
/* psa/internal_trusted_storage.h: Contains the PSA ITS API elements */
#include "psa/internal_trusted_storage.h"
#endif

#if PSA_PROTECTED_STORAGE_IMPLEMENTED
/* psa/protected_storage.h: Contains the PSA PS API elements */
#include "psa/protected_storage.h"
#endif

#if PSA_INITIAL_ATTESTATION_IMPLEMENTED
/* psa/initial_attestation.h: Contains the PSA Initial Attestation API elements */
#include "psa/initial_attestation.h"
#endif

#endif /* _PAL_CONFIG_H_ */
