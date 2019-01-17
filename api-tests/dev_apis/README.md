
# PSA Developer APIs : Architecture Test Suite

## Introduction

### PSA Developer APIs

PSA defines security service APIs for application developers. Some of these services are Crypto Services, Attestation Services, and Secure Storage Services. For more information on API specification, refer the [PSA Developer facing APIs specifications](../../api-specs/)

### Architecture Test Suite

The Architecture Test Suite is a set of examples of the invariant behaviours that are specified by the PSA Developer APIs Specifications. Use this suite to verify that these behaviours are implemented correctly in your system.

The Architecture Test Suite contains the tests that are self-checking, portable C-based tests with directed stimulus.

The tests are available as open source. The tests and the corresponding abstraction layers are available with an Apache v2.0 license allowing for external contribution. This test suite is not a substitute for design verification. To review the test logs, Arm licensees can contact Arm directly through their partner managers.

For more information on Architecture Test Suite specification, refer the [Validation Methodology](../docs/Arm_PSA_APIs_Arch_Test_Validation_Methodology.pdf) document.

## Tests Scenarios

The mapping of the rules in the specification to the test cases and the steps followed in the tests are mentioned in the [Scenario document](../docs/) present in the docs/ folder.


## Getting Started

Follow the instructions in the subsequent sections to get a copy of the source code on your local machine and build the tests. <br />

### Prerequisite

Please make sure you have all required software installed as explained in the [software requirements](../docs/sw_requirements.md).

### Porting steps

Refer to the [PSA Developer APIs Test Suite Porting Guide](../docs/porting_guide_dev_apis.md) document for porting steps.

### Build steps

To build test suite for a given platform, execute the following commands:

```
cd api-tests
./tools/scripts/setup.sh --target <platform_name> --cpu_arch <cpu_architecture_version> --suite <suite_name>  --build <build_dir> --include <include_path>
```
<br />  where:

-   <platform_name> is the same as the name of the target specific directory created in the platform/targets/ directory.  <br />
-   <cpu_architecture_version> is the Arm Architecture version name for which test binaries should be compiled. For example, Armv7M, Armv8M-Baseline and Armv8M-Mainline Architecture.  <br />
-   <suite_name> is the suite name and it is same as the suite name available in test_suites/ directory. <br />
-   <include_path> is an additional directory to be included into compiler search path. Note- You must provide Developer APIs header file implementation to Test Suite build system using this option. For example - To compiler Crypto tests, include path must point to path where "psa/crypto.h" is located in your build system.<br />
-   <build_dir> is an output directory to keep build files.

Refer ./tools/scripts/setup.sh --help to know more about options.

*To compile crypto tests for tgt_dev_apis_mbedos_fvp_mps2_m4 platform*
```
cd api-tests
./tools/scripts/setup.sh --target tgt_dev_apis_mbedos_fvp_mps2_m4 --cpu_arch armv7m --suite crypto --build BUILD_CRYPTO --include <include_path>
```

### Build output
Test suite build generates following binaries:<br />

NSPE libraries:<br />
1. <build_dir>/BUILD/val/val_nspe.a
2. <build_dir>/BUILD/platform/pal_nspe.a
3. <build_dir>/BUILD/dev_apis/<suite_name>/test_elf_combine.bin

### Binaries integration into your platform

1. Integrate val_nspe.a and pal_nspe.a libraries with your non-secure OS so that these libraries get access to PSA Developer APIs. For example,  Crypto tests would require to get access to PSA Crypto APIs. This will form a NSPE binary.
2. Load NSPE binary and test_elf_combine.bin to NS memory
3. Build your SPE binary and load into S memory

## Test Suite Execution
The following steps describe the execution flow prior to the start of test execution: <br />

1. The target platform must load above binaries into appropriate memory. <br />
3. Upon booting firmware and non-secure OS, the SUT - boot software would give control to the test suite entry point- *void val_entry(void);* as an non-secure application entry point. <br />
4. The tests are executed sequentially in a loop in the test_dispatcher function. <br />

## License

Arm PSA test suite is distributed under Apache v2.0 License.


## Feedback, contributions, and support

 - For feedback, use the GitHub Issue Tracker that is associated with this repository.
 - For support, send an email to support-psa-arch-tests@arm.com with details.
 - Arm licensees can contact Arm directly through their partner managers.
 - Arm welcomes code contributions through GitHub pull requests.

--------------

*Copyright (c) 2018-2019, Arm Limited and Contributors. All rights reserved.*

