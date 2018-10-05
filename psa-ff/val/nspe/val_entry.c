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

#include "val_entry.h"
#include "val_framework.h"
#include "val_peripherals.h"
#include "val_dispatcher.h"

/**
    @brief    - PSA C main function, does VAL init and calls test dispatcher
    @param    - None
    @return   - void
**/
void val_entry(void)
{
    test_id_t       test_id;

    if (VAL_ERROR(val_target_init()))
    {
        goto exit;
    }

    if (VAL_ERROR(val_uart_init()))
    {
        goto exit;
    }

    if (VAL_ERROR(val_get_last_run_test_id(&test_id)))
    {
        goto exit;
    }

    /* Compliance header print */
    if (test_id == VAL_INVALID_TEST_ID)
    {
        val_print(PRINT_ALWAYS, "\n***** PSA Compliance Suite - Version %d.", PSA_ACS_MAJOR_VER);
        val_print(PRINT_ALWAYS, "%d *****\n", PSA_ACS_MINOR_VER);
    }

    /* Call dispatcher routine*/
    val_dispatcher(test_id);

exit:
    val_print(PRINT_ALWAYS, "\n\nEntering standby\n", 0);
    while(1)
    {
        asm volatile("WFI");
    }
}
