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

#ifndef _PAL_INTERFACES_H_
#define _PAL_INTERFACES_H_

#include "val_common.h"

/**
    @brief    - Enable Interrupt
    @param    - intr_num: Interrupt number
    @return   - void
**/
void pal_NVIC_EnableIRQ(uint32_t intr_num);

/**
    @brief    - Disable Interrupt
    @param    - intr_num: Interrupt number
    @return   - void
**/
void pal_NVIC_DisableIRQ(uint32_t intr_num);

/**
    @brief    - Clear Interrupt Target State
    @param    - intr_num: Interrupt number
    @return   - 0  if interrupt is assigned to Secure
                1  if interrupt is assigned to Non Secure
**/
uint32_t pal_NVIC_ClearTargetState(uint32_t intr_num);

/**
    @brief    - Set Interrupt Target Stat
    @param    - intr_num: Interrupt number
    @return   - 0  if interrupt is assigned to Secure
                1  if interrupt is assigned to Non Secure
**/
uint32_t pal_NVIC_SetTargetState(uint32_t intr_num);

/**
    @brief    - Set Interrupt Priority
    @param    - intr_num: Interrupt number
                priority: Priority to set
    @return   - void
**/
void pal_NVIC_SetPriority(uint32_t intr_num, uint32_t priority);

/**
    @brief    - Get Interrupt Priority
    @param    - intr_num: Interrupt number
    @return   - Interrupt Priority
**/
uint32_t pal_NVIC_GetPriority(uint32_t intr_num);

/**
    @brief    - Set Pending Interrupt
    @param    - intr_num: Interrupt number
    @return   - void
**/
void pal_NVIC_SetPendingIRQ(uint32_t intr_num);

/**
    @brief    - Clear Pending Interrupt
    @param    - intr_num: Interrupt number
    @return   - void
**/
void pal_NVIC_ClearPendingIRQ(uint32_t intr_num);

/**
    @brief    - Get Pending Interrupt
    @param    - intr_num: Interrupt number
    @return   - 0  Interrupt status is not pending
                1  Interrupt status is pending
**/
uint32_t pal_NVIC_GetPendingIRQ(uint32_t intr_num);

/**
    @brief    - Get Active Interrupt
    @param    - intr_num: Interrupt number
    @return   - 0  Interrupt status is not active
                1  Interrupt status is active
**/
uint32_t pal_NVIC_GetActive(uint32_t intr_num);

/**
    @brief    - This function will initialise I2C peripheral
    @return   - none
**/
void pal_i2c_init(void);

/**
    @brief    - This function will read peripherals using I2C
    @param    - slv_addr: address of the peripheral
                rd_data : read buffer
                len     : length of the read buffer in bytes
    @return   - error status
**/
int  pal_i2c_read(uint32_t slv_addr, uint8_t *rd_data, uint32_t len);

/**
    @brief    - This function will read peripherals using I2C
    @param    - slv_addr: address of the peripheral
                wr_data : read buffer
                len     : length of the read buffer in bytes
    @return   - error status
**/
int  pal_i2c_write(uint32_t slv_addr, uint8_t *wr_data, uint32_t len);

/**
    @brief    - This function will initialise SPI peripheral
    @return   - none
**/
void pal_spi_init(void);

/**
    @brief    - This function will read peripherals using SPI commands
    @param    - addr : address of the peripheral
                data : read buffer
                len  : length of the read buffer in bytes
    @return   - error status
**/
int  pal_spi_read(uint32_t addr, uint8_t *data, uint32_t len);

/**
    @brief    - This function will write peripherals using SPI commands
    @param    - addr : address of the peripheral
                data : write buffer
                len  : length of the write buffer in bytes
    @return   - error status
**/
int  pal_spi_write(uint32_t addr, uint8_t *data, uint32_t len);

/**
    @brief    - This function initializes the crytographic functions
**/
void pal_crypto_init(addr_t crypto_base_addr);

/**
    @brief    - Initializes an hardware timer
    @param    - base_addr     : Base address of the timer module
              - time_us       : Time in micro seconds
              - timer_tick_us : Number of ticks per micro seconds
    @return   - SUCCESS/FAILURE
**/
int  pal_timer_init       (addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us);

/**
    @brief    - Enables a hardware timer
    @param    - base_addr     : Base address of the timer module
    @return   - SUCCESS/FAILURE
**/
int  pal_timer_enable     (addr_t base_addr);

/**
    @brief    - Disables a hardware timer
    @param    - base_addr     : Base address of the timer module
    @return   - SUCCESS/FAILURE
**/
int  pal_timer_disable    (addr_t base_addr);

