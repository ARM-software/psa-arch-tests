#/** @file
# * Copyright (c) 2019-2020, Arm Limited or its affiliates. All rights reserved.
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

# Set the minimum required version of CMake for the project
cmake_minimum_required(VERSION 3.10)

# cmake_policy
cmake_policy(SET CMP0057 NEW)

# Find python interpreter version 3 or greater
find_package(PythonInterp 3 REQUIRED)
# Find Git package
find_package(Git REQUIRED)

get_filename_component(PSA_ROOT_DIR . ABSOLUTE)

list(APPEND CMAKE_MODULE_PATH ${PSA_ROOT_DIR}/tools/cmake)
include("common/Utils")
include(${PSA_ROOT_DIR}/tools/cmake/common/CMakeSettings.cmake)
include(${PSA_ROOT_DIR}/tools/cmake/common/CMakeExternal.cmake)

# list of supported suites
list(APPEND PSA_SUITES
	"IPC"
	"CRYPTO"
	"PROTECTED_STORAGE"
	"INTERNAL_TRUSTED_STORAGE"
	"INITIAL_ATTESTATION"
)

# list of ipc files required
list(APPEND PSA_IPC_FILES
	"psa/client.h"
	"psa/service.h"
	"psa/lifecycle.h"
	"psa_manifest/sid.h"
	"psa_manifest/pid.h"
	"psa_manifest/driver_partition_psa.h"
	"psa_manifest/client_partition_psa.h"
	"psa_manifest/server_partition_psa.h"
)

# list of crypto files required
list(APPEND PSA_CRYPTO_FILES
	"psa/crypto.h"
)

# list of protected_storage files required
list(APPEND PSA_PROTECTED_STORAGE_FILES
	"psa/protected_storage.h"
)

# list of internal_trusted_storage files required
list(APPEND PSA_INTERNAL_TRUSTED_STORAGE_FILES
	"psa/internal_trusted_storage.h"
)

# list of initial_attestation files required
list(APPEND PSA_INITIAL_ATTESTATION_FILES
	"psa/initial_attestation.h"
	"psa/crypto.h"
)

# list of supported toolchains
list(APPEND PSA_TOOLCHAIN_SUPPORT
        GNUARM
        ARMCLANG
        HOST_GCC
)

# list of suported CPU arch
list(APPEND PSA_CPU_ARCH_SUPPORT
	armv8m_ml
	armv8m_bl
	armv7m
)

# list of VERBOSE options
list(APPEND PSA_VERBOSE_OPTIONS 1 2 3 4 5)

# list of PLATFORM_PSA_ISOLATION_LEVEL options
list(APPEND PLATFORM_PSA_ISOLATION_LEVEL_OPTIONS 1 2 3)

message(STATUS "[PSA] : ----------Process input arguments- start-------------")

# Check for TARGET command line argument
_check_arguments("TARGET")
# Check for SUTIE command line argument
_check_arguments("SUITE")
# Check for PSA_INCLUDE_PATHS command line argument
_check_arguments("PSA_INCLUDE_PATHS")

string(TOLOWER ${SUITE} SUITE_LOWER)

# Check for valid targets
_get_sub_dir_list(PSA_TARGET_LIST ${PSA_ROOT_DIR}/platform/targets)
if(NOT ${TARGET} IN_LIST PSA_TARGET_LIST)
	message(FATAL_ERROR "[PSA] : Error: Unspported value for -DTARGET=, supported targets are : ${PSA_TARGET_LIST}")
else()
    message(STATUS "[PSA] : TARGET is set to ${TARGET}")
endif()

# Check for the presence of required test suite directories
if((NOT IS_DIRECTORY ${PSA_ROOT_DIR}/dev_apis) OR (NOT IS_DIRECTORY ${PSA_ROOT_DIR}/ff))
	message(STATUS "[PSA] : Error: Could not find architecture test suite directories in psa root path ${PSA_ROOT_DIR}")
endif()

