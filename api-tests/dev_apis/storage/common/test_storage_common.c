/** @file
 * Copyright (c) 2024, Arm Limited or its affiliates. All rights reserved.
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

#include "test_storage_common.h"

/* Storage APIs Buffer comparison common function */
int32_t storage_buffer_comparison(uint8_t *buffer_to_compare, uint32_t test_buff_size)
{
    uint32_t j;

    for (j = 0; j < test_buff_size; j++)
    {
        if (buffer_to_compare[j] != 0x00)
        {
            return VAL_STATUS_ERROR;
        }
    }

    return VAL_STATUS_SUCCESS;
}