/**
    @brief    - Clears the interrupt status of timer
    @param    - base_addr     : Base address of the timer module
    @return   - SUCCESS/FAILURE
**/
int  pal_timer_interrupt_clear (addr_t base_addr);

/**
    @brief           - Initializes an hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
                     - time_us         : Time in micro seconds
                     - timer_tick_us   : Number of ticks per micro second
    @return          - SUCCESS/FAILURE
**/
int  pal_wd_timer_init       (addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us);

/**
    @brief           - Enables a hardware watchdog timer
    @param           - base_addr     : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int  pal_wd_timer_enable     (addr_t base_addr);

/**
    @brief           - Disables a hardware watchdog timer
    @param           - base_addr     : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int  pal_wd_timer_disable    (addr_t base_addr);


/**
    @brief           - Checks whether hardware watchdog timer is enabled
    @param           - base_addr     : Base address of the watchdog module
    @return          - Enabled : 1, Disabled : 0
**/
int  pal_is_wd_timer_enabled (addr_t base_addr);

/**
    @brief    - This function initializes the specified AES context.
                It must be the first API called before using the context.
    @param    -
    @return   -
**/
void pal_crypto_aes_init(void);

/**
    @brief    - This function sets the encryption key.
    @param    - key     : The encryption key
                keysize : The size of data passed in bits. Valid options are:
                          - 128 bits
                          - 192 bits
                          - 256 bits
    @return   -  0 if successful, or ERR_AES_INVALID_KEY_LENGTH/
                 ERR_AES_INVALID_INPUT_LENGTH
**/
int  pal_crypto_aes_setkey_enc(uint8_t *key, uint32_t keysize);

/**
    @brief    - This function sets the decryption key.
    @param    - key     : The decryption key
                keysize : The size of data passed in bits. Valid options are:
                          - 128 bits
                          - 192 bits
                          - 256 bits
    @return   -  0 if successful, or ERR_AES_INVALID_KEY_LENGTH otherwise
**/
int  pal_crypto_aes_setkey_dec(uint8_t *key, uint32_t keysize);

/**
    @brief    - This function performs an AES-ECB single-block encryption or decryption operation.
                It performs the operation defined in the mode parameter (encrypt or decrypt),
                on the input data buffer defined in the input parameter.
                mbedtls_aes_init(), and either mbedtls_aes_setkey_enc() or mbedtls_aes_setkey_dec()
                must be called before the first call to this API with the same context.
    @param    - mode     : The AES operation: AES_ENCRYPT or AES_DECRYPT
                input    : 16-byte input block
                output   : 16-byte output block
    @return   - 0 if successful
**/
int  pal_crypto_aes_crypt_ecb(int mode, uint8_t *input, uint8_t *output);

/**
    @brief    - This function performs an AES-CBC encryption or decryption operation on full blocks.
                It performs the operation defined in the mode parameter (encrypt or decrypt),
                on the input data buffer defined in the input parameter.
                It can be called as many times as needed, until all the input data is processed.
                mbedtls_aes_init(), and either mbedtls_aes_setkey_enc() or mbedtls_aes_setkey_dec()
                must be called before the first call to this API with the same context.
    @param    - mode     : The AES operation: AES_ENCRYPT or AES_DECRYPT
                length   : The length of the input data in Bytes. This must be a multiple of the block size (16 Bytes).
                iv       : Initialization vector (updated after use).
                input    : 16-byte input block
                output   : 16-byte output block
    @return   - 0 if successful
**/
int  pal_crypto_aes_crypt_cbc(int mode, uint64_t len, uint8_t *iv, uint8_t *input, uint8_t *output);

/**
    @brief    - This function performs an AES-CFB128 encryption or decryption operation.
                It performs the operation defined in the mode parameter (encrypt or decrypt),
                on the input data buffer defined in the input parameter.
                For CFB, you must set up the context with mbedtls_aes_setkey_enc(), regardless of whether you are
                performing an encryption or decryption operation, that is, regardless of the mode parameter.
                This is because CFB mode uses the same key schedule for encryption and decryption.
    @param    - mode     : The AES operation: AES_ENCRYPT or AES_DECRYPT
                length   : The length of the input data in Bytes. This must be a multiple of the block size (16 Bytes).
                iv_offset: The offset in IV (updated after use).
                iv       : Initialization vector (updated after use).
                input    : 16-byte input block
                output   : 16-byte output block
    @return   - 0 if successful
**/
int  pal_crypto_aes_crypt_cfb(int mode, uint64_t len, uint64_t *iv_offset, uint8_t *iv, uint8_t *input, uint8_t *output);

