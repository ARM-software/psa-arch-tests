
# Arm Platform Security Architecture : Test Suite


## Introduction

The Platform Security Architecture (PSA) is a holistic set of threat models, security analyses, hardware and firmware architecture specifications, and an open source firmware reference implementation. PSA provides a recipe, based on industry best practice, that allows security to be consistently designed in, at both a hardware and firmware level.


For more information, visit the PSA webpage [here](https://developer.arm.com/products/architecture/platform-security-architecture)

## Architecture test suite

The current implementation of the Architecture test suite contains tests for the TBSA-v8M specification. Arm licensees may contact their partner manager to obtain a copy of this specification. <br />

Future versions of the test suite will be enhanced to include tests for other specifications of the Platform Security Architecture.

The tests are available as open source. The tests and the corresponding abstraction layers are available with an Apache v2 license allowing for external contribution.

### TBSA-v8M
The test suite for this specification is located in the tbsa-v8m directory of this repository. See [TBSA-v8m Readme](tbsa-v8m/README.md) file for more details.

### Firmware Framework
The test suite for this specification is located in the psa-ff directory of this repository. See [Firmware Framework Readme](psa-ff/README.md) file for more details.


## License

Arm PSA test suite is distributed under Apache v2.0 License.


## Feedback, contributions, and support

 - For feedback, use the GitHub Issue Tracker that is associated with this repository.
 - For support, send an email to support-psa-arch-tests@arm.com with details.
 - Arm licensees can contact Arm directly through their partner managers.
 - Arm welcomes code contributions through GitHub pull requests. 

--------------

*Copyright (c) 2018, Arm Limited and Contributors. All rights reserved.*
