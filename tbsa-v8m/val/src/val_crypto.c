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

#include "val_crypto.h"
#include "val_peripherals.h"
#include "pal_interfaces.h"

tbsa_status_t val_crypto_compute_hash(unsigned char *input, size_t ilen, unsigned char *output, int algo)
{
    return pal_crypto_compute_hash(input, ilen, output, algo);
}

/**
    @brief    - This function performs an AES encryption
    @param    - data           : The buffer holding the data to be encrypted
                datasize       : Size of the input data
                key            : The encryption key
                keysize        : The size of key passed in bits. Valid options are:
                                 - 128 bits
                                 - 192 bits
                                 - 256 bits
                iv             : Initialization vector (updated after use)
                mode           : The AES algorithm to be used
                output_data    : The buffer that will hold the encrypted data
                output_datasize: Size of the input data since AES encrypted data size is same as input size
    @return   - 0 on success
**/
tbsa_status_t val_crypto_aes_encrypt(uint8_t *data, uint64_t datasize, uint8_t *key, uint32_t keysize, uint8_t *iv, aes_cipher_t mode, uint8_t *output_data, uint32_t output_datasize)
{
   uint32_t num_iteration, i;
   uint32_t status = -1;
   uint64_t offset = 0;
   uint8_t nonce_counter[16];
   uint8_t stream_block[16];

   num_iteration = datasize/16;
   pal_crypto_aes_init();
   status = pal_crypto_aes_setkey_enc(key, keysize);
   if(status != 0)
       return status;

   switch(mode) {
       case AES_ECB:
           for (i = 0; i < num_iteration; i++) {
               status = pal_crypto_aes_crypt_ecb(ENCRYPT, data+(i*16), output_data+(i*16));
               if(status != 0)
                   return status;
           }
           break;
       case AES_CBC:
           status = pal_crypto_aes_crypt_cbc(ENCRYPT, datasize, iv, data, output_data);
           break;
       case AES_CFB:
           status = pal_crypto_aes_crypt_cfb(ENCRYPT, datasize, &offset, iv, data, output_data);
           break;
       case AES_CTR:
           status = pal_crypto_aes_crypt_ctr(datasize, &offset, nonce_counter, stream_block,  data, output_data);
           break;
       case AES_OFB:
           status = pal_crypto_aes_crypt_ofb(datasize, &offset, nonce_counter, stream_block,  data, output_data);
           break;
       default:
           return TBSA_STATUS_INVALID_AES_MODE;

   }
   return status;

}

/**
    @brief    - This function performs an AES encryption
    @param    - data           : The buffer holding the  encrypted data
                datasize       : Size of the encrypted data
                key            : The decryption key
                keysize        : The size of key passed in bits. Valid options are:
                                 - 128 bits
                                 - 192 bits
                                 - 256 bits
                iv             : Initialization vector (updated after use)
                mode           : The AES algorithm to be used
                output_data    : The buffer that will hold the decrypted data
                output_datasize: Size of the encrypted data since AES decrypted data size is same as input size
    @return   - 0 on success
**/
tbsa_status_t val_crypto_aes_decrypt(uint8_t *data, uint64_t datasize, uint8_t *key, uint32_t keysize, uint8_t *iv, aes_cipher_t mode, uint8_t *output_data, uint32_t output_datasize)
{
   uint32_t num_iteration, i;
   uint32_t status = -1;
   uint64_t offset = 0;
   uint8_t nonce_counter[16];
   uint8_t stream_block[16];

   num_iteration = datasize/16;
   pal_crypto_aes_init();
   status = pal_crypto_aes_setkey_dec(key, keysize);
   if(status != 0)
       return status;

   switch(mode) {
       case AES_ECB:
           for (i = 0; i < num_iteration; i++) {
               status = pal_crypto_aes_crypt_ecb(DECRYPT, data+(i*16), output_data+(i*16));
               if(status != 0)
                   return status;
           }
           break;
       case AES_CBC:
           status = pal_crypto_aes_crypt_cbc(DECRYPT, datasize, iv, data, output_data);
           break;
       case AES_CFB:
           status = pal_crypto_aes_crypt_cfb(DECRYPT, datasize, &offset, iv, data, output_data);
           break;
       case AES_CTR:
           status = pal_crypto_aes_crypt_ctr(datasize, &offset, nonce_counter, stream_block,  data, output_data);
           break;
       case AES_OFB:
           status = pal_crypto_aes_crypt_ofb(datasize, &offset, nonce_counter, stream_block,  data, output_data);
           break;
       default:
           return TBSA_STATUS_INVALID_AES_MODE;

   }
   return status;

}

