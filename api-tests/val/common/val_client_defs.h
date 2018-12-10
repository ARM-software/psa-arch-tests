/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
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

#ifndef _VAL_CLIENT_H_
#define _VAL_CLIENT_H_

/****************** PSA Client API *****************/

/* Note - This header file containts the declaration of PSA defined client API elements.
 * Ideally, These elements must be defined in a header file <psa_client.h> by SPM implemented
 * library and provided to clients operation in NSPE and SPE as per the specification.
 *  If this is available in the platform, the elements declared as part of this
 *  file can be overwritten by passing --include <path_to_psa_client_h> to setup.sh script.
 */

#if PSA_IPC_IMPLEMENTED
/* psa/client.h: Contains the Client API elements. Accessible to all applications */
#include "psa/client.h"

/* psa_manifest/sid.h:  Macro definitions derived from manifest files that map from RoT Service
 * names to Service IDs (SIDs).
 * Partition manifest parse build tool must provide the implementation of this file.
*/
#include "psa_manifest/sid.h"

#else

#include "pal_sid.h"
#include "val.h"
#define PSA_FRAMEWORK_VERSION       (0x0100)
#define PSA_VERSION_NONE            (0)
#define PSA_SUCCESS                 (0)
#define PSA_CONNECTION_REFUSED      (INT32_MIN + 1)
#define PSA_CONNECTION_BUSY         (INT32_MIN + 2)
#define PSA_DROP_CONNECTION         (INT32_MIN)
#define PSA_NULL_HANDLE             ((psa_handle_t)0)

typedef int32_t psa_status_t;
typedef int32_t psa_handle_t;

typedef struct psa_invec {
    const void *base;
    size_t len;
} psa_invec;

typedef struct psa_outvec {
    void *base;
    size_t len;
} psa_outvec;

#endif /* #if PSA_IPC_IMPLEMENTED */

#define INVALID_SID                     0x0000FA20
#endif /* _VAL_CLIENT_H_ */
