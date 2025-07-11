/** @file
 * Copyright (c) 2023-2025, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 * Copyright 2023 NXP
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

#include "pal_common.h"

__attribute__((weak)) void pal_set_custom_test_list(char *custom_test_list)
{
	(void)custom_test_list;

	return;
}

__attribute__((weak)) bool_t pal_is_test_enabled(test_id_t test_id)
{
	(void)test_id;

	return 1;
}

/**
 *   @brief    - initialize anything relavent to a platform
 *               like any key initialization for crypto operations and etc
 *               this is optional Api to implement
 *   @return   - TRUE/FALSE
**/
__attribute__((weak)) unsigned int pal_platform_init(void)
{
	return (unsigned int)PAL_STATUS_SUCCESS;
}

__attribute__((weak)) int pal_print(uint8_t c)
{
	(void)c;

	return PAL_STATUS_SUCCESS;
}
