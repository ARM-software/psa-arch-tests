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

#ifndef _PAL_SID_H_
#define _PAL_SID_H_

/* SID Constant defined and used by test suite */
#define CLIENT_TEST_DISPATCHER_SID      0x0000FA01
#define SERVER_TEST_DISPATCHER_SID      0x0000FB01
#define SERVER_SECURE_CONNECT_ONLY_SID  0x0000FB02
#define SERVER_STRICT_MINOR_VERSION_SID 0x0000FB03
#define SERVER_UNSPECIFED_MINOR_V_SID   0x0000FB04
#define SERVER_RELAX_MINOR_VERSION_SID  0x0000FB05
#define SERVER_UNEXTERN_SID             0x0000FB06
#define SERVER_CONNECTION_DROP_SID      0x0000FB07
#define DRIVER_UART_SID                 0x0000FC01
#define DRIVER_WATCHDOG_SID             0x0000FC02
#define DRIVER_NVMEM_SID                0x0000FC03
#define DRIVER_TARGET_INIT_SID          0x0000FC04

#endif /* _PAL_SID_H_ */
