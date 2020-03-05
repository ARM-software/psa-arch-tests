
# Test failure analysis document

This file contains list of failures identified when testing the release
on tgt_dev_apis_tfm_an521, tgt_dev_apis_tfm_musca_a and  tgt_dev_apis_tfm_musca_b1
targets with TFM commit Hash - 6e7be077eabef00b4825e245604148d82b11f832.
The reason for each failing test is listed here in this file.

## Known Failures

| Test | Fail description                                                                | Github issue |
|------|---------------------------------------------------------------------------------| ------------ |
|test_c006 | psa_hash_compute is unimplemented in TFM/mbed-crypto. So the test has not been verified. 	| https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c007 | psa_hash_compare is unimplemented in TFM/mbed-crypto. So the test has not been verified.	| https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c009 | psa_key_derivation_input_bytes returns incorrect error code | https://github.com/ARMmbed/mbed-crypto/issues/175 |
|test_c020 | psa_key_derivation_output_bytes returns incorrect error code | https://github.com/ARMmbed/mbed-crypto/issues/175 |
|test_c021 | psa_key_derivation_output_key returns incorrect error code | https://github.com/ARMmbed/mbed-crypto/issues/175 |
|test_c046 | psa_mac_compute is unimplemented in TFM/mbed-crypto. So the test has not been verified.	| https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c047 | psa_mac_verify is unimplemented in TFM/mbed-crypto. So the test has not been verified.		| https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c048 | psa_cipher_encrypt is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c049 | psa_cipher_decrypt is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c050 | Persistent key storage is unavailable in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/382 |
|test_c051 | Persistent key storage is unavailable in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/382 |
|test_c052 | psa_aead_encrypt_setup is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c053 | psa_aead_decrypt_setup is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c054 | psa_aead_generate_nonce is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c055 | psa_aead_set_nonce is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c056 | psa_aead_set_lengths is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c057 | psa_aead_update_ad is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c058 | psa_aead_update is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c059 | psa_aead_finish is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c060 | psa_aead_abort is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c061 | psa_aead_verify is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |

## License

Arm PSA test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2020, Arm Limited and Contributors. All rights reserved.*
