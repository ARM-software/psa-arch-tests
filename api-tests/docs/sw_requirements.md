
# Architecture Test Suite Software Requirements

Before starting the test suite build, ensure that the following requirements are met: <br />

- Host Operating System     : Ubuntu 16.04
- Scripting tools           : Perl 5.12.3, Python 3.1.7
- Compiler toolchain        : GNU Arm Embedded Toolchain 6.3.1, Arm Compiler v6.7

**Note**: To compile the test suite code, at least one of the above supported compiler toolchains
        must be available in the build environment.

### Setting up a shell to enable compiler toolchain after installation

To import GNU Arm in your bash shell console, execute:
~~~
export PATH=<GNU_ARM_PATH>/bin:$PATH
~~~

To import Arm Compiler in your bash shell console, execute:
~~~
export PATH=<ARM_COMPILER_PATH>/bin:$PATH
~~~

## Download source

To download the master branch of the repository, type the following command: <br />
~~~
git clone https://github.com/ARM-software/psa-arch-tests.git
~~~

## Cloning the external repository

To build Initial Attestation tests, the PSA APIs test suite clones the following
git repository. The cloning is automated as part of the test suite
build script **./setup.sh**:

~~~
git clone https://github.com/laurencelundblade/QCBOR.git ./platform/targets/<platform_name>/nspe/initial_attestation/ext
cd ./platform/targets/<platform_name>/nspe/initial_attestation/ext; git checkout da53227db1488dde0952bdff66c3d904dce270b3
~~~

## To build on Cygwin(32-bit)

To build test suite on Cygwin ensure all the above prerequisite in place.

**Note**: Downloading the test suite in Window platform can have extra
newline chars than Unix. Therefore, it is recommended to execute the
following command to change the newline char format before running
any test suite command.<br />

~~~
dos2unix ./tools/scripts/setup.sh
~~~

## License
Arm PSA test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2018-2019, Arm Limited and Contributors. All rights reserved.*
