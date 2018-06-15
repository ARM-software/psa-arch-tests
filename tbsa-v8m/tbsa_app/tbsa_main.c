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
#include "tbsa.h"

/**
    @brief    - TBSA C main function, zero out BSS, does VAL init
    @param    - None
    @return   - void
**/
void tbsa_main (void)
{
    tbsa_status_t status;
    test_id_t test_id_prev;

    /* zero bss section */
    memset(&__tbsa_bss_start__, 0, TBSA_REGION_SIZE(&__tbsa_bss_start__, &__tbsa_bss_end__));

    status = val_infra_init(&test_id_prev);
    if (TBSA_ERROR(status))
    {
        val_print(PRINT_ERROR, "\nVal Infra Init failed with error = %x ", status);
        goto exit;
    }
    val_print(PRINT_ALWAYS, "\nTBSA Compliance Suite",0);
    val_print(PRINT_ALWAYS, "\nVersion :\t%d.", TBSA_ACS_MAJOR_VER);
    val_print(PRINT_ALWAYS, "%d\n\n", TBSA_ACS_MINOR_VER);


    /* Call the dispatcher routine*/
    tbsa_dispatcher(test_id_prev);

exit:
    val_print(PRINT_ALWAYS, "\n\nEntering standby\n", 0);
    while(1) {
        asm volatile("WFI");
    }
}