if(FALSE)
# Check for build directory specified
if(NOT DEFINED BUILD)
	set(BUILD ${CMAKE_CURRENT_BINARY_DIR}/BUILD CACHE INTERNAL "Defaulting build directory to ${BUILD}" FORCE)
else()
	set(BUILD ${CMAKE_CURRENT_BINARY_DIR}/${BUILD}/BUILD CACHE INTERNAL "Defaulting build directory to ${BUILD}" FORCE)
endif()
endif()

# Check for valid suite cmake argument passed
if(NOT ${SUITE} IN_LIST PSA_SUITES)
	message(FATAL_ERROR "[PSA] : Error: Unsupported value for -DSUITE=, select one from supported suites which are : ${PSA_SUITES}")
else()
    message(STATUS "[PSA] : SUITE is set to ${SUITE}")
endif()

# Project variables
set(PSA_TARGET_PRE_BUILD		psa_pre_build)
set(PSA_TARGET_GENERATE_DATABASE_PRE	psa_generate_database_prerequisite)
set(PSA_TARGET_GENERATE_DATABASE	psa_generate_database)
set(PSA_TARGET_GENERATE_DATABASE_POST	psa_generate_database_cleanup)
if(${SUITE} STREQUAL "INITIAL_ATTESTATION")
	set(PSA_TARGET_QCBOR		psa_qcbor)
endif()
set(PSA_TARGET_PAL_NSPE_LIB		pal_nspe)
set(PSA_TARGET_VAL_NSPE_LIB		val_nspe)
set(PSA_TARGET_TEST_COMBINE_LIB		test_combine)
set(PSA_TARGET_DRIVER_PARTITION_LIB	driver_partition)
set(PSA_TARGET_CLIENT_PARTITION_LIB	client_partition)
set(PSA_TARGET_SERVER_PARTITION_LIB	server_partition)
if(${SUITE} STREQUAL "IPC")
	set(PSA_SUITE_DIR		${PSA_ROOT_DIR}/ff/${SUITE_LOWER})
	set(PSA_SUITE_OUT_DIR		${CMAKE_CURRENT_BINARY_DIR}/ff/${SUITE_LOWER})
else()
	set(PSA_SUITE_DIR		${PSA_ROOT_DIR}/dev_apis/${SUITE_LOWER})
	set(PSA_SUITE_OUT_DIR		${CMAKE_CURRENT_BINARY_DIR}/dev_apis/${SUITE_LOWER})
endif()
set(PSA_TARGET_CONFIG_HEADER_GENERATOR	${PSA_ROOT_DIR}/tools/scripts/target_cfg/targetConfigGen.py)
set(PSA_TESTLIST_GENERATOR		${PSA_ROOT_DIR}/tools/scripts/gen_tests_list.py)
set(TARGET_CONFIGURATION_FILE		${PSA_ROOT_DIR}/platform/targets/${TARGET}/target.cfg)
set(TGT_CONFIG_SOURCE_C			${CMAKE_CURRENT_BINARY_DIR}/targetConfigGen.c)
set(OUTPUT_HEADER			target_database.h)
set(DATABASE_TABLE_NAME			target_database)
set(DATABASE_TABLE_SECTION_NAME		"NOSECTION")
set(TARGET_HEADER_GEN_INCLUDE_PATHS     "${PSA_ROOT_DIR}/val/nspe|${PSA_ROOT_DIR}/val/common|${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe/common|${PSA_ROOT_DIR}/platform/targets/${TARGET}/nspe/crypto")
set(TESTSUITE_DB			${PSA_SUITE_DIR}/testsuite.db)
set(PSA_TESTLIST_FILE			${CMAKE_CURRENT_BINARY_DIR}/testlist.txt)
set(PSA_TEST_ENTRY_LIST_INC		${CMAKE_CURRENT_BINARY_DIR}/test_entry_list.inc)
set(PSA_TEST_ENTRY_FUN_DECLARE_INC	${CMAKE_CURRENT_BINARY_DIR}/test_entry_fn_declare_list.inc)
set(PSA_CLIENT_TEST_LIST_DELCARE_INC	${CMAKE_CURRENT_BINARY_DIR}/client_tests_list_declare.inc)
set(PSA_CLIENT_TEST_LIST_INC		${CMAKE_CURRENT_BINARY_DIR}/client_tests_list.inc)
set(PSA_SERVER_TEST_LIST_DECLARE_INC	${CMAKE_CURRENT_BINARY_DIR}/server_tests_list_declare.inc)
set(PSA_SERVER_TEST_LIST		${CMAKE_CURRENT_BINARY_DIR}/server_tests_list.inc)
if(${SUITE} STREQUAL "INITIAL_ATTESTATION")
	set(PSA_QCBOR_INCLUDE_PATH      ${CMAKE_CURRENT_BINARY_DIR}/${PSA_TARGET_QCBOR}/inc)
