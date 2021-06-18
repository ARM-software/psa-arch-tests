/** @file
 * Copyright (c) 2021, Arm Limited or its affiliates. All rights reserved.
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
#ifndef _PAL_COMMON_H_
#define _PAL_COMMON_H_
#include <platform.h>
#include <stddef.h>
/**
 *   @brief    - Initialise the host-side channel for communication with device.
 *   @param    - ctx            Hook for platform-specific descriptor
 *               buffer         Pointer to memory used for TX/RX
 *               buffer_size    Size of allocated memory for TX/RX
 *   @return   - SUCCESS/FAILURE
**/
int msg_interface_init(void *ctx, uint8_t buffer[], size_t size);

/**
 *   @brief    - Release the host-side channel for communication with device.
 *   @param    - ctx            Hook for platform-specific descriptor
 *   @return   - SUCCESS/FAILURE
**/
int msg_interface_free(void *ctx);

#endif
