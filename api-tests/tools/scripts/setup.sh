#!/usr/bin/env bash

#/** @file
# * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
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
export CLIENT_FILE_FOUND=0
export SERVICE_FILE_FOUND=0
HELP="

Usage: setup.sh [--source SOURCE_DIR] [--build BUILD_DIR] [--target TARGET] [--suite SUITE]
                [--toolchain TOOLCHAIN] [--cpu_arch CPU_ARCH] [--clean] [--verbose PRINT_LEVEL]
                [--include INCLUDE_PATH] [--help|-h]

Arguments Info:
    --source <SOURCE_DIR>   : SOURCE_DIR pointing to architecture test suite directory structure.
                              Default is current directory
    --build  <BUILD_DIR>    : To select the build (output) directory. Default: BUILD/ inside current directory
    --target <TARGET>       : Provide target string as argument.
                              target.cfg file corresponding to input string must be avaiable at
                              platform/targets/<TARGET>/
    --suite <SUITE>         : Compile tests for given suite. Support values are ipc and crypto.
                              Default is ipc suite.
    --toolchain <TOOLCHAIN> : Build using the given TOOLCHAIN. Supported value is GNUARM (GNU Arm Embedded).
                              Future release will be extended to support ARMCLANG (ARM Compiler 6.x).
    --cpu_arch <CPU_ARCH>   : Provide cpu arch string as argument.
                              Supported CPU arch are armv8m_ml, armv8m_bl and armv7m.
    --clean                 : Clean the build directory
    --verbose <PRINT_LEVEL> : Print verbosity level
                              Supported print levels are:
                                1 - INFO & above.
                                2 - DEBUG & above.
                                3 - TEST & above.(Default)
                                4 - WARN & ERROR.
                                5 - ERROR.
    --include <INCLUDE_PATH>: Additional directory to be included into compiler search path. Provide --include <path>
                              where path pointing to location of PSA defined header files.
                              You can specify multiple source locations using --include option.
                              Ex: --include <path1>  --include <path2>
                              Note- If PSA IPC implemented in your platform, include path must point to path
                              where \"psa/client.h\", \"psa/service.h\" and test partition manifest output files
                              (\"psa_manifest/sid.h\" and \"psa_manifest/<manifestfilename>.h\") are located.
    --help|-h               : Print this help message

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
       --clean )
                  export CLEAN=1
                  ;;
       --verbose )  shift
                  export VERBOSE=$1
                  ;;
       --include )  shift
                  export INCLUDE="$INCLUDE -I $1/"
                  INCLUDE_PATHS=("${INCLUDE_PATHS[@]}" $1)
                  ;;
       --help | -h | * )
                  echo "$HELP"
                  exit 1
                  ;;
               * )
                  echo "Error: Invaid argument"
                  echo "$HELP"
                  exit 1
        esac
        shift
done

echo ">>>> Processing inputs..."
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

if [ -z "$SUITE" ]
then
   export SUITE=ipc
   echo "--suite option is not provided, hence using default value \$SUITE=$SUITE"
else
   echo "Using \$SUITE=$SUITE"
fi

PSA_IPC_IMPLEMENTED=`grep -c "^ *PSA_IPC_IMPLEMENTED\s*:=\s*1" $SOURCE/platform/targets/$TARGET/Makefile`
PSA_CRYPTO_IMPLEMENTED=`grep -c "^ *PSA_CRYPTO_IMPLEMENTED\s*:=\s*1" $SOURCE/platform/targets/$TARGET/Makefile`

# Check PSA_IPC_IMPLEMENTED validity
if [ $SUITE == "ipc" ] && [ $PSA_IPC_IMPLEMENTED == "0" ]
then
   echo "Error: PSA_IPC_IMPLEMENTED must be set to 1 for ipc suite
         in $SOURCE/platform/$TARGET/Makefile"
   exit 1
fi

# Check PSA_CRYPTO_IMPLEMENTED validity
if [ $SUITE == "crypto" ] && [ $PSA_CRYPTO_IMPLEMENTED == "0" ]
then
   echo "Error: PSA_CRYPTO_IMPLEMENTED must be set to 1 for crypto suite
         in $SOURCE/platform/$TARGET/Makefile"
   exit 1
fi

if [ $PSA_IPC_IMPLEMENTED == "1" ]
then
    # Check --include validity for ipc suite
    if [ -z "$INCLUDE" ]
    then
          echo "Error: --include option is not provided.
          You must provide PSA defined API element header files "psa/client.h" and "psa/service.h"
          and test partition manifest output files to compile tests and test framework.
          Provide --include <path> where path pointing to location of required files.
          You can specify multiple source locations using --include option.
          Ex: --include <path1>  --include <path2> "
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
        done
        if [ $CLIENT_FILE_FOUND ==  "0" ]
        then
            echo "Couldn't find psa/client.h file in paths: ${INCLUDE_PATHS[@]}"
            exit 1
        fi
        if [ $SERVICE_FILE_FOUND == "0" ]
        then
            echo "Couldn't find psa/service.h file in paths: ${INCLUDE_PATHS[@]}"
            exit 1
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
   echo "Error: Unsupported value for --toolchain=$TOOLCHAIN. Supported toolchain are GNUARM and ARMCLANG"
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
   echo "Error: Unsupported value for --cpu_arch=$CPU_ARCH. Supported CPU arch are armv8m_ml, armv8m_bl, armv7m"
   exit 1
fi

if [ ! -z "$VERBOSE" ]
then
    if [ $VERBOSE != "1" ] && [ $VERBOSE != "2" ] && [ $VERBOSE != "3" ] && [ $VERBOSE != "4" ] && [ $VERBOSE != "5" ]
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

MAKE_OPTIONS=" SOURCE=$SOURCE "
MAKE_OPTIONS+=" BUILD=$BUILD "
MAKE_OPTIONS+=" TARGET=$TARGET "
MAKE_OPTIONS+=" SUITE=$SUITE "
MAKE_OPTIONS+=" TOOLCHAIN=$TOOLCHAIN "
MAKE_OPTIONS+=" CPU_ARCH=$CPU_ARCH "
MAKE_OPTIONS+=" VERBOSE=$VERBOSE "
MAKE_OPTIONS+=" PSA_IPC_IMPLEMENTED=$PSA_IPC_IMPLEMENTED "
MAKE_OPTIONS+=" USER_INCLUDE=\"$INCLUDE\" "

if [ -z "$CLEAN" ]
then
   #Build VAL/PAL static library and Tests ELFs
   echo "make -f $SOURCE/tools/makefiles/Makefile $MAKE_OPTIONS USER_INCLUDE=\"$INCLUDE\" all "
   make -f $SOURCE/tools/makefiles/Makefile $MAKE_OPTIONS USER_INCLUDE="$INCLUDE" all
else
   make -f $SOURCE/tools/makefiles/Makefile $MAKE_OPTIONS USER_INCLUDE="$INCLUDE" clean
fi
