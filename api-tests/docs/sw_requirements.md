
# Architecture Test Suite Software Requirements

Before starting the test suite build, ensure that the following requirements are met: <br />

- Host Operating System     : Ubuntu 16.04, Windows 10
- Scripting tools           : Python 3.1.7
- Compiler toolchain        : GNU Arm Embedded Toolchain 6.3.1, Arm Compiler v6.7, GCC 5.4.0 32-Bit (Linux Host), MinGW 6.3.0 32-Bit (Windows Host)
- Build tools               : CMake 3.10

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
git repository. The cloning is automated as part of the test suite cmake build:

**Repo** <br />
Name      : https://github.com/laurencelundblade/QCBOR.git <br />
Commit id : 42272e466a8472948bf8fca076d113b81b99f0e0

## License
Arm PSA test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2018-2019, Arm Limited and Contributors. All rights reserved.*
