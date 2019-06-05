#!/usr/bin/env bash
#/** @file
# * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
# * SPDX-License-Identifier : Apache-2.0
# *
# * Licensed under the Apache License, Version 2.0 (the "License");
# * you may not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# *  http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
#**/

echo ""

declare -a INCLUDE_PATHS
export SUITE=" "
export TEST_COMBINE_ARCHIVE=0
export INCLUDE_PANIC_TESTS=0
export WATCHDOG_AVAILABLE=0
export SP_HEAP_MEM_SUPP=0
export CLIENT_FILE_FOUND=0
export SERVICE_FILE_FOUND=0
export MANIFEST_OUT_FILE_FOUND=0
export CRYPTO_FILE_FOUND=0
export PROTECTED_STORAGE_FILE_FOUND=0
export INTERNAL_TRUSTED_STORAGE_FILE_FOUND=0
export INITIAL_ATTESTATION_FILE_FOUND=0
export LIFECYCLE_FILE_FOUND=0

IPC_HEADER_FILE_REQ="If PSA IPC implemented in your platform, include path must point to path
where \"psa/client.h\", \"psa/service.h\", \"psa/lifecycle.h\" and test partition manifest output files
(\"psa_manifest/sid.h\", \"psa_manifest/pid.h\" and \"psa_manifest/<manifestfilename>.h\") are located.
"
CRYPTO_HEADER_FILE_REQ="If PSA CRYPTO APIs are implemented into your platform then you must provide
\"psa/crypto.h\" file to setup.sh script using --include option to compile tests and framework.
"
PS_HEADER_FILE_REQ="If PSA PROTECTED STORAGE APIs are implemented into your platform then you must provide
\"psa/protected_storage.h\" file to setup.sh script using --include option to compile tests and framework.
"
ITS_HEADER_FILE_REQ="If PSA INTERNAL_TRUSTED_STORAGE APIs are implemented into your platform then you must provide
\"psa/internal_trusted_storage.h\" file to setup.sh script using --include option to compile tests and framework.
"
ATTESTATION_HEADER_FILE_REQ="If PSA INITIAL_ATTESTATION APIs are implemented into your platform then you must provide
\"psa/initial_attestation.h\" and \"psa/crypto.h\" file to setup.sh script using --include option to compile tests and framework.
"

HELP="

Usage: setup.sh [--source SOURCE_DIR] [--build BUILD_DIR] [--target TARGET] [--suite SUITE]
                [--toolchain TOOLCHAIN] [--cpu_arch CPU_ARCH] [--verbose PRINT_LEVEL]
                [--include INCLUDE_PATH] [--help|-h]
Toplevel script to build tests and framework sources for given test suite.

Arguments Info:
    --source <SOURCE_DIR>   : SOURCE_DIR pointing to architecture test suite directory structure.
                              Default is current directory
    --build  <BUILD_DIR>    : To select the build (output) directory. Default: BUILD/ inside current directory
    --target <TARGET>       : Provide target string as argument.
                              target.cfg file corresponding to input string must be avaiable at
                              platform/targets/<TARGET>/
    --suite <SUITE>         : Compile tests for given suite. Support values are:
                              ipc, crypto, internal_trusted_storage, protected_storage,
                              and initial_attestation.
    --toolchain <TOOLCHAIN> : Build using the given TOOLCHAIN.
                              Supported values are GNUARM (GNU Arm Embedded) and ARMCLANG (ARM Compiler 6.x).
    --cpu_arch <CPU_ARCH>   : Provide cpu arch string as argument.
                              Supported CPU arch are armv8m_ml, armv8m_bl and armv7m.
    --verbose <PRINT_LEVEL> : Print verbosity level
                              Supported print levels are:
                                1 - INFO & above.
                                2 - DEBUG & above.
                                3 - TEST & above.(Default)
                                4 - WARN & ERROR.
                                5 - ERROR.
    --archive_tests         : Create combine test archive(.a) file by combining available test objects files.
                              Absence of this option would create combine test binary(.bin) by combining available test elfs
    --include_panic_tests   : Consider panic tests (mentioned in testsuite.db of respective suite) along with functional tests
                              for building the final executables. Absence of this option would consider only non-panic (ie, functional) tests
    --include <INCLUDE_PATH>: Additional directory to be included into compiler search path. Provide --include <path>
                              where path pointing to location of PSA defined header files.
                              You can specify multiple source locations using --include option.
                              Ex: --include <path1>  --include <path2>
    --help|-h               : Print this help message

Notes:
1. $IPC_HEADER_FILE_REQ
2. $CRYPTO_HEADER_FILE_REQ
3. $PS_HEADER_FILE_REQ
4. $ITS_HEADER_FILE_REQ
5. $ATTESTATION_HEADER_FILE_REQ

"

if [ "$#" == "0" ]; then
    echo "Error: no argument to setup.sh"
    echo "$HELP"
    exit 1
fi

while [  $# -gt 0 ]; do
    case $1 in
    --source ) shift
                  export SOURCE=$1
                  ;;
       --build )  shift
                  export BUILD="$1/BUILD"
                  ;;
       --target )  shift
                  export TARGET=$1
                  ;;
       --suite )  shift
                  export SUITE=$1
                  ;;
       --toolchain )  shift
                  export TOOLCHAIN=$1
                  ;;
       --cpu_arch )  shift
                  export CPU_ARCH=$1
                  ;;
       --verbose )  shift
                  export VERBOSE=$1
                  ;;
       --archive_tests )
                  export TEST_COMBINE_ARCHIVE=1
                  ;;
       --include_panic_tests )
                  export INCLUDE_PANIC_TESTS=1
                  ;;
       --include )  shift
                  export INCLUDE="$INCLUDE -I $1/"
                  INCLUDE_PATHS=("${INCLUDE_PATHS[@]}" $1)
                  ;;
       --help | -h )
                  echo "$HELP"
                  exit 1
                  ;;
               * )
                  echo "Error: Invaid argument $1"
                  echo "$HELP"
                  exit 1
        esac
        shift
