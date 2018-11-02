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

#include "val/spe/val_driver_service_apis.h"

void driver_main(void)
{
    psa_signal_t    signals = 0;
    psa_msg_t       msg = {0};
    uint32_t        data = 0;
    uart_fn_type_t  uart_fn;
    nvmem_param_t   nvmem_param;
    char            string[256] = {0};
    uint8_t         buffer[256] = {0};
    wd_param_t      wd_param;
    addr_t          uart_base;

    while (1)
    {
        val_status_t fn_status = VAL_STATUS_SUCCESS;
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);

        /* Service Print functionality */
        if (signals & DRIVER_UART_SIG)
        {
            psa_get(DRIVER_UART_SIG, &msg);
            switch (msg.type)
            {
                case PSA_IPC_CONNECT:
                    psa_reply(msg.handle, PSA_SUCCESS);
                    break;
                case PSA_IPC_CALL:
                    if ((msg.in_size[0] > sizeof(uart_fn))
                        && (msg.in_size[1] > sizeof(string))
                        && (msg.in_size[2] > sizeof(data)))
                    {
                        /* buffer overflow */
                        psa_reply(msg.handle, VAL_STATUS_ERROR);
                        break;
                    }
                    else
                    {
                        psa_read(msg.handle, 0, &uart_fn, msg.in_size[0]);
                    }

                    if (uart_fn == UART_INIT)
                    {
                        /* arg2=uart_base */
                        psa_read(msg.handle, 1, &uart_base, msg.in_size[1]);
                        fn_status = val_uart_init_sf(uart_base);
                    }
                    else
                    {
                        /* arg2=string, arg3=DATA */
                        psa_read(msg.handle, 1, &string, msg.in_size[1]);
                        psa_read(msg.handle, 2, &data, msg.in_size[2]);
                        fn_status = val_print_sf(string, data);
                    }
                    if (VAL_ERROR(fn_status))
                    {
                        psa_reply(msg.handle, VAL_STATUS_ERROR);
                    }
                    else
                    {
                        psa_reply(msg.handle, PSA_SUCCESS);
                    }
                    break;
                case PSA_IPC_DISCONNECT:
                    psa_reply(msg.handle, PSA_SUCCESS);
                    break;
            }
        }

        /* Service Watchdog functionality */
        if (signals & DRIVER_WATCHDOG_SIG)
        {
            psa_get(DRIVER_WATCHDOG_SIG, &msg);
            switch (msg.type)
            {
                case PSA_IPC_CALL:
                    if (msg.in_size[0] > sizeof(wd_param))
                    {
                        /* buffer overflow */
                        psa_reply(msg.handle, VAL_STATUS_ERROR);
                        val_print_sf("msg.in_size[0] buffer overflow\n", 0);
                        break;
                    }
                    else
                    {
                        psa_read(msg.handle, 0, &wd_param, msg.in_size[0]);
                    }

                    if (wd_param.wd_fn_type == WD_INIT_SEQ)
                    {
                        fn_status = val_wd_timer_init_sf(wd_param.wd_base_addr,
                                                         wd_param.wd_time_us,
                                                         wd_param.wd_timer_tick_us);
                    }
                    else if (wd_param.wd_fn_type == WD_ENABLE_SEQ)
                    {
                        fn_status = val_wd_timer_enable_sf(wd_param.wd_base_addr);
                    }
                    else if (wd_param.wd_fn_type == WD_DISABLE_SEQ)
                    {
                        fn_status = val_wd_timer_disable_sf(wd_param.wd_base_addr);
                    }
                    else if (wd_param.wd_fn_type == WD_STATUS_SEQ)
                    {
                        fn_status = val_is_wd_timer_enabled_sf(wd_param.wd_base_addr);
                    }
                    if (fn_status == VAL_STATUS_SUCCESS)
                    {
                        psa_reply(msg.handle, PSA_SUCCESS);
                    }
                    else
                    {
                        psa_reply(msg.handle, VAL_STATUS_ERROR);
                    }
                    break;
                case PSA_IPC_CONNECT:
                case PSA_IPC_DISCONNECT:
                    psa_reply(msg.handle, PSA_SUCCESS);
                    break;
            }
        }

         /* Service NVMEM functionality */
        if (signals & DRIVER_NVMEM_SIG)
        {
            psa_get(DRIVER_NVMEM_SIG, &msg);
            switch (msg.type)
            {
                case PSA_IPC_CALL:
                    if (msg.in_size[0] > sizeof(nvmem_param))
                    {
                        /* buffer overflow */
                        psa_reply(msg.handle, VAL_STATUS_ERROR);
                        val_print_sf("msg.in_size[0] buffer overflow\n", 0);
                        break;
                    }
                    else
                    {
                        psa_read(msg.handle, 0, &nvmem_param, msg.in_size[0]);
                    }

                    if (nvmem_param.nvmem_fn_type == NVMEM_READ)
                    {
                        fn_status = val_nvmem_read_sf(nvmem_param.base,
                                                      nvmem_param.offset,
                                                      buffer,
                                                      nvmem_param.size);
                        psa_write(msg.handle, 0, (const void*) buffer, msg.out_size[0]);
                    }
                    else
                    {
                        if (msg.in_size[1] > sizeof(buffer))
                        {
                            /* buffer overflow */
                            fn_status = VAL_STATUS_ERROR;
                            val_print_sf("msg.in_size[1] buffer overflow\n", 0);
                        }
                        else
                        {
                            psa_read(msg.handle, 1, (void*) buffer, msg.in_size[1]);
                            fn_status = val_nvmem_write_sf(nvmem_param.base,
                                                           nvmem_param.offset,
                                                           buffer,
                                                           nvmem_param.size);
                        }
                    }
                    if (fn_status == VAL_STATUS_SUCCESS)
                    {
                        psa_reply(msg.handle, PSA_SUCCESS);
                    }
                    else
                    {
                        psa_reply(msg.handle, VAL_STATUS_ERROR);
                    }
                    break;
                case PSA_IPC_CONNECT:
                case PSA_IPC_DISCONNECT:
                    psa_reply(msg.handle, PSA_SUCCESS);
                    break;
            }
        }

        if (signals == 0)
        {
            val_print_sf("psa_wait returned zero for PSA_BLOCK. Entering into infinite loop\n",
                         0);
            while (1);
        }
    }
}