endif()

# Validity check for required files for a given suite
if(NOT DEFINED PSA_${SUITE}_FILES)
	message(FATAL_ERROR "[PSA] : List of file/s to verify against ${suite} is not defined")
endif()
foreach(file_item ${PSA_${SUITE}_FILES})
	set(PSA_FILE_FOUND FALSE)
	foreach(include_path ${PSA_INCLUDE_PATHS})
		if((EXISTS ${include_path}/${file_item}) AND
		   (NOT PSA_FILE_FOUND))
			set(PSA_FILE_FOUND TRUE)
			break()
		endif()
	endforeach()
	if(NOT PSA_FILE_FOUND)
		message(FATAL_ERROR "[PSA] : Couldn't find ${file_item} in ${PSA_INCLUDE_PATHS}")
	endif()
endforeach()

# Check for TOOLCHAIN command line argument
if(NOT DEFINED TOOLCHAIN)
        set(TOOLCHAIN "GNUARM" CACHE INTERNAL "Compiler used" FORCE)
        message(STATUS "[PSA] : Defaulting compiler to ${TOOLCHAIN}")
else()
        message(STATUS "[PSA] : TOOLCHAIN is set to ${TOOLCHAIN}")
endif()

if(${TOOLCHAIN} STREQUAL "ARMCLANG" OR ${TOOLCHAIN} STREQUAL "GNUARM")
    if(NOT DEFINED CPU_ARCH)
	    message(FATAL_ERROR "[PSA] : Error: -DCPU_ARCH option missing")
    else()
        # Check for CPU architecture
        if(NOT ${CPU_ARCH} IN_LIST PSA_CPU_ARCH_SUPPORT)
	        message(FATAL_ERROR "[PSA] : Error: Unsupported value for -DCPU_ARCH=, supported CPU arch are : ${PSA_CPU_ARCH_SUPPORT}")
        endif()
    endif()
    message(STATUS "[PSA] : CPU_ARCH is set to ${CPU_ARCH}")
endif()

# Check for VERBOSE
if(NOT DEFINED VERBOSE)
	set(VERBOSE 3 CACHE INTERNAL "Default VERBOSE value" FORCE)
        message(STATUS "[PSA] : Defaulting VERBOSE to ${VERBOSE}")
else()
	if(NOT ${VERBOSE} IN_LIST PSA_VERBOSE_OPTIONS)
		message(FATAL_ERROR "[PSA] : Error: Unspported value for -DVERBOSE=, supported values are : ${PSA_VERBOSE_OPTIONS}")
	endif()
    message(STATUS "[PSA] : VERBOSE is set to ${VERBOSE}")
endif()

# Check for PLATFORM_PSA_ISOLATION_LEVEL
if(NOT DEFINED PLATFORM_PSA_ISOLATION_LEVEL)
	set(PLATFORM_PSA_ISOLATION_LEVEL 3 CACHE INTERNAL "Default PLATFORM_PSA_ISOLATION_LEVEL value" FORCE)
	if(${SUITE} STREQUAL "IPC")
        message(STATUS "[PSA] : Defaulting PLATFORM_PSA_ISOLATION_LEVEL to ${PLATFORM_PSA_ISOLATION_LEVEL}")
	endif()