done

echo "----------Process input arguments- start-------------"
if [ -z "$SOURCE" ]
then
   export SOURCE=./
   echo "--source option is not provided, hence setting \$SOURCE to present dir"
else
   echo "setting \$SOURCE to $SOURCE"
fi

if [ ! -d "$SOURCE/dev_apis" ] || [ ! -d "$SOURCE/ff" ]
then
   echo "Error: Could not find architecture test suite directories in current path $SOURCE"
   exit 1
fi

if [ -z "$BUILD" ]
then
   export BUILD="./BUILD"
   echo "--build option is not provided, hence setting \$BUILD to ./BUILD"
else
   echo "setting \$BUILD to $BUILD"
fi

if [ -z "$TARGET" ]
then
   echo "Provide target string as argument using --target <string>"
   exit 1
else
   echo "Using \$TARGET=$TARGET"
fi

if [ "$SUITE" != "ipc" ] && [ "$SUITE" != "crypto" ] &&  [ "$SUITE" != "protected_storage" ] &&
   [ "$SUITE" != "internal_trusted_storage" ] &&  [ "$SUITE" != "initial_attestation" ]
then
   echo "Error: Unsupported value for --suite=$SUITE.
   Refer help message to see supported suites"
   exit 1
fi

PLATFORM_MAKEFILE=$SOURCE/platform/targets/$TARGET/Makefile
PSA_IPC_IMPLEMENTED=`grep -c "^ *PSA_IPC_IMPLEMENTED\s*:=\s*1" $PLATFORM_MAKEFILE`
PSA_CRYPTO_IMPLEMENTED=`grep -c "^ *PSA_CRYPTO_IMPLEMENTED\s*:=\s*1" $PLATFORM_MAKEFILE`
PSA_PROTECTED_STORAGE_IMPLEMENTED=`grep -c "^ *PSA_PROTECTED_STORAGE_IMPLEMENTED\s*:=\s*1" $PLATFORM_MAKEFILE`
PSA_INTERNAL_TRUSTED_STORAGE_IMPLEMENTED=`grep -c "^ *PSA_INTERNAL_TRUSTED_STORAGE_IMPLEMENTED\s*:=\s*1" $PLATFORM_MAKEFILE`
PSA_INITIAL_ATTESTATION_IMPLEMENTED=`grep -c "^ *PSA_INITIAL_ATTESTATION_IMPLEMENTED\s*:=\s*1" $PLATFORM_MAKEFILE`
WATCHDOG_AVAILABLE=`grep -c "^ *watchdog.num\s*=\s*1\s*;" $SOURCE/platform/targets/$TARGET/target.cfg`
SP_HEAP_MEM_SUPP=`grep -c "^ *dut.0.sp_heap_mem_supp\s*=\s*AVAILABLE\s*;" $SOURCE/platform/targets/$TARGET/target.cfg`

