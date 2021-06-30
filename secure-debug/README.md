
# PSA Secure Debug : Arm ADAC Test Suite

## Introduction

### ADAC Specification

The ADAC specification defines the protocol that allows a target to securely authenticate a debug host. The specification targets software-only layers that sit above the physical debug link.

The ADAC specification can be obtained at [ADAC specification](https://developer.arm.com/documentation/den0101/0000).

### ADAC test suite

The ADAC test suite checks if a device-side implementation conforms to the behavior described in the ADAC specification. The tests are self-checking, portable C-based tests with directed stimulus. The tests are expected to run on the host platform only. The tests drive the commands from the host platform via the debug link and verify the response obtained from the target platform.

The tests are available as open source. The tests and the corresponding abstraction layers are available with an Apache v2.0 license allowing for external contribution.

## Release details
 - Release Version - 0.5
 - Code Quality: Alpha <br />
   The suite is in development. This indicates that the suite contains tests that has not been validated on any hardware platform. Please use this opportunity to suggest enhancements and point out errors.

 - The ADAC tests are written for version Beta of the [ADAC specification](https://developer.arm.com/documentation/den0101/0000).
 - This test suite is not a substitute for design verification.

## Layers

ADAC tests use a layered software-stack approach to enable porting across different test platforms. The constituents of the layered stack are:
        - Test suite
        - Validation Abstraction Layer (VAL)
        - Platform Abstraction Layer (PAL)

These tests are written on top of Validation Abstraction Layer (VAL) and Platform Abstraction Layer (PAL).

The abstraction layers provide platform information and runtime environment to enable execution of the tests.

In this release, PAL implementation uses unix sockets as the link layer for sending commands and receiving responses.

Partners can also write their own abstraction layer implementations to allow ADAC tests to be run in other host platforms and support the debug link layer.

## Scenarios

The mapping of the rules in the specification to the test cases and the steps followed in the tests are mentioned in the [Scenario document](docs/psa_adac_scenarios.md) present in the docs/ folder.

## Getting started


Follow the instructions in the subsequent sections to get a copy of the source code on your local machine and build the tests. <br />
See User guide document to get details on the steps involved in porting the test suite to your platform.
This test suite is provided as a separate directory within the psa-arch-tests repository.

### Prerequisites

Before starting the test suite build, ensure that the following requirements are met:

- Host Operating System     : Ubuntu 16.04.4, RHEL 7, Windows 10
- Scripting tools           : Python 3.7.1
- Other open-source tools   : GNUARM 6.3.1, GCC 5.4.0 32-Bit (Linux Host), mingw 6.3.0 32-Bit (Windows Host)
                              CMake 3.1 or later


## Download source
To download the master branch of the repository, type the following command:

	git clone https://github.com/ARM-software/psa-arch-tests.git

To download the master branch of the psa-adac repository, type the following command:

	git clone git@github.com:ARMmbed/psa-adac.git

Note:
    To obtain the access to the psa-adac repository and some of its dependent submodules, Arm Licensees may contact Arm through their partner managers.


## Porting steps

Refer to the [Porting Guide](docs/porting_guide_adac_host.md) document for porting steps for your host platform.
Refer to the [User Guide](docs/Arm_Authenticated_Debug_Access_Control_Test_Suite_User_Guide.pdf) document in the docs folder for additional details.

## Build steps

To build ADAC test suite for a given host platform, execute the following commands: <br/>
~~~
    cd psa-arch-tests/secure-debug
    git clone git@github.com:ARMmbed/psa-adac.git --recurse-submodules
    mkdir <host_build_dir>
    cd <host_build_dir>
	cmake ../ -G<generator-name> -DTARGET=<target-name> -DCOMPILER=<compiler-selection> -DSUITE=<suite-selection>
	    where:
        - <generator-name>          "Unix Makefiles"  - to generate Makefiles for Linux and Cygwin
                                    "MinGW Makefiles" - to generate Makefiles for cmd.exe on Windows
        - <target-name>             target to build, as created in the platform/board directory
        - <compiler-selection>      GNU
                                    Defaults to GNU if not specified
        - <suite-selection>         ADAC              - specify the ADAC suite

	To build project
	   cmake --build .
	To clean
	   cmake --build . -- clean

    For example:
    cmake ../ -G"Unix Makefiles" -DTARGET=native -DSUITE=ADAC
    cmake --build .
~~~

Note:
    It is recommended to build each different build configurations in separate directories.

### Build output
The following ADAC test suite build outputs are available under host build directory, as created.

	- psa_adac_test executable
	- val and platform layer libraries

## Test Suite Execution

The current release provides a reference implementation of ADAC target which communicates with the host platform using unix sockets. In order to execute the test suite, please follow the steps below:

    cd psa-arch-tests/secure-debug/psa-adac
    make native

To launch the target to run as a server instance, run the target executable in another terminal as shown:

    cd psa-arch-tests/secure-debug/psa-adac
    ./build/native/Debug/psa_adac_server coms.socket tools/test/resources/chains/chain.EcdsaP256-0

To run the test suite, execute the following command from your host build directory <host_build_dir>:

    ./psa_adac_test ../psa-adac/tools/test/resources/keys/EcdsaP256Key-0.pem \../psa-adac/tools/test/resources/chains/chain.EcdsaP256-0 ../psa-adac/coms.socket

Note:
 - Please look at the psa-adac directory for details about other target platforms.
 - The keys and the certificate credentials for a supported cryptosystem can be generated using the tools provided in the psa-adac repository.
 - The test suite is designed to run once per supported cryptosystem. Partners must ensure that the device must be provisioned with the correct certificates installed in the device's RoT.


## Security implication

ADAC test suite may run at higher privilege level. An attacker can utilize these tests to elevate privilege which can potentially reveal the platform secure attests. To prevent such security vulnerabilities into the production system, it is strongly recommended that ADAC test suite is run on development platforms. If it is run on production system, make sure system is scrubbed after running the test suite.

## License

Arm ADAC Architecture test suite is distributed under Apache v2.0 License.
The psa-adac code repository is distributed under BSD-3-Clause License.


## Feedback, contributions, and support

 - For feedback, use the GitHub Issue Tracker that is associated with this repository.
 - For support, send an email to support-psa-arch-tests@arm.com with details.
 - Arm licensees can contact Arm directly through their partner managers.
 - Arm welcomes code contributions through GitHub pull requests.

--------------

*Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.*
