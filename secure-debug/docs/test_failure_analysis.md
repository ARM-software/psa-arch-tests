
# Test failure analysis document
-----------------------------------------------------

This file contains list of failures identified when testing the release of Alpha1 quality ADAC Test suite.
PSA-ADAC repository commit Hash - e91ebc4c06351e860b5c50dbdc2901a8260979a1
along with the patch file adac_ats_alpha1.patch
The reason for each failing test is listed here in this file.

## Known Failures

| Test | Fail description                                                                | Github issue |
|------|---------------------------------------------------------------------------------| ------------ |
|test_a010 | Token extension hash signing check failed | https://github.com/ARM-software/psa-adac/issues/5 |

## License

Arm PSA test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.*