# Check PSA_IPC_IMPLEMENTED validity
if [ $SUITE == "ipc" ] && [ $PSA_IPC_IMPLEMENTED == "0" ]
then
   echo "Error: PSA_IPC_IMPLEMENTED must be set to 1 for ipc suite
         in $PLATFORM_MAKEFILE"
   exit 1
fi

# Check PSA_CRYPTO_IMPLEMENTED validity
if [ $SUITE == "crypto" ] && [ $PSA_CRYPTO_IMPLEMENTED == "0" ]
then
   echo "Error: PSA_CRYPTO_IMPLEMENTED must be set to 1 for crypto suite
         in $PLATFORM_MAKEFILE"
   exit 1
fi

# Check PSA_PROTECTED_STORAGE_IMPLEMENTED validity
if [ $SUITE == "protected_storage" ] && [ $PSA_PROTECTED_STORAGE_IMPLEMENTED == "0" ]
then
   echo "Error: PSA_PROTECTED_STORAGE_IMPLEMENTED must be set to 1 for protected_storage suite
         in $PLATFORM_MAKEFILE"
   exit 1
fi

# Check PSA_INTERNAL_TRUSTED_STORAGE_IMPLEMENTED validity
if [ $SUITE == "internal_trusted_storage" ] && [ $PSA_INTERNAL_TRUSTED_STORAGE_IMPLEMENTED == "0" ]
then
   echo "Error: PSA_INTERNAL_TRUSTED_STORAGE_IMPLEMENTED must be set to 1 for internal_trusted_storage suite
         in $PLATFORM_MAKEFILE"
   exit 1
fi

# Check PSA_INITIAL_ATTESTATION_IMPLEMENTED validity
if [ $SUITE == "initial_attestation" ] && [ $PSA_INITIAL_ATTESTATION_IMPLEMENTED == "0" ]
then
   echo "Error: PSA_INITIAL_ATTESTATION_IMPLEMENTED must be set to 1 for initial_attestation suite
         in $PLATFORM_MAKEFILE"
   exit 1
fi

if [ $PSA_IPC_IMPLEMENTED == "1" ]
then
    # Check --include validity for ipc suite
    if [ -z "$INCLUDE" ]
    then
          echo "Error: --include option is not provided. $IPC_HEADER_FILE_REQ"
          exit 1
    else
        for path in "${INCLUDE_PATHS[@]}"
        do
            if [ -f "$path/psa/client.h" ]
            then
                export CLIENT_FILE_FOUND=1
            fi
            if [ -f "$path/psa/service.h" ]
            then
                export SERVICE_FILE_FOUND=1
            fi
            if [ -f "$path/psa_manifest/sid.h" ] && [ -f "$path/psa_manifest/pid.h" ]
            then
                export MANIFEST_OUT_FILE_FOUND=1
            fi
            if [ -f "$path/psa/lifecycle.h" ]
            then
                export LIFECYCLE_FILE_FOUND=1
            fi
        done
        if [ $CLIENT_FILE_FOUND ==  "0" ]
        then
            echo "Couldn't find psa/client.h file in paths: ${INCLUDE_PATHS[@]}"
            echo "$IPC_HEADER_FILE_REQ"
            exit 1
        fi
        if [ $SERVICE_FILE_FOUND == "0" ]
        then
            echo "Couldn't find psa/service.h file in paths: ${INCLUDE_PATHS[@]}"
            echo "$IPC_HEADER_FILE_REQ"
            exit 1
        fi
        if [ $MANIFEST_OUT_FILE_FOUND == "0" ]
        then
            echo "Couldn't find psa_manifest/sid.h or psa_manifest/pid.h file in paths: ${INCLUDE_PATHS[@]}"
            echo "$IPC_HEADER_FILE_REQ"
            exit 1
        fi
        if [ $LIFECYCLE_FILE_FOUND ==  "0" ]
        then
            echo "Couldn't find psa/lifecycle.h file in paths: ${INCLUDE_PATHS[@]}"
            echo "$IPC_HEADER_FILE_REQ"
            exit 1
        fi
    fi
fi

