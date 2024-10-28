/** @file
 * Copyright (c) 2024, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/
#include "test_crypto_common.h"

typedef struct {
    char                    test_desc[75];
    psa_algorithm_t         alg;
} test_data;

static const test_data check1[] = {
#ifdef ARCH_TEST_SPAKE2P
#ifdef ARCH_TEST_SHA256
#ifdef ARCH_TEST_SPAKE2P_HMAC
{
  .test_desc       = "SPAKE2P Test - SPAKE2P HMAC - SHA256 \n",
  .alg             = PSA_ALG_SPAKE2P_HMAC(PSA_ALG_SHA_256)
},
#endif
#ifdef ARCH_TEST_SPAKE2P_CMAC
{
  .test_desc       = "SPAKE2P Test - SPAKE2P CMAC - SHA256 \n",
  .alg             = PSA_ALG_SPAKE2P_CMAC(PSA_ALG_SHA_256)
},
#endif
#endif
#ifdef ARCH_TEST_SPAKE2P_MATTER
{
  .test_desc       = "SPAKE2P Test - SPAKE2P MATTER \n",
  .alg             = PSA_ALG_SPAKE2P_MATTER
},
#endif
#endif
};