/**
    @brief    - This function performs an AES-CTR encryption or decryption operation.
                It performs the operation defined in the mode parameter (encrypt or decrypt),
                on the input data buffer defined in the input parameter.
                For CTR, you must set up the context with mbedtls_aes_setkey_enc(), regardless of whether you are
                performing an encryption or decryption operation, that is, regardless of the mode parameter.
                This is because CTR mode uses the same key schedule for encryption and decryption.
    @param    - length        : The length of the input data in Bytes. This must be a multiple of the block size (16 Bytes).
                nc_offset     : The offset in the current stream_block, for resuming within the current cipher stream.
                                The offset pointer should be 0 at the start of a stream.
                nonce_counter : The 128-bit nonce and counter.
                stream_block  : The saved stream block for resuming. This is overwritten by the function.
                input         : 16-byte input block
                output        : 16-byte output block
    @return   - 0 if successful
**/
int  pal_crypto_aes_crypt_ctr(uint64_t len, uint64_t *nc_offset, uint8_t *nonce_cntr, uint8_t *stream_block, uint8_t *input, uint8_t *output);

/**
    @brief    - This function performs an AES-CTR encryption or decryption operation.
                It performs the operation defined in the mode parameter (encrypt or decrypt),
                on the input data buffer defined in the input parameter.
                For CTR, you must set up the context with mbedtls_aes_setkey_enc(), regardless of whether you are
                performing an encryption or decryption operation, that is, regardless of the mode parameter.
                This is because CTR mode uses the same key schedule for encryption and decryption.
    @param    - length        : The length of the input data in Bytes. This must be a multiple of the block size (16 Bytes).
                nc_offset     : The offset in the current stream_block, for resuming within the current cipher stream.
                                The offset pointer should be 0 at the start of a stream.
                nonce_counter : The 128-bit nonce and counter.
                stream_block  : The saved stream block for resuming. This is overwritten by the function.
                input         : 16-byte input block
                output        : 16-byte output block
    @return   - 0 if successful
**/
int  pal_crypto_aes_crypt_ofb(uint64_t len, uint64_t *nc_offset, uint8_t *nonce_cntr, uint8_t *stream_block, uint8_t *input, uint8_t *output);

/**
    @brief    - This function initializes the SHA256 context.
                It must be the first API called before using the context.
    @param    -
    @return   -
**/
void pal_crypto_sha256_init(void);

/**
    @brief    - This function starts a SHA-224 or SHA-256 checksum calculation.
    @param    - is_224  : Determines which function to use.
                          0: Use SHA-256
                          1: Use SHA-224
    @return   - 0 on success.
**/
int  pal_crypto_sha256_start(uint32_t is_224);

/**
    @brief    - This function feeds an input buffer into an ongoing SHA-256 checksum calculation.
    @param    - input  : The buffer holding the data.
                ilen   : The length of the input data.
    @return   - 0 on success.
**/
int  pal_crypto_sha256_update(uint8_t *input, uint64_t ilen);

/**
    @brief    - This function finishes the SHA-256 operation, and writes the result to the output buffer.
    @param    - output  : The SHA-224 or SHA-256 checksum result.
    @return   - 0 on success.
**/
int  pal_crypto_sha256_finish(uint8_t *output);

/**
    @brief    - This function generates AES key using various specified entropy sources.
    @param    - key  : The buffer where the generated key is stored
                size : Size of the key to be generated. Valid options are:
                          - 128 bits
                          - 192 bits
                          - 256 bits
    @return   - 0 on success.
**/
int  pal_crypto_aes_generate_key(uint8_t *key, uint32_t size);

/**
    @brief    - This function calculates the SHA-224 or SHA-256 checksum of a buffer.
    @param    - input   : The buffer holding the data.
                ilen    : The length of the input data.
                output  : The SHA-224 or SHA-256 checksum result.
                algo    : Determines which function to use.
                          0: Use SHA-256.
                          1: Use SHA-224.
    @return   - 0 on success.
**/
int  pal_crypto_compute_hash(unsigned char *input, size_t ilen, unsigned char *output, int algo);

/*
    @brief     - This function initializes the uart
    @param     - uart_base_addr : Base address of the UART
    @return    -
*/
void pal_uart_init                  (addr_t uart_base_addr);

/*
    @brief     - Send data to UART TX FIFO
    @param     - data : data to be written to TX FIFO
    @return    - void
*/
void pal_uart_tx                    (uint8_t data);

/**
    @brief    - provides the database source location.
    @param    - void
    @return   - Returns base address of database
**/
void *pal_get_target_cfg_start(void);