/**
    @brief    - This function generates AES key
    @param    - key      : The buffer where the generated key is stored
                enc_type : Type of algorithm to be used
                size     : Size of the key to be generated
    @return   - 0 on success
**/
tbsa_status_t val_crypto_key_generate(uint8_t *key, crypt_t enc_type, uint32_t size)
{
    uint32_t status = -1;
    switch(enc_type) {
        case AES:
            status = pal_crypto_aes_generate_key(key, size);
            break;
        case RSA:
            /* TBD */
        default:
            return TBSA_STATUS_INVALID_ARGS;
    }
   return status;
}

/**
    @brief    - This function will validate the Public key
    @param    - type   : Type of the public key
                size   : Size of the key
                addr   : Address of the key in fuse
                valid  : Buffer to store if the key is valid
    @return   - error status
**/
tbsa_status_t val_crypto_validate_public_key(crypt_t type, uint32_t *key, uint32_t size, addr_t addr, uint32_t *valid)
{
    uint32_t zerosHash = 0;
    uint32_t zerosCount = 0;
    uint32_t status = TBSA_STATUS_SUCCESS;
    switch(type) {
        case ECC:
        case RSA:
            status = pal_fuse_count_zeros_in_rotpk(&zerosHash);
            if (status != TBSA_STATUS_SUCCESS)
               return status;

            for (int i=0; i<size; i++)
            {
                pal_fuse_count_zeros(key[i], &zerosCount);
            }

            if (zerosHash != zerosCount)
                return TBSA_STATUS_INCORRECT_VALUE;

        break;
        default:
            return TBSA_STATUS_INVALID_ARGS;
    }
    *valid = 1;
    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - This function will key information from targetConfig.
    @param    - key_info_desc : the structure which will have the info
                key_type      : type of the key
                instance      : instance of the key
    @return   - error status
**/
tbsa_status_t val_crypto_get_key_info(key_desc_t **key_desc, key_type_t key_type, uint32_t instance)
{
    key_desc_t    *key_desc_l;
    key_hdr_t     *key_hdr;
    tbsa_status_t status;
    uint32_t      index = 0;
    bool_t        key_present = 0;

    status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_KEY, 0, 0),
                          (uint8_t **)&key_hdr, (uint32_t *)sizeof(key_hdr_t));
    if (status != TBSA_STATUS_SUCCESS)
        return status;

    while (index < key_hdr->num) {
        status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_KEY, KEY_KEY, index),
                              (uint8_t **)&key_desc_l, (uint32_t *)sizeof(key_desc_t));
        if (status != TBSA_STATUS_SUCCESS)
            return status;

        if ((key_desc_l->type & key_type) != key_type) {
            index++;
            continue;
        }

        key_present = 1;
        break;
    }

    if (key_present) {
        *key_desc = key_desc_l;
        return TBSA_STATUS_SUCCESS;
    }

    return TBSA_STATUS_NOT_FOUND;
}

