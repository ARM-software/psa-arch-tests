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

#include <psa_adac.h>
#include <psa_adac_debug.h>
#include "unix_msg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(_MSC_VER)
    #include <unistd.h>
#else // !defined(_MSC_VER)
    #include <io.h>

    // Disable warning about POSIX function names.
    #pragma warning(disable : 4996)
#endif // !defined(_MSC_VER)

int static_buffer_msg_init(uint8_t *buffer, size_t size);
int static_buffer_msg_release(void);

#define PAL_STATUS_UNSUPPORTED_FUNC      0xFF

typedef enum {
    PAL_STATUS_SUCCESS = 0x0,
    PAL_STATUS_ERROR   = 0x80
} pal_status_t;

enum {
    BUFFER_UNINITIALIZED = 0,
    BUFFER_EMPTY,
    BUFFER_REQUEST,
    BUFFER_RESPONSE
};
static size_t static_buffer_size;
static uint8_t *static_buffer_pointer;
static uint8_t static_buffer_status = BUFFER_UNINITIALIZED;
static int _fd;


int pal_print(const char *str, int32_t data)
{
    if (printf(str, data) < 0)
    {
        return PAL_STATUS_ERROR;
    }
    return PAL_STATUS_SUCCESS;
}

void pal_terminate_simulation(void)
{
    ;
}

int pal_system_reset(void)
{
    return PAL_STATUS_UNSUPPORTED_FUNC;
}

int static_buffer_msg_init(uint8_t *buffer, size_t size)
{
    int ret = -1;

    if (static_buffer_status == BUFFER_UNINITIALIZED) {
        static_buffer_size = size;
        static_buffer_pointer = buffer;
        static_buffer_status = BUFFER_EMPTY;
        ret = 0;
    }
    return ret;
}

int static_buffer_msg_release(void)
{
    int ret = -1;

    if (static_buffer_status == BUFFER_EMPTY) {
        static_buffer_size = 0;
        static_buffer_pointer = NULL;
        static_buffer_status = BUFFER_UNINITIALIZED;
        ret = 0;
    }
    return ret;
}

request_packet_t *request_packet_build(uint16_t command, uint8_t *data, size_t data_size)
{
    request_packet_t *request = NULL;

    if ((static_buffer_status == BUFFER_EMPTY) &&
        (data_size <= (static_buffer_size - sizeof(request_packet_t)))) {
        request = (request_packet_t *) static_buffer_pointer;
        request->command = command;
        request->data_count = data_size / 4UL;
        (void) memcpy((void *) request->data, (void *) data, data_size);
        static_buffer_status = BUFFER_REQUEST;
    }
    return request;
}

request_packet_t *request_packet_lock(size_t *max_data_size)
{
    request_packet_t *request = NULL;

    if (static_buffer_status == BUFFER_EMPTY) {
        if (max_data_size != NULL)
            *max_data_size = static_buffer_size - sizeof(response_packet_t);

        request = (request_packet_t *) static_buffer_pointer;
        static_buffer_status = BUFFER_REQUEST;
    }
    return request;
}

int request_packet_release(request_packet_t *packet)
{
    int ret = -1;

    if (static_buffer_status == BUFFER_REQUEST) {
        static_buffer_status = BUFFER_EMPTY;
        ret = 0;
    }
    return ret;
}

response_packet_t *response_packet_lock(size_t *max_data_size)
{
    response_packet_t *response = NULL;

    if (static_buffer_status == BUFFER_EMPTY) {
        if (max_data_size != NULL)
            *max_data_size = static_buffer_size - sizeof(response_packet_t);

        response = (response_packet_t *) static_buffer_pointer;
        static_buffer_status = BUFFER_RESPONSE;
    }
    return response;
}

int response_packet_release(response_packet_t *packet)
{
    int ret = -1;

    if (static_buffer_status == BUFFER_RESPONSE) {
        static_buffer_status = BUFFER_EMPTY;
        ret = 0;
    }
    return ret;
}

int msg_interface_init(void *ctx, uint8_t buffer[], size_t buffer_size)
{
    unix_socket_init();

    if (ctx == NULL)
        return -1;

    _fd = *((int *) ctx);
    return static_buffer_msg_init(buffer, buffer_size);
}

int msg_interface_free(void *ctx)
{
    unix_socket_close(_fd);
    _fd = -1;
    return static_buffer_msg_release();
}

static int message_receive(int fd, uint8_t buffer[], size_t max, size_t *size)
{
    if (nread(fd, buffer, sizeof(request_packet_t)) != sizeof(request_packet_t)) {
        PSA_ADAC_LOG_ERR("transport_unix", "Error receiving message header\n");
        return -1;
    }
    request_packet_t *p = (request_packet_t *) buffer;

    if (4 + p->data_count * 4 > max) {
        PSA_ADAC_LOG_ERR("transport_unix", "Message would overflow buffer (%d > %d)\n",
                                            4 + p->data_count * 4, (int) max);
        return -1;
    }
    if (p->data_count) {
        if (nread(fd, (uint8_t *) p->data, p->data_count * 4) != p->data_count * 4) {
            PSA_ADAC_LOG_ERR("transport_unix", "Error receiving message body\n");
            return -1;
        }
    }
    //PSA_ADAC_LOG_DUMP("msg", "receive", buffer, 4 + p->data_count * 4);
    return 0;
}

static int message_send(int fd, uint8_t buffer[], size_t size)
{
    //PSA_ADAC_LOG_DUMP("msg", "send", buffer, size);
    return nwrite(fd, (uint8_t *) buffer, size) == size ? 0 : -1;
}

int request_packet_send(request_packet_t *packet)
{
    if (packet == NULL)
        return -1;

    size_t size = sizeof(request_packet_t) + 4 * packet->data_count;

    return message_send(_fd, (uint8_t *) packet, size);
}

response_packet_t *response_packet_receive()
{
    size_t max = 0;
    response_packet_t *r = response_packet_lock(&max);

    if (r != NULL) {
        if (message_receive(_fd, (uint8_t *) r, max, NULL) == 0)
            return r;

        PSA_ADAC_LOG_ERR("transport_unix", "Error Receiving Response");
        response_packet_release(r);
    } else {
        PSA_ADAC_LOG_ERR("transport_unix", "Error Locking Response");
    }
    return NULL;
}

void psa_adac_platform_init(void)
{
    ;
}

