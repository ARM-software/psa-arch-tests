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

#include "val_pe.h"
#include "val_test.h"
#include "val_infra.h"
#include "val_target.h"
#include "val_interrupt.h"
#include "val_test_common.h"
#include "pal_interfaces.h"

/* globals */
print_verbosity_t    g_print_level = PRINT_TEST;
tbsa_status_buffer_t *g_status_buffer = NULL;
addr_t               g_test_binary_src_addr;
uint32_t             g_test_binary_in_ram;

/* externs*/
extern tbsa_isr_vector      g_tbsa_s_isr_vector;
extern tbsa_isr_vector      g_tbsa_ns_isr_vector;
extern const tbsa_val_api_t tbsa_val_ns_api;

/**
    @brief    - Initialise test status buffer
    @return   - tbsa_status_t
**/

static
tbsa_status_t
val_status_buffer_init(void)
{
    tbsa_status_buffer_t *psbuf = g_status_buffer;
    int i;

    for(i=0; i < TBSA_STATUS_BUF_SIZE;)
    {
        psbuf->state   = 0x0;
        psbuf->status  = (uint8_t)TBSA_STATUS_INVALID;
        i             += sizeof(tbsa_status_buffer_t);
        psbuf++;
    }
    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Print module
    @param    - verbosity: Print verbosity level
              - string   : Input string
              - data     : Value for format specifier
    @return   - tbsa_status_t
**/
tbsa_status_t
val_print(print_verbosity_t verbosity, char *string, uint32_t data)
{
    if(verbosity >= g_print_level) {
        pal_print_raw(string, data);
    }
    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Returns the base address of target configuration block database.
    @param    - blob     : Populates the base address
    @return   - tbsa_status_t
**/
tbsa_status_t
val_target_cfg_get_next(void **blob)
{
    tbsa_status_t status = TBSA_STATUS_SUCCESS;
    target_cfg_hdr_t *hdr;
    uint32_t size;

    if (*blob == NULL) {
        *blob = pal_target_get_cfg_start();
        if (blob == NULL) {
            return TBSA_STATUS_NOT_FOUND;
        }
        hdr = *blob;

        /* Sanity check signature and version here */
       if ((hdr->version != 1) || (hdr->size == 0)) {
            val_print(PRINT_ERROR, "Target config database Error. \n", 0);
            return status;
        }
        hdr++;
        *blob = hdr;  // skip the header. start with the first record.
        return status;
    }

    size = (((cfg_type_t *)*blob)->size) & 0xFFFFFF;
    if (size) {
        *blob = (void *)((uint8_t *)*blob + size);
        return TBSA_STATUS_SUCCESS;
    }
    return TBSA_STATUS_ERROR;
}

/**
    @brief    - This function checks for the given configuration ID with the block in
                target configuration database.
    @param    - cfg_id   : Configuration ID of a block
              - data     : Returns block base address
              - size     : Block size
    @return   - tbsa_status_t
**/
tbsa_status_t
val_target_get_cfg_blob(cfg_id_t cfg_id, uint8_t **data, uint32_t *size)
{
    tbsa_status_t status;
    void *config_blob = NULL;

    val_print(PRINT_DEBUG, "Input id is %x \n", cfg_id);
    do {

        status = val_target_cfg_get_next(&config_blob);

        if (TBSA_ERROR(status)) {
            break;
        }

        if (((cfg_type_t *)config_blob)->cfg_id == cfg_id) {
            *data  = (uint8_t *)config_blob;
            status = TBSA_STATUS_SUCCESS;
            break;
        } else if (((((cfg_type_t *)config_blob)->cfg_id & TBSA_TEST_MAJOR_GROUP_MASK) == (cfg_id & TBSA_TEST_MAJOR_GROUP_MASK)) && \
                   !(((cfg_type_t *)config_blob)->cfg_id & (TBSA_TEST_MINOR_GROUP_MASK | TBSA_TEST_CFG_INSTANCE_MASK))) {
                config_blob = (void *)((uint8_t *)config_blob + sizeof(memory_hdr_t));
                if (((cfg_type_t *)config_blob)->cfg_id == cfg_id) {
                    *data  = (uint8_t *)config_blob;
                    status = TBSA_STATUS_SUCCESS;
                    break;
                }
        } else if (((cfg_type_t *)config_blob)->cfg_id == TBSA_TEST_INVALID_CFG_ID) {
                status = TBSA_STATUS_NOT_FOUND;
                break;
        }
    } while(1);

    return status;
}

/**

  @brief  - This function returns the data associated with a given
           config ID.
  @param  -  size  : if the input size is less than the data size to
           returned, the size is updated with the actual data size and
           error is returned.
  @return - data contains the information of type specific to a
            config id.
**/
tbsa_status_t
val_target_get_config(cfg_id_t cfg_id, uint8_t **data, uint32_t *size)
{
    tbsa_status_t status;

    if ((cfg_id < TARGET_MIN_CFG_ID) || (cfg_id > TARGET_MAX_CFG_ID)) {
        val_print(PRINT_ERROR, "Invalid Target data config ID = %x \n", cfg_id);
        return TBSA_STATUS_INVALID_ARGS;
    }
    status = val_target_get_cfg_blob(cfg_id, data, size);

    if (TBSA_ERROR(status)) {
        val_print(PRINT_ERROR, "\n\tGet Config failed with status = %x", status);
        val_print(PRINT_ERROR, " for cfg_id = %x", cfg_id);
        return status;
    }
    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Initialise VAL memory blocks
**/
void val_memory_init (void)
{
    addr_t *addr;

    pal_mem_get_status_buffer_addr(&addr, TBSA_STATUS_BUF_SIZE);
    if(addr != NULL) {
        g_status_buffer = (tbsa_status_buffer_t *)addr;
        val_status_buffer_init();
    }
}

/**
    @brief    - This function returns the test ID of the last test that was run
    @return   - test_id_t
**/
test_id_t val_nvram_get_last_id (void)
{
    tbsa_status_t  status;
    memory_desc_t  *memory_desc;
    boot_t         boot;
    test_id_t      test_id;

    status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_NVRAM, 0),
                                   (uint8_t **)&memory_desc,
                                   (uint32_t *)sizeof(memory_desc_t));
    if(status != TBSA_STATUS_SUCCESS) {
        val_print(PRINT_ERROR, "\n\tNVRAM not present", 0);
        return TBSA_TEST_INVALID;
    }

    status = val_nvram_read(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
    if(status != TBSA_STATUS_SUCCESS) {
        val_print(PRINT_ERROR, "\n\tNVRAM read error", 0);
        return TBSA_TEST_INVALID;
    }

    if (boot.state != WARM_BOOT_REQUESTED) {
        return TBSA_TEST_INVALID;
    }

    status = val_nvram_read(memory_desc->start, TBSA_NVRAM_OFFSET(NV_TEST), &test_id, sizeof(test_id_t));
    if(status != TBSA_STATUS_SUCCESS) {
        val_print(PRINT_ERROR, "\n\tNVRAM read error", 0);
        return TBSA_TEST_INVALID;
    }

    return test_id;
}

/**
    @brief    - Initialise VAL infrastructure
    @param    - test_id  : Populate test ID
    @return   - tbsa_status_t
**/
tbsa_status_t
val_infra_init(test_id_t *test_id)
{
    tbsa_status_t status;

    val_mem_reg_write(SYST_CSR, 0x0);
    val_mem_reg_write(SYST_CSR_NS, 0x0);
    val_mem_reg_write(VTOR, (uint32_t)&g_tbsa_s_isr_vector);
    val_mem_reg_write(VTOR_NS, (uint32_t)&g_tbsa_ns_isr_vector);

    val_memory_init();

    pal_spi_init();

    status = val_get_test_binary_info(&g_test_binary_src_addr, &g_test_binary_in_ram);

    if (status != TBSA_STATUS_SUCCESS) {
        return status;
    }

    status = val_uart_init();
    if(status != TBSA_STATUS_SUCCESS) {
        return status;
    }

    *test_id = val_nvram_get_last_id();

    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Exit procedures for VAL infrastructure
    @return   - tbsa_status_t
**/
tbsa_status_t
val_infra_exit(void)
{

   return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Parses input status for a given test and
                outputs appropriate information on the console
    @param    - test_id  : Current test ID
    @return   - tbsa_status_t
**/
tbsa_status_t
val_report_status(test_id_t test_id)
{
    tbsa_status_buffer_t *psbuf = g_status_buffer;
    uint32_t status;

    if (psbuf == NULL) {
        val_print(PRINT_INFO, "No memory for status buffer\n", 0);
        return TBSA_STATUS_ERROR;
    }

    status = val_get_status();

    psbuf += test_id;

    psbuf->state = (status >> TEST_STATE_BIT) & TEST_STATE_MASK;
    psbuf->status = status & TEST_STATUS_MASK;

    switch(psbuf->state)
    {
        case TBSA_TEST_START:
            val_print(PRINT_INFO, "Test\t: 0x%x START\n\n", test_id);
            break;

        case TBSA_TEST_END:
            val_print(PRINT_INFO, "Test\t: 0x%x END\n\n", test_id);
            break;

        case TBSA_TEST_PASS:
            val_print(PRINT_TEST, "\n                                                           PASS", 0);
            break;

        case TBSA_TEST_FAIL:
            val_print(PRINT_TEST, "\n                                                           FAIL", 0);
            break;

        case TBSA_TEST_SKIP:
            val_print(PRINT_TEST, "\n                                                           SKIP", 0);
            break;

        case TBSA_TEST_PENDING:
            val_print(PRINT_TEST, "\n                                                           PEND", 0);
            break;
    }

    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Records the state and status of test
    @param    - status   : Test status
    @return   - tbsa_status_t
**/
tbsa_status_t
val_set_status(uint32_t status)
{
    if (g_status_buffer == NULL) {
        val_print(PRINT_ERROR, "No memory for status buffer\n", 0);
        return TBSA_STATUS_ERROR;
    }

    /* g_status_buf offset 0 is reserved for saving/checking test status temporarily */
    g_status_buffer->state   = ((status >> TEST_STATE_BIT) & TEST_STATE_MASK);
    g_status_buffer->status  = (status & TEST_STATUS_MASK);

    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Returns the state and status of the test
**/
uint32_t
val_get_status(void)
{
    if (g_status_buffer == NULL) {
        val_print(PRINT_ERROR, "No memory for status buffer\n", 0);
        return (TBSA_TEST_FAIL << TEST_STATE_BIT) | TBSA_STATUS_ERROR;
    }

    /* g_status_buf offset 0 is reserved for saving/checking test status temporarily */
    return ((g_status_buffer->state) << TEST_STATE_BIT) | (g_status_buffer->status);
}

/*
    @brief           - This function checks if the input status argument is an error.
                       On error, we print the checkpoint value and set the status.
    @param           - checkpoint      : Test debug checkpoint
                     - tbsa_status_t   : Test status
    @return          - returns the input status back to the program.
*/
tbsa_status_t
val_err_check_set(uint32_t checkpoint, tbsa_status_t status)
{
    if (TBSA_ERROR(status)) {
        val_print(PRINT_ERROR, "\n        Checkpoint %x : ", checkpoint);
        val_print(PRINT_ERROR, "Status = %x \n", status);
        val_set_status(RESULT_FAIL(status));
    } else {
        val_print(PRINT_DEBUG, "\n        Checkpoint %x : ", checkpoint);
    }

    return status;
}

/*
    @brief     - Reads 'size' bytes from NVRAM at a given 'base + offset' into given buffer.
    @param     - base      : Base address of NVRAM
               - offset    : Offset
               - buffer    : Pointer to source address
               - size      : Number of bytes
    @return    - tbsa_status_t
*/
tbsa_status_t val_nvram_read(addr_t base, uint32_t offset, void *buffer, int size)
{
    if(pal_nvram_read(base, offset, buffer, size)) {
        return TBSA_STATUS_SUCCESS;
    } else {
        return TBSA_STATUS_ERROR;
    }
}

/*
    @brief     - Writes 'size' bytes from buffer into NVRAM at a given 'base + offset'.
    @param     - base      : Base address of NVRAM
               - offset    : Offset
               - buffer    : Pointer to source address
               - size      : Number of bytes
    @return    - tbsa_status_t
*/
tbsa_status_t val_nvram_write(addr_t base, uint32_t offset, void *buffer, int size)
{
    if(pal_nvram_write(base, offset, buffer, size)) {
        return TBSA_STATUS_SUCCESS;
    } else {
        return TBSA_STATUS_ERROR;
    }
}

/*
    @brief      - Generates system warm/cold reset.
    @param      - reset_type - Warm or Cold
    @return     - void
*/
void val_system_reset(system_reset_t reset_type)
{
    switch(reset_type)
    {
        case WARM_RESET:
            pal_system_warm_reset();
            break;

        case COLD_RESET:
            pal_system_cold_reset();
            break;

        default:
            return;
    }
}

/*
    @brief    - Reports the last reset type.
    @return   - system_reset_t
*/
system_reset_t val_system_reset_type(void)
{
    if(pal_is_cold_reset()) {
        return COLD_RESET;
    } else if(pal_is_warm_reset()){
        return WARM_RESET;
    } else {
        return UNKNOWN_RESET;
    }
}

/*
    @brief    - Reads the pre-defined component name against given test_id
    @param    - test_id  : Current Test ID
    @return   - Component name
*/
char *val_get_comp_name(test_id_t test_id)
{
    switch(TBSA_GET_COMP_NUM(test_id))
    {
        case TBSA_BASE_BASE:
            return TBSA_0_STR;
        case TBSA_TRUSTED_BOOT_BASE:
            return TBSA_1_STR;
        case TBSA_CRYPTO_BASE:
            return TBSA_2_STR;
        case TBSA_DEBUG_BASE:
            return TBSA_3_STR;
        case TBSA_DRAM_BASE:
            return TBSA_4_STR;
        case TBSA_EIP_BASE:
            return TBSA_5_STR;
        case TBSA_ENTROPY_SOURCE_BASE:
            return TBSA_6_STR;
        case TBSA_FUSES_BASE:
            return TBSA_7_STR;
        case TBSA_INFRASTRUCTURE_BASE:
            return TBSA_8_STR;
        case TBSA_INTERRUPTS_BASE:
            return TBSA_9_STR;
        case TBSA_SECURE_RAM_BASE:
            return TBSA_10_STR;
        case TBSA_PERIPHERALS_BASE:
            return TBSA_11_STR;
        case TBSA_SAMPLE_BASE:
            return TBSA_12_STR;
        case TBSA_TRUSTED_TIMERS_BASE:
            return TBSA_13_STR;
        case TBSA_VERSION_COUNTERS_BASE:
            return TBSA_14_STR;
        case TBSA_VOLATALITY_BASE:
            return TBSA_15_STR;
        default:
            return "No Component";

    }
}

/*
    @brief    - Reads the target configuration database to populate the
                tbsa_test_combined.bin source address and its physical
    @param    - test_binary_src_addr : physical location of tbsa_test_combined.bin file
                test_binary_in_ram   : 1 - if present in RAM, 0 - otherwise
                location(RAM or external storage).
    @return   - Component name
*/
tbsa_status_t val_get_test_binary_info(addr_t *test_binary_src_addr, uint32_t *test_binary_in_ram)
{
    tbsa_status_t        status;
    miscellaneous_desc_t *misc_desc;

    status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MISCELLANEOUS, MISCELLANEOUS_DUT, 0),
                                   (uint8_t **)&misc_desc,
                                   (uint32_t *)sizeof(miscellaneous_desc_t));

    if (status != TBSA_STATUS_SUCCESS) {
        *test_binary_src_addr = 0x0;
        *test_binary_in_ram   = 0x0;
        return status;
    }

    *test_binary_src_addr = misc_desc->test_binary_src_addr;
    *test_binary_in_ram   = misc_desc->test_binary_in_ram;

    return status;
}

/*
     @brief    - VAL facilitating memcpy support for tests
     @param    - dst  : destination address
                 src  : source address
                 size : number of bytes to copy
     @return   - void
*/
void val_memcpy(void *dst, void *src, uint32_t size)
{
    memcpy(dst, src, size);
}

/*
     @brief    - VAL facilitating execution of piece of trusted code at a given address
     @param    - addr : Trusted addres
     @return   - Trusted code return value
*/
__attribute__((naked))
uint32_t val_execute_in_trusted_mode(addr_t address)
{
    __asm__("push {lr, r3}\n\t"
            "mov  r3, r0\n\t"
            "blx  r3\n\t"
            "pop  {pc, r3}\n\t");
}
