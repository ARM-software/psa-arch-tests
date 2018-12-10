
# PSA APIs : Architecture Test Suite

## Introduction

### PSA APIs

Arm Platform Security Architecture (PSA) is a holistic set of threat models, security analysis, hardware and firmware architecture specifications, and an open source firmware reference implementation. PSA provides a recipe, based on industry best practice, that allows security to be consistently designed in, at both a hardware and firmware level. One of the PSA goals is to make IoT security easier and quicker for everyone. This means having reliable, consistent APIs and useful built-in security functions for device manufacturers and the developer community. These PSA APIs provides a consistent developer experience hiding the underlying complexity of the security system.

The PSA APIs Specification consists of following set of APIs:

#### PSA Firmware Framework (PSA-FF)

The PSA-FF defines a standard interface and framework, to isolate trusted functionality withinconstrained IoT devices.

The framework provides:
- Architecture that describes isolated runtime environments (partitions) for trusted and untrusted firmware.
- A standard model for describing the functionality and resources in each partition.
- A secure IPC interface to request services from other partitions.
- A model that describes how the partitions can interact with one another, as well as the hardware and the firmware framework implementation, itself.
- A standard interfaces for the PSA RoT Services.

This specification enables the development of secure firmware functionality that can be reused on different devices that use any conforming implementation of the Firmware Framework. For more information, download the [PSA FF Specification](https://pages.arm.com/psa-resources-ff.html?_ga=2.97388575.1220230133.1540547473-1540784585.1540547382)

#### PSA Application RoT APIs

PSA defines security service APIs for application developers. Some of these services are Crypto Services, Attestation Services, and Secure Storage Services. For more information, refer the [PSA Application RoT APIs specifications](../specification/)

### Architecture Test Suite

The Architecture Test Suite is a set of examples of the invariant behaviours that are specified by the PSA APIs Specifications. Use this suite to verify that these behaviours are implemented correctly in your system.

PSA APIs Architecture Test Suite contains the tests that are self-checking, portable C-based tests with directed stimulus.

The tests are available as open source. The tests and the corresponding abstraction layers are available with an Apache v2.0 license allowing for external contribution.

## Release Update
 - Release Version - 0.5
 - Code Quality - Alpha. This indicates the suite is in development and it contains tests which have not been validated on any platform. Please use this opportunity to suggest enhancements and point out errors.
 - Current release contains:
   1. **IPC tests** are written for version 1.0-Beta-0 of the PSA FF specification.
   2. **Crypto tests** are written for the crypto service APIs defined in the [psa/crypto.h](../specification/a-rot/include/crypto.h)
 - This test suite is not a substitute for design verification.
 - To review the test logs, Arm licensees can contact Arm directly through their partner managers.

## Layers

PSA APIs Architecture Test Suite uses a layered software-stack approach to enable porting across different test platforms. The constituents of the layered stack are: <br />
-   Test suite
-   Validation Abstraction Layer (VAL)
-   Platform Abstraction Layer (PAL)

The tests are written on top of Validation Abstraction Layer (VAL) and Platform Abstraction Layer (PAL). The abstraction layers provide platform information and runtime environment to enable execution of the tests.

In this release, PAL is written on top of baremetal drivers. It is expected that this layer will be ported to specific software stack of the platform.

## Scenarios

The mapping of the rules in the specification to the test cases and the steps followed in the tests are mentioned in the [Scenario document](docs/) present in the docs/ folder.


## Getting Started

Follow the instructions in the subsequent sections to get a copy of the source code on your local machine and build the tests. <br />

### Prerequisite

Before starting the test suite build, ensure that the following requirements are met: <br />

- Host Operating System     : Ubuntu 16.04
- Scripting tools           : Perl 5.12.3
- Other open-source tools   : GNU Arm Embedded Toolchain 6.3.1

### Download source
To download the master branch of the repository, type the following command: <br />

	git clone https://github.com/ARM-software/psa-arch-tests.git

### Porting steps

Refer to the [Porting Guide](docs/Arm_PSA_FF_Arch_Test_Porting_Guide.md) document for porting steps.
Refer to the [Validation Methodology](docs/Arm_PSA_FF_Arch_Test_Validation_Methodology.pdf) document in the docs folder for additional details.

### Build steps

To build PSA APIs test suite for a given platform, execute the following commands:

1. cd api-tests
2. Using your secure partition build tool, parse following test suite partition manifest files and generate necessary manifest output files. Refer "psa_framework_version" field of manifest to view the version of the PSA Firmware Framework specification this manifest conforms to and your build tool must conform to manifest rules specified in that version of specification.<br />
   <br />Test suite manifest to be parsed:<br />
   - platform/targets/<platform_name>/manifests/common/driver_partition_psa.json
   - platform/targets/<platform_name>/manifests/ipc/client_partition_psa.json
   - platform/targets/<platform_name>/manifests/ipc/server_partition_psa.json

   Where, driver_partition_psa.json is required by all suites - IPC and Crypto. Remaining two are used only by IPC suite, threfore use these two manifest files only if you are compling IPC tests.
3. ./tools/scripts/setup.sh --target <platform_name> --cpu_arch <cpu_architecture_version> --suite <suite_name>  --build <build_dir> --include <include_path><br />

   Examples: <br />
   To compile IPC tests:  ./tools/scripts/setup.sh --target fvp_mps2_cm4_mbedos --cpu_arch armv7m --suite ipc --include <include_path> --build BUILD_IPC <br />
   To compile Crypto tests: ./tools/scripts/setup.sh --target fvp_mps2_cm4_mbedos --cpu_arch armv7m --suite crypto --include <include_path> --build BUILD_CRYPTO

<br />  where:

-   <platform_name> is the same as the name of the target specific directory created in the platform/targets/ directory.  <br />
-   <cpu_architecture_version> is the Arm Architecture version name for which test binaries should be compiled. For example, Armv7M, Armv8M-Baseline and Armv8M-Mainline Architecture.  <br />
-   <suite_name> is the suite name and it is same as the suite name available in test_suites/ directory. <br />
-   <include_path> is an additional directory to be included into compiler search path. If PSA IPC implemented in your platform, include path must point to path where "psa/client.h", "psa/service.h" and test partition manifest output files("psa_manifest/sid.h" and "psa_manifest/<manifestfilename>.h") are located.<br />
-   <build_dir> is an output directory to keep build files.

Refer ./tools/scripts/setup.sh --help to know more about options.

### Build output
PSA APIs test suite build generates following output binaries:<br />

NSPE libraries:<br />
1. <build_dir>/BUILD/val/val_nspe.a
2. <build_dir>/BUILD/platform/pal_nspe.a
3. <build_dir>/BUILD/<top_level_suite>/<suite_name>/test_elf_combine.bin

SPE libraries:<br />
1. <build_dir>/BUILD/partition/driver_partition.a
- Additional SPE libraries explicitly for IPC tests
2. <build_dir>/BUILD/partition/client_partition.a
3. <build_dir>/BUILD/partition/server_partition.a

<br />  where:
- <top_level_suite> is either ff (Firmware Framework) or a-rot (Application RoT) suite.

### Binaries integration into your platform

1. Integrate test partition (SPE archives) with your software stack containing SPM so that partition code get access to PSA defined client and secure partition APIs. This forms a SPE binary.
2. Integrate BUILD/val/val_nspe.a and BUILD/platform/pal_nspe.a libraries with your Non-Secure OS so that these libraries get access to PSA client APIs. For Crypto tests, these libraries would require to get access to PSA Crypto APIs as well. This will form a NSPE binary.
3. Load NSPE binary and test_elf_combine.bin to NS memory
4. Load SPE binary into S memory

## Test Suite Execution
The following steps describe the execution flow prior to the start of test execution: <br />

1. The target platform must load above binaries into appropriate memory. <br />
2. The System Under Test (SUT) would boot to an environment which intializes SPM and test suite partitions are ready to accept requests. <br />
3. On the non-secure side, the SUT - boot software would give control to the test suite entry point- *void val_entry(void);* as an application entry point. <br />
4. The tests are executed sequentially in a loop in the test_dispatcher function. <br />

## License

Arm PSA APIs Architecture test suite is distributed under Apache v2.0 License.


## Feedback, contributions, and support

 - For feedback, use the GitHub Issue Tracker that is associated with this repository.
 - For support, send an email to support-psa-arch-tests@arm.com with details.
 - Arm licensees can contact Arm directly through their partner managers.
 - Arm welcomes code contributions through GitHub pull requests.

--------------

*Copyright (c) 2018, Arm Limited and Contributors. All rights reserved.*