/**
    @brief    - This function will get crypto base address from targetConfig.
    @param    - attribute     : specify if secure or non-secure crypto
    @return   - error status
**/
tbsa_status_t val_crypto_set_base_addr(dev_attr_t attribute)
{
   tbsa_status_t status;
   uint32_t index = 0;
   crypto_hdr_t          *crypto_hdr;
   crypto_desc_t         *crypto_desc;
   bool_t                crypto_present = 0;

   status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_CRYPTO, 0, 0),
                          (uint8_t **)&crypto_hdr,
                          (uint32_t *)sizeof(crypto_hdr_t));

   if (status != TBSA_STATUS_SUCCESS)
       return status;

   while (index < crypto_hdr->num) {
       status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_CRYPTO, CRYPTO_CRYPTO, index),
                                       (uint8_t **)&crypto_desc,
                                       (uint32_t *)sizeof(crypto_desc_t));
       if (status != TBSA_STATUS_SUCCESS)
            return status;

       if (crypto_desc->attribute != attribute) {
            index++;
            continue;
       }

       crypto_present = 1;
       break;
    }

    if (crypto_present) {
        val_crypto_init(crypto_desc->base);
        return TBSA_STATUS_SUCCESS;
    }
    else
    {
        return TBSA_STATUS_NOT_FOUND;
    }
}

/**
    @brief    - This function will revoke the given key
    @param    - index    : Index of the key to be revoked
                addr     : address of the key
                size     : size of the key
    @return   - error status
**/
tbsa_status_t val_crypto_revoke_key(uint32_t index, addr_t addr, size_t size)
{
    /* TBD */
    return TBSA_STATUS_ERROR;
}

/*
     @brief    - Reads the target configuration database to populate the
                 crypto base address
     @param    - crypto_base_addr: address of the crypto base
     @return   - void
*/
void val_crypto_init(uint32_t crypto_base_addr)
{
    pal_crypto_init(crypto_base_addr);
}
/**
    @brief    - This function will validate the certificate using public key
    @param    - certificate_base_addr : Base address of the certificate where it is stored in memory
                public_key_addr       : Base address of the public key where it is stored in memory
                certificate_size      : Certificate memory size
                public_key_size       : Public key memory size
    @return   - error status
**/
tbsa_status_t val_crypto_validate_certificate(addr_t certificate_base_addr,addr_t public_key_addr, size_t certificate_size, size_t public_key_size)
{
    return pal_crypto_validate_certificate(certificate_base_addr,public_key_addr,certificate_size,public_key_size);
}

/**
    @brief    - This function will get unique ID from valid certificate using public key
    @param    - certificate_base_addr : Base address of the certificate where it is stored in memory
                public_key_addr       : Base address of the public key where it is stored in memory
                certificate_size      : Certificate memory size
                public_key_size       : Public key memory size
    @return   - unique ID of the certificate
**/
tbsa_status_t val_crypto_get_uniqueID_from_certificate(addr_t certificate_base_addr,addr_t public_key_addr, size_t certificate_size, size_t public_key_size)
{
    return pal_crypto_get_uniqueID_from_certificate(certificate_base_addr,public_key_addr,certificate_size,public_key_size);
}

/**
    @brief    - This function will get DPM field from public key
    @param    - public_key_addr       : Base address of the public key where it is stored in memory
                public_key_size       : Public key memory size
                dpm_field             : Pointer where DPM will be stored
    @return   - Error status
**/
tbsa_status_t val_crypto_get_dpm_from_key(addr_t public_key_addr, size_t public_key_size, uint32_t *dpm_field)
{
    uint32_t status = TBSA_STATUS_SUCCESS;

    status = pal_crypto_get_dpm_from_key (public_key_addr,public_key_size,(uint32_t *) dpm_field);
    return status;

}

/**
    @brief    - This function will get DPM field from certificate
    @param    - certificate_base_addr       : Base address of the certificate where it is stored in memory
                certificate_size       : certificate memory size
                dpm_field             : Pointer where DPM will be stored
    @return   - Error status
**/
tbsa_status_t val_crypto_get_dpm_from_certificate(addr_t certificate_base_addr, size_t certificate_size, uint32_t *dpm_field)
{
    uint32_t status = TBSA_STATUS_SUCCESS;

    status = pal_crypto_get_dpm_from_certificate (certificate_base_addr, certificate_base_addr, (uint32_t *) dpm_field);
    return status;
}
