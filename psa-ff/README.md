
# PSA Firmware Framework : Architecture Test Suite

## Introduction

### PSA FF Specification

Arm Platform Security Architecture (PSA) is a holistic set of threat models, security analysis, hardware and firmware architecture specifications, and an open source firmware reference implementation. PSA provides a recipe, based on industry best practice, that allows security to be consistently designed in, at both a hardware and firmware level.

The PSA Firmware Framework defines a standard programming environment and firmware interfaces for implementing and accessing security services within a device's Root of Trust.

The Firmware Framework specification: <br />
-   provides requirements for the SPM
-   defines a standard runtime environment for developing protected RoT Services, including the programming interfaces
    provided by the SPM for implementing and using RoT Services
-   defines the standard interfaces for the PSA RoT Services.

To receive a copy of the PSA FF specification, Arm Licensees may contact Arm through their partner managers.

### Architecture Test Suite

The Architecture Test Suite is a set of examples of the invariant behaviours that are specified by the PSA FF Specification. Use this suite to verify that these behaviours are implemented correctly in your system.

PSA FF Architecture Test Suite contains the tests that are self-checking, portable C-based tests with directed stimulus.

The tests are available as open source. The tests and the corresponding abstraction layers are available with an Apache v2.0 license allowing for external contribution.

## Release Update
 - Release Version - 0.5
 - Code Quality: Alpha <br />
   This indicates the suite is in development and it contains tests which have not been validated on any platform. Please use this opportunity to suggest enhancements and point out errors.
 - Current release contains the test suite for IPC and Crypto tests.
 - This test suite is not a substitute for design verification.
 - To review the test logs, Arm licensees can contact Arm directly through their partner managers.

## Layers

PSA FF Architecture Test Suite uses a layered software-stack approach to enable porting across different test platforms. The constituents of the layered stack are: <br />
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

To build PSA FF test suite for a given platform, execute the following commands:

1. cd psa-ff
2. ./tools/scripts/setup.sh --target <platform_name> --cpu_arch <cpu_architecture_version> --suite <suite_name>  --build <build_dir> <br />

   Examples: <br />
   To compile IPC tests:  ./tools/scripts/setup.sh --target fvp_mps2_cm4_mbedos --cpu_arch armv7m --suite ipc --include <include_path> --build BUILD_IPC <br />
   To compile Crypto tests: ./tools/scripts/setup.sh --target fvp_mps2_cm4_mbedos --cpu_arch armv7m --suite crypto --build BUILD_CRYPTO

<br /> where:

-   <platform_name> is the same as the name of the target specific directory created in the platform/targets/ directory.  <br />
-   <cpu_architecture_version> is the Arm Architecture version name for which compliance binaries should be compiled the binaries. For example, Armv7M, Armv8M-Baseline and Armv8M-Mainline Architecture.  <br />
-   <suite_name> is the suite name and it is same as the suite name available in test_suites/ directory. <br />
-   <include_path> is an additional directory to be included into compiler search path. For IPC tests, it must point to path where psa_client.h and psa_service.h files are located. <br />
-   <build_dir> is an output directory to keep build files.

Refer ./tools/scripts/setup.sh --help to know more about options.

### Build output
PSA FF test suite build generates following output binaries:

	- <build_dir>/BUILD/val/val_nspe.a
	- <build_dir>/BUILD/platform/pal_nspe.a
	- <build_dir>/BUILD/<suite_name>/test_elf_combine.bin

### Binaries integration into your platform

1. Compile compliance tests partition code using your build tool. You must integrate compliance partition code with your software stack containing SPM so that partition code get access to PSA defined client and RoT service APIs. This forms a SPE binary. Refer to the [Porting Guide](docs/Arm_PSA_FF_Arch_Test_Porting_Guide.md) for more detail on compliance partition code compilation.
2. Integrate BUILD/val/val_nspe.a and BUILD/platform/pal_nspe.a libraries with your Non-Secure OS so that these libraries get access to PSA client APIs. For Crypto tests, these libraries would require to get access to PSA Crypto APIs as well. This will form a NSPE binary.
3. Load NSPE binary and test_elf_combine.bin to NS memory
4. Load SPE binary into S memory

## Test Suite Execution
The following steps describe the execution flow prior to the start of test execution: <br />

1. The target platform must load above binaries into appropriate memory. <br />
2. The System Under Test (SUT) would boot to an environment which intializes SPM and PSA FF partitions are ready to accept requests. <br />
3. On the non-secure side, the SUT - boot software would give control to the compliance tests entry point- *void val_entry(void);* as an application entry point. <br />
4. The tests are executed sequentially in a loop in the test_dispatcher function. <br />

## License

Arm PSA FF Architecture test suite is distributed under Apache v2.0 License.


## Feedback, contributions, and support

 - For feedback, use the GitHub Issue Tracker that is associated with this repository.
 - For support, send an email to support-psa-arch-tests@arm.com with details.
 - Arm licensees can contact Arm directly through their partner managers.
 - Arm welcomes code contributions through GitHub pull requests.

--------------

*Copyright (c) 2018, Arm Limited and Contributors. All rights reserved.*