else()
	if(NOT ${PLATFORM_PSA_ISOLATION_LEVEL} IN_LIST PLATFORM_PSA_ISOLATION_LEVEL_OPTIONS)
		message(FATAL_ERROR "[PSA] : Error: Unspported value for -DPLATFORM_PSA_ISOLATION_LEVEL=, supported values are : ${PLATFORM_PSA_ISOLATION_LEVEL_OPTIONS}")
	endif()
	if(${SUITE} STREQUAL "IPC")
		message(STATUS "[PSA] : PLATFORM_PSA_ISOLATION_LEVEL is set to ${PLATFORM_PSA_ISOLATION_LEVEL}")
	endif()
endif()

if(NOT DEFINED INCLUDE_PANIC_TESTS)
	#By default panic tests are disabled
	set(INCLUDE_PANIC_TESTS 0 CACHE INTERNAL "Default INCLUDE_PANIC_TESTS value" FORCE)
        message(STATUS "[PSA] : Defaulting INCLUDE_PANIC_TESTS to ${INCLUDE_PANIC_TESTS}")
else()
	if(INCLUDE_PANIC_TESTS EQUAL 1)
		message(STATUS "[PSA] : "
		"INCLUDE_PANIC_TESTS set to 1, therefore including PSA APIs panic tests into the regression,\n"
		"\tensure that watchdog.num is set to 1 in ${PSA_ROOT_DIR}/platform/targets/${TARGET}/target.cfg")
	endif()
endif()

if(NOT DEFINED WATCHDOG_AVAILABLE)
	#Assuming watchdog is available to program by test suite
	set(WATCHDOG_AVAILABLE	1 CACHE INTERNAL "Default WATCHDOG_AVAILABLE value" FORCE)
        message(STATUS "[PSA] : Defaulting WATCHDOG_AVAILABLE to ${WATCHDOG_AVAILABLE}")
else()
    message(STATUS "[PSA] : WATCHDOG_AVAILABLE is set to ${WATCHDOG_AVAILABLE}")
endif()

if((INCLUDE_PANIC_TESTS EQUAL 1) AND
   (WATCHDOG_AVAILABLE EQUAL 0))
	message(WARNING "[PSA]: "
	     "Note that to test PSA APIs panic conditions, test harness may require to access"
		 "the watchdog timer in oder to recover from panic and to be able to continue with"
		 "next test. Ignore this warning if system under test has capability to reset the"
		 "system when it encounters panic condition.")
endif()

if(NOT DEFINED SP_HEAP_MEM_SUPP)
	#Are dynamic memory functions available to secure partition?
	set(SP_HEAP_MEM_SUPP 1 CACHE INTERNAL "Default SP_HEAP_MEM_SUPP value" FORCE)
        message(STATUS "[PSA] : Defaulting SP_HEAP_MEM_SUPP to ${SP_HEAP_MEM_SUPP}")
endif()

if(NOT DEFINED TEST_COMBINE_ARCHIVE)
	#Default option is to create archive
	set(TEST_COMBINE_ARCHIVE 1 CACHE INTERNAL "Default TEST_COMBINE_ARCHIVE value" FORCE)
        message(STATUS "[PSA] : Defaulting TEST_COMBINE_ARCHIVE to ${TEST_COMBINE_ARCHIVE}")
else()
    message(STATUS "[PSA] : TEST_COMBINE_ARCHIVE is set to ${TEST_COMBINE_ARCHIVE}")
endif()

message(STATUS "[PSA] : ----------Process input arguments- complete-------------")


