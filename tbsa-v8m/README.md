
# TBSA-v8M : Architecture Test Suite


## Introduction

### TBSA-v8m Specification

The TBSA-v8M specification defines the behavior of an abstract machine referred as TBSA-v8M system. Implementations compliant with TBSA-v8M architecture must conform to the described behavior of the TBSA-v8M System.

To receive a copy of the TBSA-v8M specification, Arm Licensees may contact Arm through their partner managers.

### Architecture test suite

The TBSA-M Architecture test suite checks whether an implementation conforms to the behaviors described in the TBSA-v8M specification.

TBSA-M compliance tests are self-checking, portable C-based tests
with directed stimulus.

The tests are available as open source. The tests and the corresponding abstraction layers are available with an Apache v2 license allowing for external contribution.


## Release details
 - Code Quality: Alpha v0.5
 - The TBSA-v8M tests are written for version 1.0 of the TBSA-v8M specification. <br />
   The suite quality is Alpha. This indicates that the suite contains tests which have not been validated on any platform and may contain a reasonable amount of functional bugs.

  - This test suite is not a substitute for design verification.

 - To review the test logs, Arm licensees can contact ARM directly through their partner managers.

## Layers

TBSA-M compliance tests use a layered software-stack approach to enable porting across different test platforms. The constituents of the layered stack are as below:
         - Test suite
         - Validation Abstraction Layer (VAL)
         - Platform Abstraction Layer (PAL)


These tests are written on top of Validation Abstraction Layer (VAL) and Platform Abstraction Layer (PAL).

The abstraction layers provide platform information and runtime environment to enable execution of the tests.

In the present release, PAL is written on top of baremetal drivers and CMSIS.

Partners can also write their own abstraction layer implementations to allow TBSA-v8M tests to be run in other environments, for example, as raw workload on an RTL simulation.


## Getting started

	Follow the instructions in the subsequent sections to get a copy of the source code on your local machine and build the tests. See 'Porting steps for TBSA-v8M test suite' to port the TBSA-M test suite on a given target system.


### Prerequisites

Before starting the test suite build, ensure that the following requirements are met:

- Host Operating System     : Ubuntu
- Scripting tools           : Perl 5.12.3
- Other open-source tools   : GCC 6.3.1

Refer to the docs/TBSA-M-User-Guide.pdf for other environment requirements of the TBSA-M test suite.
<br />


## Download source
	To download the master branch of the repository, type the below command

	git clone https://github.com/ARM-software/psa-m-arch-test.git <br />


## Porting steps

TBD

## Build steps

To build TBSA-M ACK for a given platform, execute the following commands:

      1. cd syscomp_tbsa_m <br />
      2. make TARGET=<platform_name>  <br />
          ex: make TARGET=fvp        <br />


### Build output
	TBSA build generates two output binaries:
        - tbsa.elf and 
        - tbsa_test_combined.bin


## Test Suite Execution

TBD

## License

ARM TBSA-v8M Architecture test suite is distributed under Apache v2.0 License.


## Feedback, contributions, and support

 - For feedback, use the GitHub Issue Tracker that is associated with this repository.
 - For support, please send an email to "support-psa-tests-acs@arm.com" with details.
 - ARM licensees can contact ARM directly through their partner managers.
 - ARM welcomes code contributions through GitHub pull requests. For details, see "docs/Contributions.txt".
