
# Test failure analysis document

This file contains list of failures identified when testing the release
on tgt_dev_apis_tfm_an521 and  tgt_dev_apis_tfm_musca_b1
targets with TFM commit Hash - d0f6ceb0559af6ef08f4624eeac079cc07a1f077.
The reason for each failing test is listed here in this file.

## Known Failures

| Test | Fail description                                                                | Github issue |
|------|---------------------------------------------------------------------------------| ------------ |
|test_c021 | psa_key_derivation_output_key returns incorrect error code | https://github.com/ARMmbed/mbed-crypto/issues/175 |
|test_c026 | psa_sign_message not supported | https://jira.arm.com/browse/IOTPSW-4100 |
|test_c027 | psa_mac_update not supported | https://jira.arm.com/browse/IOTPSW-4100 |
|test_c028 | PSA_KEY_USAGE_SIGN_MESSAGE not found | https://jira.arm.com/browse/IOTPSW-4100 |
|test_c029 | PSA_KEY_USAGE_VERIFY_MESSAGE not supported | https://jira.arm.com/browse/IOTPSW-4100 |
|test_c030 | PSA_KEY_USAGE_VERIFY_MESSAGE not supported | https://jira.arm.com/browse/IOTPSW-4100 |
|test_c046 | psa_mac_compute is unimplemented in TFM/mbed-crypto. So the test has not been verified.	| https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c047 | psa_mac_verify is unimplemented in TFM/mbed-crypto. So the test has not been verified.		| https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c048 | psa_cipher_encrypt is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c049 | psa_cipher_decrypt is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://github.com/ARMmbed/mbed-crypto/issues/381 |
|test_c051 | PSA_KEY_USAGE_VERIFY_MESSAGE not supported | https://jira.arm.com/browse/IOTPSW-4100 |
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
|test_c062 | psa_hash_suspend is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://jira.arm.com/browse/IOTPSW-4100 |
|test_c063 | psa_hash_resume is unimplemented in TFM/mbed-crypto. So the test has not been verified. | https://jira.arm.com/browse/IOTPSW-4100 |

## License

Arm PSA test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.*