if((${SUITE} STREQUAL "INITIAL_ATTESTATION") AND (NOT EXISTS ${PSA_TARGET_QCBOR}))
# Clone QCBOR and move to specified tag
execute_process(COMMAND ${GIT_EXECUTABLE} clone ${PSA_QCBOR_GIT_REPO_LINK} ${PSA_TARGET_QCBOR}
	RESULT_VARIABLE ${PSA_TARGET_QCBOR}_clone_result
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
if(${PSA_TARGET_QCBOR}_clone_result)
	message(FATAL_ERROR "git clone failed for ${PSA_QCBOR_GIT_REPO_LINK}")
endif()

if(NOT ${PSA_TARGET_QCBOR}_clone_result)
execute_process(COMMAND ${GIT_EXECUTABLE} checkout -q "${PSA_QCBOR_GIT_REPO_TAG}"
	RESULT_VARIABLE ${PSA_TARGET_QCBOR}_checkout_result
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${PSA_TARGET_QCBOR})
if(${PSA_TARGET_QCBOR}_checkout_result)
	message(FATAL_ERROR "git checkout failed for Repo : ${PSA_QCBOR_GIT_REPO_LINK}, Tag : ${PSA_QCBOR_GIT_REPO_TAG}")
endif()
endif()
endif()

# Create PSA clean list
list(APPEND PSA_CLEAN_LIST
        ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_HEADER}
	${PSA_TESTLIST_FILE}
	${PSA_TEST_ENTRY_LIST_INC}
	${PSA_TEST_ENTRY_FUN_DECLARE_INC}
	${PSA_CLIENT_TEST_LIST_DELCARE_INC}
	${PSA_CLIENT_TEST_LIST_INC}
	${PSA_SERVER_TEST_LIST_DECLARE_INC}
	${PSA_SERVER_TEST_LIST}
)

# Process testsuite.db
message(STATUS "[PSA] : Creating testlist.txt 'available at ${PSA_TESTLIST_FILE}'")
execute_process(COMMAND ${PYTHON_EXECUTABLE} ${PSA_TESTLIST_GENERATOR}
					${SUITE_LOWER}
					${TESTSUITE_DB}
					${INCLUDE_PANIC_TESTS}
					${PSA_TESTLIST_FILE}
					${PSA_TEST_ENTRY_LIST_INC}
					${PSA_TEST_ENTRY_FUN_DECLARE_INC}
					${PSA_CLIENT_TEST_LIST_DELCARE_INC}
					${PSA_CLIENT_TEST_LIST_INC}
					${PSA_SERVER_TEST_LIST_DECLARE_INC}
					${PSA_SERVER_TEST_LIST})

# Creating CMake list variable from file
file(READ ${PSA_TESTLIST_FILE} PSA_TEST_LIST)
string(REGEX REPLACE "\n" ";" PSA_TEST_LIST "${PSA_TEST_LIST}")

add_custom_target(
	${PSA_TARGET_GENERATE_DATABASE_PRE}
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/platform
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/val
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/partition
	COMMAND ${CMAKE_COMMAND} -E make_directory ${PSA_SUITE_OUT_DIR}
)

# Generate target files from User provided data base
include(ExternalProject)
ExternalProject_Add(
        ${PSA_TARGET_GENERATE_DATABASE}
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}
        DOWNLOAD_COMMAND ""
        UPDATE_COMMAND ""
        PATCH_COMMAND ""
        BUILD_COMMAND ""
        SOURCE_DIR "${PSA_ROOT_DIR}/tools/scripts/target_cfg"
	CMAKE_ARGS -DPYTHON_EXECUTABLE=${PYTHON_EXECUTABLE}
		-DOUT_DIR=${CMAKE_CURRENT_BINARY_DIR}
		-DTARGET=${TARGET}
		-DGENERATOR_FILE=${PSA_TARGET_CONFIG_HEADER_GENERATOR}
		-DINCLUDE_DIR=${PSA_ROOT_DIR}/val/common
		-DTARGET_CONFIGURATION_FILE=${TARGET_CONFIGURATION_FILE}
		-DTGT_CONFIG_SOURCE_C=${TGT_CONFIG_SOURCE_C}
		-DOUTPUT_HEADER=${OUTPUT_HEADER}
		-DDATABASE_TABLE_NAME=${DATABASE_TABLE_NAME}
		-DDATABASE_TABLE_SECTION_NAME=${DATABASE_TABLE_SECTION_NAME}
		-DTARGET_HEADER_GEN_INCLUDE_PATHS=${TARGET_HEADER_GEN_INCLUDE_PATHS}
	LIST_SEPARATOR |
        TEST_COMMAND ""
)

