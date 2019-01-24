
# PSA Firmware Framework : Architecture Test Suite

## Introduction

### PSA Firmware Framework (PSA-FF)

The PSA-FF defines a standard interface and framework, to isolate trusted functionality withinconstrained IoT devices.

The framework provides:
- Architecture that describes isolated runtime environments (partitions) for trusted and untrusted firmware.
- A standard model for describing the functionality and resources in each partition.
- A secure IPC interface to request services from other partitions.
- A model that describes how the partitions can interact with one another, as well as the hardware and the firmware framework implementation, itself.
- A standard interfaces for the PSA RoT Services.

This specification enables the development of secure firmware functionality that can be reused on different devices that use any conforming implementation of the Firmware Framework. For more information, download the [PSA FF Specification](https://pages.arm.com/psa-resources-ff.html?_ga=2.97388575.1220230133.1540547473-1540784585.1540547382)

### Architecture Test Suite

The Architecture Test Suite is a set of examples of the invariant behaviours that are specified by the PSA FF Specification. Use this suite to verify that these behaviours are implemented correctly in your system.

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

Refer to the [PSA-FF Test Suite Porting Guide](../docs/porting_guide_ff.md) document for porting steps.

### Build steps

To build test suite for a given platform, execute the following commands:

1. cd api-tests

2. Using your secure partition build tool, parse following test suite partition manifest files and generate manifest output files. The manifest parsing tool must be compliant to manifest rules defined in the PSA FF specification.<br />
   <br />Test suite manifest to be parsed:<br />
   - platform/targets/<platform_name>/manifests/common/driver_partition_psa.json
   - platform/targets/<platform_name>/manifests/ipc/client_partition_psa.json
   - platform/targets/<platform_name>/manifests/ipc/server_partition_psa.json

3. Compile tests <br />
```
   ./tools/scripts/setup.sh --target <platform_name> --cpu_arch <cpu_architecture_version> --suite <suite_name>  --build <build_dir> --include <include_path>
```
<br />  where:

-   <platform_name> is the same as the name of the target specific directory created in the platform/targets/ directory.  <br />
-   <cpu_architecture_version> is the Arm Architecture version name for which test binaries should be compiled. For example, Armv7M, Armv8M-Baseline and Armv8M-Mainline Architecture.  <br />
-   <suite_name> is the suite name and it is same as the suite name available in test_suites/ directory. <br />
-   <include_path> is an additional directory to be included into compiler search path. Note- To compile ipc tests, include path must point to path where "psa/client.h", "psa/service.h" and test partition manifest output files(``"psa_manifest/sid.h" and "psa_manifest/<manifestfilename>.h"``) are located in your build system.<br />
-   <build_dir> is an output directory to keep build files.

Refer ./tools/scripts/setup.sh --help to know more about options.

*To compile ipc tests for tgt_ff_mbedos_fvp_mps2_m4 platform*
```
cd api-tests
./tools/scripts/setup.sh --target tgt_ff_mbedos_fvp_mps2_m4 --cpu_arch armv7m --suite ipc --build BUILD_IPC --include <include_path1> --include <include_path2>
```

### Build output
Test suite build generates following binaries:<br />

NSPE libraries:<br />
1. <build_dir>/BUILD/val/val_nspe.a
2. <build_dir>/BUILD/platform/pal_nspe.a
3. <build_dir>/BUILD/ff/<suite_name>/test_elf_combine.bin

SPE libraries:<br />
1. <build_dir>/BUILD/partition/driver_partition.a
2. <build_dir>/BUILD/partition/client_partition.a
3. <build_dir>/BUILD/partition/server_partition.a

### Binaries integration into your platform

1. Integrate test partition (SPE archives) with your software stack containing SPM so that partition code get access to PSA defined client and secure partition APIs. This forms a SPE binary.
2. Integrate BUILD/val/val_nspe.a and BUILD/platform/pal_nspe.a libraries with your Non-Secure OS so that these libraries get access to PSA client APIs. This will form a NSPE binary.
3. Load NSPE binary and test_elf_combine.bin to NS memory
4. Load SPE binary into S memory

## Test Suite Execution
The following steps describe the execution flow prior to the start of test execution: <br />

1. The target platform must load above binaries into appropriate memory. <br />
2. The System Under Test (SUT) would boot to an environment which intializes SPM and test suite partitions are ready to accept requests. <br />
3. On the non-secure side, the SUT - boot software would give control to the test suite entry point- *void val_entry(void);* as an application entry point. <br />
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