/*
    @brief     - Writes 'size' bytes from buffer into NVRAM at a given 'base + offset'.
    @param     - base      : Base address of NVRAM
               - offset    : Offset
               - buffer    : Pointer to source address
               - size      : Number of bytes
    @return    - error status
*/
int  pal_nvram_write(addr_t base, uint32_t offset, void *buffer, int size);

/*
    @brief     - Reads 'size' bytes from NVRAM at a given 'base + offset' into given buffer.
    @param     - base      : Base address of NVRAM
               - offset    : Offset
               - buffer    : Pointer to source address
               - size      : Number of bytes
    @return    - 1/0
*/
int  pal_nvram_read (addr_t base, uint32_t offset, void *buffer, int size);

/*
    @brief     - Generates system warm reset.
    @param     - void
    @return    - void
*/
void  pal_system_warm_reset(void);

/*
    @brief     - Generates system cold reset.
    @param     - void
    @return    - void
*/
void  pal_system_cold_reset(void);

/*
    @brief     - Reports the last reset type.
    @param     - void
    @return    - YES(1)/NO(0)
*/
int  pal_is_cold_reset(void);

/*
    @brief     - Reports the last reset type.
    @param     - void
    @return    - YES(1)/NO(0)
*/
int  pal_is_warm_reset(void);

/**
    @brief    - This function will set the debug permission based on the input arg
    @param    - index          : DPM index
                access_ns      : TRUE  - allow debug access only for non-secure address
                                 FALSE - allow debug access to both secure and non-secure addresses
    @return   - error status
**/
int pal_dpm_set_access_ns_only(uint32_t index, bool_t access_ns);

/**
    @brief    - Allow a memory region to be configured as Non-Secure via MPC
    @param    - instance : MPC instance number
              - addr     : Memory address to be configured by MPC
              - sec_attr : Map the address to either Secure or Non-Secure security attribute
    @return   - error status
**/
int pal_mpc_configure_mem_to_nonsecure (addr_t start_addr,addr_t end_addr);

/**
    @brief    - Allow a memory region to be configured as Secure via MPC
    @param    - instance : MPC instance number
              - addr     : Memory address to be configured by MPC
              - sec_attr : Map the address to either Secure or Non-Secure security attribute
    @return   - error status
**/
int pal_mpc_configure_mem_to_secure (addr_t start_addr,addr_t end_addr);

/**
    @brief    - Read the value of given fuse address
    @param    - addr     : Address of the fuse
              - data     : Buffer to store the data
              - size     : Number of words to be read
    @return   - error status
**/
int pal_fuse_read(addr_t addr, uint32_t *data, size_t size);

/**
    @brief    - Write the value in given fuse address
    @param    - addr     : Address of the fuse
              - data     : Data to be written
    @return   - error status
**/
int pal_fuse_write(addr_t addr, uint32_t *data, size_t size);

/**
    @brief    - Count the number of Zeros in ROTPK
    @param    - zero_cnt : Buffer to store the zero count
    @return   - error status
**/
int pal_fuse_count_zeros_in_rotpk(uint32_t *zero_cnt);

/**
    @brief    - Count the number of Zeros in the given value
    @param    - value    : They number of zeros to be determined
                zero_cnt : Buffer to store the zero count
    @return   -
**/
void pal_fuse_count_zeros(uint32_t value, uint32_t *zero_cnt);

/**
    @brief    - This function will read the LCS register
    @param    - Buffer to store the LCS value
    @return   - error status
**/
int pal_fuse_get_lcs(uint32_t *pLcs);

/**
    @brief    - This function will validate the cerificate using public key.
    @param    - certificate_base_addr : Base address of the certificate where it is stored in memory
                public_key_addr       : Base address of the public key where it is stored in memory
                certificate_size      : Certificate memory size
                public_key_size       : Public key memory size
    @return   - 0 on success.
**/
int pal_crypto_validate_certificate(uint32_t certificate_base_addr, uint32_t public_key_addr, uint32_t certificate_size, uint32_t public_key_size);

/**
    @brief    - This function will get unique ID from valid cerificate using public key
    @param    - certificate_base_addr : Base address of the certificate where it is stored in memory
                public_key_addr       : Base address of the public key where it is stored in memory
                certificate_size      : Certificate memory size
                public_key_size       : Public key memory size
    @return   - unique ID of the certificate
**/
int pal_crypto_get_uniqueID_from_certificate(uint32_t certificate_base_addr, uint32_t public_key_addr, uint32_t certificate_size, uint32_t public_key_size);

#endif /* PAL_INTERFACES_H */
