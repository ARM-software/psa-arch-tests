
# TBSA-v8M : Architecture Test Suite


## Introduction

### TBSA-v8M Specification

The TBSA-v8M specification defines the behavior of an abstract machine referred to as a TBSA-v8M system. Implementations that are compliant with the TBSA-v8M architecture must conform to the described behavior of the TBSA-v8M System.

To receive a copy of the TBSA-v8M specification, Arm Licensees may contact Arm through their partner managers.

### Architecture test suite

The TBSA-v8M Architecture test suite checks whether an implementation conforms to the behaviors described in the TBSA-v8M specification.

TBSA-v8M compliance tests are self-checking, portable C-based tests
with directed stimulus.

The tests are available as open source. The tests and the corresponding abstraction layers are available with an Apache v2.0 license allowing for external contribution.


## Release details
 - Release Version - 0.6
 - Code Quality: Alpha <br />
   The suite is in development. This indicates that the suite contains tests which have not been validated on any platform. Please use this opportunity to suggest enhancements and point out errors.

 - The TBSA-v8M tests are written for version 1.1 of the TBSA-v8M specification.
 - This test suite is not a substitute for design verification.

 - To review the test logs, Arm licensees can contact Arm directly through their partner managers.

## Layers

TBSA-v8M compliance tests use a layered software-stack approach to enable porting across different test platforms. The constituents of the layered stack are:
         - Test suite
         - Validation Abstraction Layer (VAL)
         - Platform Abstraction Layer (PAL)


These tests are written on top of Validation Abstraction Layer (VAL) and Platform Abstraction Layer (PAL).

The abstraction layers provide platform information and runtime environment to enable execution of the tests.

In this release, PAL is written on top of baremetal drivers and CMSIS.

Partners can also write their own abstraction layer implementations to allow TBSA-v8M tests to be run in other environments, for example, as raw workload on an RTL simulation.

## Scenarios

The mapping of the rules in the specification to the test cases and the steps followed in the tests are mentioned in the [Scenario document](docs/Arm_TBSA-v8M_Scenario_document.pdf) present in the docs/ folder.

## Getting started


Follow the instructions in the subsequent sections to get a copy of the source code on your local machine and build the tests. <br />
See User guide document to get details on the steps involved in Porting the test suite to your platform.


### Prerequisites

Before starting the test suite build, ensure that the following requirements are met:

- Host Operating System     : Ubuntu
- Scripting tools           : Perl 5.12.3
- Other open-source tools   : GCC 6.3.1



## Download source
To download the master branch of the repository, type the following command:

	git clone https://github.com/ARM-software/psa-arch-tests.git


## Porting steps

Refer to the [User Guide](docs/Arm_TBSA-v8M_Validation_Methodology_and_User_Guide.pdf) document in the docs folder for details.

## Build steps

To build TBSA-v8M test suite for a given platform, execute the following commands:
	1. cd syscomp_tbsa_m <br />
	2. make TARGET=<platform_name>  <br />
	   Example: make TARGET=fvp

To build only certain test for a given platform, execute the following commands:
    1. cd syscomp_tbsa_m <br />
    2. make TARGET=<platform_name> SUITE=<test_name> <br />
       Example: make TARGET=fvp SUITE=boot,crypto,mem

<br /> where <platform_name> is the same as the name of the target specific directory created in the platform/board directory.


### Build output
TBSA build generates two output binaries:

	- tbsa.elf
	- tbsa_test_combined.bin

## Test Suite Execution
The following steps describe the execution flow prior to the start of test execution.
1. The target platform must load the tbsa.elf file.
2. The suite exectuion begins from the tbsa_entry.
3. The tests are executed sequentially in a loop in the tbsa_dispatcher function.


## License

Arm TBSA-v8M Architecture test suite is distributed under Apache v2.0 License.


## Feedback, contributions, and support

 - For feedback, use the GitHub Issue Tracker that is associated with this repository.
 - For support, send an email to support-psa-tests-acs@arm.com with details.
 - Arm licensees can contact Arm directly through their partner managers.
 - Arm welcomes code contributions through GitHub pull requests. For details, see "docs/Contributions.txt".