# Add custom target to clean generated files of the external project
add_custom_target(
        ${PSA_TARGET_GENERATE_DATABASE_POST}
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/src/${PSA_TARGET_GENERATE_DATABASE}-build/ -- clean
)

# Check for supported toolchain/s
if(${TOOLCHAIN} IN_LIST PSA_TOOLCHAIN_SUPPORT)
        include(${PSA_ROOT_DIR}/tools/cmake/compiler/${TOOLCHAIN}.cmake)
else()
        message(FATAL_ERROR "[PSA] : Error: Unspported value for -DTOOLCHAIN=, supported toolchain are : ${PSA_TOOLCHAIN_SUPPORT}")
endif()

# Global macro to identify the PSA test suite cmake build
add_definitions(-DPSA_CMAKE_BUILD)
add_definitions(-D${SUITE})
add_definitions(-DVERBOSE=${VERBOSE})
add_definitions(-DPLATFORM_PSA_ISOLATION_LEVEL=${PLATFORM_PSA_ISOLATION_LEVEL})

if(${SP_HEAP_MEM_SUPP} EQUAL 1)
	add_definitions(-DSP_HEAP_MEM_SUPP)
endif()

# Build PAL NSPE LIB
include(${PSA_ROOT_DIR}/platform/targets/${TARGET}/target.cmake)
# Build VAL NSPE LIB
#add_definitions(-DVAL_NSPE_BUILD)
include(${PSA_ROOT_DIR}/val/val_nspe.cmake)
# Build test
include(${PSA_SUITE_DIR}/suite.cmake)
if(${SUITE} STREQUAL "IPC")
# Build SPE LIB
include(${PSA_ROOT_DIR}/val/val_spe.cmake)
endif()

add_dependencies(${PSA_TARGET_GENERATE_DATABASE}	${PSA_TARGET_GENERATE_DATABASE_PRE})
add_dependencies(${PSA_TARGET_GENERATE_DATABASE_POST}	${PSA_TARGET_GENERATE_DATABASE})
add_dependencies(${PSA_TARGET_PAL_NSPE_LIB} 		${PSA_TARGET_GENERATE_DATABASE_POST})
add_dependencies(${PSA_TARGET_VAL_NSPE_LIB}		${PSA_TARGET_PAL_NSPE_LIB})
add_dependencies(${PSA_TARGET_TEST_COMBINE_LIB}		${PSA_TARGET_VAL_NSPE_LIB})
if(${SUITE} STREQUAL "IPC")
add_dependencies(${PSA_TARGET_DRIVER_PARTITION_LIB}	${PSA_TARGET_TEST_COMBINE_LIB})
add_dependencies(${PSA_TARGET_CLIENT_PARTITION_LIB}	${PSA_TARGET_DRIVER_PARTITION_LIB})
add_dependencies(${PSA_TARGET_SERVER_PARTITION_LIB}	${PSA_TARGET_CLIENT_PARTITION_LIB})
endif()

# Include the files for make clean
foreach(clean_item ${PSA_CLEAN_LIST})
        set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${clean_item})
endforeach()

set_property(TARGET ${PSA_TARGET_VAL_NSPE_LIB}          PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/val)
set_property(TARGET ${PSA_TARGET_PAL_NSPE_LIB}          PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/platform)
set_property(TARGET ${PSA_TARGET_TEST_COMBINE_LIB}      PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${PSA_SUITE_OUT_DIR})
if(${SUITE} STREQUAL "IPC")
set_property(TARGET ${PSA_TARGET_DRIVER_PARTITION_LIB}  PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/partition)
set_property(TARGET ${PSA_TARGET_CLIENT_PARTITION_LIB}  PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/partition)
set_property(TARGET ${PSA_TARGET_SERVER_PARTITION_LIB}  PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/partition)
endif()
