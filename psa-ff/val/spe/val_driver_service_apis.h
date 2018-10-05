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

#ifndef _VAL_DRIVER_PARTITION_APIS_H_
#define _VAL_DRIVER_PARTITION_APIS_H_

#include "val/common/val.h"
#include "val/common/val_client_defs.h"
#include "val_service_defs.h"
#include "pal_interfaces_s.h"

/* <manifestfilename.h> Manifest definitions. Only accessible to Secure Partition.
 * The file name is based on the name of the Secure Partitions manifest file.
 * The name must not collide with other header files.
 * Compliance tests expect the below manifest output files implementation from build tool.
 */
#include "driver_partition_psa.h"

val_status_t val_uart_init_sf(addr_t uart_base_addr, print_verbosity_t print_level);
val_status_t val_print_sf(print_verbosity_t verbosity, char *string, uint32_t data);
val_status_t val_wd_timer_init_sf(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us);
val_status_t val_wd_timer_enable_sf(addr_t base_addr);
val_status_t val_wd_timer_disable_sf(addr_t base_addr);
val_status_t val_is_wd_timer_enabled_sf(addr_t base_addr);
val_status_t val_nvmem_read_sf(addr_t base, uint32_t offset, void *buffer, int size);
val_status_t val_nvmem_write_sf(addr_t base, uint32_t offset, void *buffer, int size);
#endif
