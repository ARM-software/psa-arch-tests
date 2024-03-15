
# Test failure analysis document

This file contains list of failures identified when testing the release on tgt_dev_apis_tfm_an521 target with TFM commit Hash - cec79b0328125b6730879966261ddf1a869440c4.
The reason for each failing test is listed here in this file.

## Known Failures

CRYPTO:

| Test | Fail description                                                                | Github issue |
|------|---------------------------------------------------------------------------------| ------------ |
|test_c024 <br />test_c025 <br />test_c046<br /> test_c047<br /> test_c050<br /> test_c051 | Enabling the ARCH_TEST_ARIA configuration to test the ARIA key type will cause the failure. | https://github.com/Mbed-TLS/mbedtls/pull/8763 |
|test_c064 | psa_hash_suspend is unimplemented in TF-M/mbed-crypto. So the test has not been verified. |  |
|test_c065 | psa_hash_resume is unimplemented in TF-M/mbed-crypto. So the test has not been verified. |  |
|test_c067 | psa_key_derivation_verify_bytes is unimplemented in TF-M/mbed-crypto. So the test has not been verified. | https://github.com/Mbed-TLS/mbedtls/issues/7135 |
|test_c068 | psa_key_derivation_verify_key is unimplemented in TF-M/mbed-crypto. So the test has not been verified. | https://github.com/Mbed-TLS/mbedtls/issues/7135 |

## License

Arm PSA test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2021-2024, Arm Limited and Contributors. All rights reserved.*
