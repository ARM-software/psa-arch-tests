/** @file
 * Copyright (c) 2026, Arm Limited or its affiliates. All rights reserved.
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

#ifndef VAL_LOG_H
#define VAL_LOG_H

#include <stdint.h>

/* Verbosity enums, Lower the value, higher the verbosity */
typedef enum {
    INFO    = 1,
    DBG     = 2,
    TEST    = 3,
    WARN    = 4,
    ERROR   = 5,
    ALWAYS  = 9
} print_verbosity_t;

/**
 *   @brief    - This function prints the given string and data onto the uart
 *   @param    - verbosity  : Print Verbosity level
 *   @param    - msg        : Input String
 *   @param    - ...        : ellipses for variadic args
 *   @return   - SUCCESS((Any positive number for character written)/FAILURE(0))
**/
uint32_t val_printf(print_verbosity_t verbosity, const char *msg, ...);

#endif /* VAL_LOG_H */
