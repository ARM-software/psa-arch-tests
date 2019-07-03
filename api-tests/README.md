
# PSA APIs Architecture Test Suite

## Introduction

Arm *Platform Security Architecture* (PSA) is a holistic set of threat models, security analyses, hardware and firmware architecture specifications, and an open source firmware reference implementation. PSA provides a recipe, based on industry best practice, that allows security to be consistently designed in, at both a hardware and firmware level. One of the PSA goals is to make IoT security easier and quicker for everyone. This means having reliable, consistent APIs and useful built-in security functions for device manufacturers and the developer community. These PSA APIs provides a consistent developer experience, hiding the underlying complexity of the security system.

For more information, visit the [PSA Webpage](https://developer.arm.com/products/architecture/platform-security-architecture).

## Architecture test suite

The current implementation of the architecture test suite contains tests for PSA Firmware Framework and PSA Developer APIs specifications. The tests are available as open source. The tests and the corresponding abstraction layers are available with an Apache v2.0 license, allowing external contribution.

### PSA Firmware Framework (PSA-FF)
The test suite for this specification is located in the **ff** directory of this repository. See the [PSA Firmware Framework README](ff/README.md) file for more details.

### PSA Developer APIs
The test suite for this specification is located in the **dev_apis** directory of this repository. See the [PSA Developer APIs README](dev_apis/README.md) file for more details.

## Release Update
 - Release Version: 0.9
 - Code quality: Arm welcomes suggestions for enhancements and error corrections.
 - This release contains following tests: <br />

1. Developer APIs test list:

| Test Category            | Specification Version                |
|--------------------------|--------------------------------------|
| Crypto                   | [PSA Crypto API 1.0-Beta1](../api-specs/crypto/v1.0-beta1/)     |
| Protected Storage        | [PSA Protected Storage API 1.0-Beta2](../api-specs/storage/v1.0-beta2/) |
| Internal Trusted Storage | [PSA Internal Trusted Storage API 1.0-Beta2](../api-specs/storage/v1.0-beta2/) |
| Initial Attestation      | [PSA Initial Attestation API 1.0-Beta0](../api-specs/attestation/v1.0-beta0/)  |

2. PSA-FF tests that are written for version 1.0-Beta1 of the PSA FF specification.

## License

Arm PSA test suite is distributed under Apache v2.0 License.

## Feedback, contributions, and support

 - For feedback, use the GitHub Issue Tracker that is associated with this repository.
 - For support, send an email to support-psa-arch-tests@arm.com with details.
 - Arm licensees can contact Arm directly through their partner managers.
 - Arm welcomes code contributions through GitHub pull requests.

--------------

*Copyright (c) 2018-2019, Arm Limited and Contributors. All rights reserved.*
