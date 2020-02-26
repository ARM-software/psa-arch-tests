
# Test failure analysis document

This file contains list of failures identified when running the PSA API tests
on tgt_dev_apis_tfm_an521, tgt_dev_apis_tfm_musca_a and  tgt_dev_apis_tfm_musca_b1
targets with TFM commit Hash - 6e7be077eabef00b4825e245604148d82b11f832.
The reason for each failing test is listed here in this file.

## Known Failures

| Test | Fail description                                                                |
|------|---------------------------------------------------------------------------------|
|test_c006 | psa_hash_compute is unimplemented in TFM	|
|test_c007 | psa_hash_compare is unimplemented in TFM	|
|test_c009 | psa_key_derivation_input_bytes returns incorrect error code |
|test_c016 | psa_generate_key generates incorrect key length for RSA |
|test_c020 | psa_key_derivation_output_bytes returns incorrect error code |
|test_c021 | psa_key_derivation_output_key returns incorrect error code |
|test_c039 | psa_asymmetric_encrypt does not have support for ECC keys |
|test_c046 | psa_mac_compute is unimplemented in TFM	|
|test_c047 | psa_mac_verify is unimplemented in TFM		|
|test_c048 | psa_cipher_encrypt is unimplemented in TFM |
|test_c049 | psa_cipher_decrypt is unimplemented in TFM |
|test_c050 | psa_open_key API not enabled in TFM |
|test_c051 | psa_close_key API not enabled in TFM |
|test_c052 | psa_aead_encrypt_setup is unimplemented in TFM |
|test_c053 | psa_aead_decrypt_setup is unimplemented in TFM |
|test_c054 | psa_aead_generate_nonce is unimplemented in TFM |
|test_c055 | psa_aead_set_nonce is unimplemented in TFM |
|test_c056 | psa_aead_set_lengths is unimplemented in TFM |
|test_c057 | psa_aead_update_ad is unimplemented in TFM |
|test_c058 | psa_aead_update is unimplemented in TFM |
|test_c059 | psa_aead_finish is unimplemented in TFM |
|test_c060 | psa_aead_abort is unimplemented in TFM |
|test_c061 | psa_aead_verify is unimplemented in TFM |

## License

Arm PSA test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2020, Arm Limited and Contributors. All rights reserved.*