if [ $PSA_CRYPTO_IMPLEMENTED == "1" ]
then
    # Check --include validity for crypto suite
    if [ -z "$INCLUDE" ]
    then
          echo "Error: --include option is not provided."
          echo "$CRYPTO_HEADER_FILE_REQ"
          exit 1
    else
        for path in "${INCLUDE_PATHS[@]}"
        do
            if [ -f "$path/psa/crypto.h" ]
            then
                export CRYPTO_FILE_FOUND=1
            fi
        done
        if [ $CRYPTO_FILE_FOUND ==  "0" ]
        then
            echo "Couldn't find psa/crypto.h file in paths: ${INCLUDE_PATHS[@]}"
            echo "$CRYPTO_HEADER_FILE_REQ"
            exit 1
        fi
    fi
fi

if [ $PSA_PROTECTED_STORAGE_IMPLEMENTED == "1" ]
then
    # Check --include validity for protected storage suite
    if [ -z "$INCLUDE" ]
    then
          echo "Error: --include option is not provided."
          echo "$PS_HEADER_FILE_REQ"
          exit 1
    else
        for path in "${INCLUDE_PATHS[@]}"
        do
            if [ -f "$path/psa/protected_storage.h" ]
            then
                export PROTECTED_STORAGE_FILE_FOUND=1
            fi
        done
        if [ $PROTECTED_STORAGE_FILE_FOUND ==  "0" ]
        then
            echo "Couldn't find psa/protected_storage.h file in paths: ${INCLUDE_PATHS[@]}"
            echo "$PS_HEADER_FILE_REQ"
            exit 1
        fi
    fi
fi

if [ $PSA_INTERNAL_TRUSTED_STORAGE_IMPLEMENTED == "1" ]
then
    # Check --include validity for internal trusted storage suite
    if [ -z "$INCLUDE" ]
    then
          echo "Error: --include option is not provided."
          echo "$ITS_HEADER_FILE_REQ"
          exit 1
    else
        for path in "${INCLUDE_PATHS[@]}"
        do
            if [ -f "$path/psa/internal_trusted_storage.h" ]
            then
                export INTERNAL_TRUSTED_STORAGE_FILE_FOUND=1
            fi
        done
        if [ $INTERNAL_TRUSTED_STORAGE_FILE_FOUND ==  "0" ]
        then
            echo "Couldn't find psa/internal_trusted_storage.h file in paths: ${INCLUDE_PATHS[@]}"
            echo "$ITS_HEADER_FILE_REQ"
            exit 1
        fi
    fi
fi

if [ $PSA_INITIAL_ATTESTATION_IMPLEMENTED == "1" ]
then
    # Check --include validity for initial_attestation suite
    if [ -z "$INCLUDE" ]
    then
          echo "Error: --include option is not provided."
          echo "$ATTESTATION_HEADER_FILE_REQ"
          exit 1
    else
        for path in "${INCLUDE_PATHS[@]}"
        do
            if [ -f "$path/psa/initial_attestation.h" ]
            then
                export INITIAL_ATTESTATION_FILE_FOUND=1
            fi
            if [ -f "$path/psa/crypto.h" ]
            then
                export CRYPTO_FILE_FOUND=1
            fi
        done

        if [ $INITIAL_ATTESTATION_FILE_FOUND ==  "0" ]
        then
            echo "Couldn't find psa/initial_attestation.h file in paths: ${INCLUDE_PATHS[@]}"
            echo "$ATTESTATION_HEADER_FILE_REQ"
            exit 1
        elif [ $INITIAL_ATTESTATION_FILE_FOUND ==  "1" ]
        then
            if [ $CRYPTO_FILE_FOUND ==  "0" ]
            then
                echo "Couldn't find psa/crypto.h file in paths: ${INCLUDE_PATHS[@]}"
                echo "$CRYPTO_HEADER_FILE_REQ"
                exit 1
            fi
            if [ ! -d "$SOURCE/platform/targets/$TARGET/nspe/initial_attestation/ext" ]
            then
                git clone https://github.com/laurencelundblade/QCBOR.git $SOURCE/platform/targets/$TARGET/nspe/initial_attestation/ext
                cd $SOURCE/platform/targets/$TARGET/nspe/initial_attestation/ext; git checkout da53227db1488dde0952bdff66c3d904dce270b3 ; cd -
            else
                echo "QCBOR library already cloned"
            fi
        fi
    fi
fi

if [ -z "$TOOLCHAIN" ]
then
   export TOOLCHAIN=GNUARM
   echo "--toolchain option is not provided, hence using default value \$TOOLCHAIN=$TOOLCHAIN"
else
   echo "Using \$TOOLCHAIN=$TOOLCHAIN"
fi

