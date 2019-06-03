
# PSA Developer APIs Architecture Test Suite

## PSA Developer APIs

PSA defines security service APIs for application developers. Some of these services are Crypto, Attestation, and Secure storage. For more information on API specifications, refer to the [PSA Developer APIs Specifications](../../api-specs/).

## Architecture test suite

The architecture test suite is a set of examples of the invariant behaviors that are specified in the PSA Developer APIs specifications. Use this suite to verify whether these behaviors are implemented correctly in your system. This  suite contains self-checking and portable C-based tests with directed stimulus. These tests are available as open source. The tests and the corresponding abstraction layers are available with an Apache v2.0 license, allowing external contribution.

This test suite is not a substitute for design verification. To review the test logs, Arm licensees can contact Arm directly through their partner managers.

For more information on the architecture test suite framework and methodology to run the tests, refer to the [Validation Methodology](../docs/Arm_PSA_APIs_Arch_Test_Validation_Methodology.pdf) document.

## Test scenarios

The mapping of the rules in the specification to the test cases and the steps followed in the tests are mentioned in the [Scenario Document](../docs/) that is present in the **docs/** folder.


## Getting started

Follow the instructions in the subsequent sections to get a copy of the source code on your local machine and build the tests. Make sure you have all required software installed as explained in the [Software Requirements Document](../docs/sw_requirements.md).

### Porting steps

Refer to the [PSA Developer APIs Test Suite Porting Guide](../docs/porting_guide_dev_apis.md) document for porting steps.

### Build steps

To build the test suite for your target platform, execute the following commands:

```
cd api-tests
./tools/scripts/setup.sh --target <platform_name> --cpu_arch <cpu_architecture_version> --suite <suite_name>  --build <build_dir> --include <include_path> --archive_tests
```
<br />  where:

-   <platform_name> is the same as the name of the target-specific directory created in the **platform/targets/** directory.  <br />
-   <cpu_architecture_version> is the Arm Architecture version name for which the tests should be compiled. For example, Armv7M, Armv8M-Baseline and Armv8M-Mainline Architecture.  <br />
-   <suite_name> is the suite name that is the same as the suite name available in **dev_apis/** directory. <br />
-   <build_dir> is a directory to store build output files. <br />
-   <include_path> is an additional directory to be included into the compiler search path. <br />
Note: You must provide Developer APIs header file implementation to the test suite build system using this option. For example, to compile Crypto tests, the include path must point to the path where **psa/crypto.h** is located in your build system.<br />
-  Use **--archive_tests** option to create a combined test archive (**test_combine.a**) file by combining available test object files. Absence of this option creates a combined test binary (**test_elf_combine.bin**) by combining the available test ELF files.

For details about options, refer to **./tools/scripts/setup.sh --help**.

To compile Crypto tests for **tgt_dev_apis_mbedos_fvp_mps2_m4** platform, execute the following commands:
```
cd api-tests
./tools/scripts/setup.sh --target tgt_dev_apis_mbedos_fvp_mps2_m4 --cpu_arch armv7m --suite crypto --build BUILD_CRYPTO --include <include_path>  --archive_tests
```

### Build output
Building the test suite generates the following NSPE binaries:<br />
- **<build_dir>/BUILD/val/val_nspe.a**
- **<build_dir>/BUILD/platform/pal_nspe.a**
- **<build_dir>/BUILD/dev_apis/<suite_name>/test_combine.a**

### Integrating the libraries into your target platform

1. Integrate **val_nspe.a**, **pal_nspe.a**, and **test_combine.a** libraries with your Non-secure OS so that these libraries get access to the PSA Developer APIs. For example, Crypto tests require access to PSA Crypto APIs. This forms an NSPE binary.
2. Load the NSPE binary to the Non-secure memory.
3. Build your SPE binary and load into the Secure memory.

## Test suite execution
The following steps describe the execution flow before the test execution: <br />

1. The target platform must load the above binaries into appropriate memory. <br />
3. Upon booting firmware and Non-secure OS, the SUT boot software gives control to the test suite entry point void **val_entry(void);** as an Non-secure application entry point. <br />
4. The tests are executed sequentially in a loop in the test_dispatcher function. <br />

For details on test suite integration, refer to the **Integrating the test suite with the SUT** section of [Validation Methodology](../docs/Arm_PSA_APIs_Arch_Test_Validation_Methodology.pdf).

## License

Arm PSA test suite is distributed under Apache v2.0 License.


## Feedback, contributions, and support

 - For feedback, use the GitHub Issue Tracker that is associated with this repository.
 - For support, send an email to support-psa-arch-tests@arm.com with details.
 - Arm licensees can contact Arm directly through their partner managers.
 - Arm welcomes code contributions through GitHub pull requests.

--------------

*Copyright (c) 2018-2019, Arm Limited and Contributors. All rights reserved.*