if [ $TOOLCHAIN != "GNUARM" ] && [ $TOOLCHAIN != "ARMCLANG" ]
then
   echo "Error: Unsupported value for --toolchain=$TOOLCHAIN.
   Supported toolchain are GNUARM and ARMCLANG"
   exit 1
fi

if [ -z "$CPU_ARCH" ]
then
   echo "Error: Provide cpu arch string as argument using --cpu_arch <string>"
   echo "Supported CPU arch are armv8m_ml, armv8m_bl, armv7m"
   exit 1
else
   echo "Using \$CPU_ARCH=$CPU_ARCH"
fi

if [ $CPU_ARCH != "armv8m_ml" ] && [ $CPU_ARCH != "armv8m_bl" ] && [ $CPU_ARCH != "armv7m" ]
then
   echo "Error: Unsupported value for --cpu_arch=$CPU_ARCH.
   Supported CPU arch are armv8m_ml, armv8m_bl, armv7m"
   exit 1
fi

if [ ! -z "$VERBOSE" ]
then
    if [ $VERBOSE != "1" ] && [ $VERBOSE != "2" ] &&
       [ $VERBOSE != "3" ] && [ $VERBOSE != "4" ] &&
       [ $VERBOSE != "5" ]
    then
        echo "Error: Unsupported value for --verbose=$VERBOSE."
        echo "Supported print levels are:"
        echo "1 - INFO & above."
        echo "2 - DEBUG & above."
        echo "3 - TEST & above."
        echo "4 - WARN & ERROR."
        echo "5 - ERROR."
        exit 1
    fi
else
    export VERBOSE=3
fi

if [ $INCLUDE_PANIC_TESTS == "1" ] && [ $WATCHDOG_AVAILABLE == "0" ]
then
   echo "
Warning: You have set watchdog.num to 0 in $SOURCE/platform/targets/$TARGET/target.cfg
Note - To test PSA APIs panic conditions, test harness may require to access watchdog timer
to recover from panic and to be able to continue with next test.
Ignore this warning if system under test has capability to reset the system
when it encounters panic condition.
"
fi

echo "----------Process input arguments- complete-------------"
echo ""

MAKE_OPTIONS=" SOURCE=$SOURCE "
MAKE_OPTIONS+=" BUILD=$BUILD "
MAKE_OPTIONS+=" TARGET=$TARGET "
MAKE_OPTIONS+=" SUITE=$SUITE "
MAKE_OPTIONS+=" TOOLCHAIN=$TOOLCHAIN "
MAKE_OPTIONS+=" CPU_ARCH=$CPU_ARCH "
MAKE_OPTIONS+=" VERBOSE=$VERBOSE "
MAKE_OPTIONS+=" TEST_COMBINE_ARCHIVE=$TEST_COMBINE_ARCHIVE "
MAKE_OPTIONS+=" INCLUDE_PANIC_TESTS=$INCLUDE_PANIC_TESTS "
MAKE_OPTIONS+=" WATCHDOG_AVAILABLE=$WATCHDOG_AVAILABLE "
MAKE_OPTIONS+=" SP_HEAP_MEM_SUPP=$SP_HEAP_MEM_SUPP "
MAKE_OPTIONS+=" PSA_IPC_IMPLEMENTED=$PSA_IPC_IMPLEMENTED "
MAKE_OPTIONS+=" PSA_CRYPTO_IMPLEMENTED=$PSA_CRYPTO_IMPLEMENTED "
MAKE_OPTIONS+=" PSA_PROTECTED_STORAGE_IMPLEMENTED=$PSA_PROTECTED_STORAGE_IMPLEMENTED "
MAKE_OPTIONS+=" PSA_INTERNAL_TRUSTED_STORAGE_IMPLEMENTED=$PSA_INTERNAL_TRUSTED_STORAGE_IMPLEMENTED "
MAKE_OPTIONS+=" PSA_INITIAL_ATTESTATION_IMPLEMENTED=$PSA_INITIAL_ATTESTATION_IMPLEMENTED "
MAKE_OPTIONS+=" USER_INCLUDE=\"$INCLUDE\" "

#Build VAL/PAL static library and Tests ELFs
echo "make -f $SOURCE/tools/makefiles/Makefile $MAKE_OPTIONS USER_INCLUDE=\"$INCLUDE\" all "
make -f $SOURCE/tools/makefiles/Makefile $MAKE_OPTIONS USER_INCLUDE="$INCLUDE" all
