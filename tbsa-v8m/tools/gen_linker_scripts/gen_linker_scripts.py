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

import sys
import os
import shutil
from shutil import copyfile
from linker_address_length import *

num_args = len(sys.argv)
if (num_args != 2):
	print("\nTarget string is required as second argument!")
	sys.exit(1)

tbsa_linker_path  = "../../platform/board/" + sys.argv[1] + "/linker"
tbsa_tgt_cfg_file = "../../platform/board/" + sys.argv[1] + "/tbsa_tgt.cfg"
src_tbsa_template = "./tbsa.linker.template"
src_test_template = "./test.linker.template"
dst_tbsa_linker   = tbsa_linker_path + "/tbsa.linker"
dst_test_s_linker = tbsa_linker_path + "/test_s.linker"
dst_test_ns_linker= tbsa_linker_path + "/test_ns.linker"

s_addr       = ""
ns_addr      = ""
nsc_addr     = ""
s_test_addr  = ""
ns_test_addr = ""

# Check to ensure the presence of target configuration file
try:
	with open(tbsa_tgt_cfg_file) as file:
		pass
except IOError:
	print("The Target Configuration File is not preset for a given target!!!")
	sys.exit(1)

# Create linker directory in the appropriate target path
if os.path.exists(tbsa_linker_path):
	shutil.rmtree(tbsa_linker_path)
os.makedirs(tbsa_linker_path)

# Subroutine which search and replaces the input strings in a given file
def update_addr(file_handle, replace_from, replace_to):
	# Read the file
	with open(file_handle, 'r') as file:
		filedata = file.read()

	# Replace the target string
	filedata = filedata.replace(replace_from, replace_to)

	# Write the file again
	with open(file_handle, 'w') as file:
		file.write(filedata)

# Open target file to read S, NS and NSC address configuration
with open(tbsa_tgt_cfg_file) as tgt_cfg_f:
	for line in tgt_cfg_f:
		if "dut.0.s_addr" in line:
			s_addr += line[(line.find("=") + 1):line.find(";")].strip()
		if "dut.0.ns_addr" in line:
			ns_addr += line[(line.find("=") + 1):line.find(";")].strip()
		if "dut.0.nsc_addr" in line:
			nsc_addr += line[(line.find("=") + 1):line.find(";")].strip()
		if "dut.0.s_test_addr" in line:
			s_test_addr += line[(line.find("=") + 1):line.find(";")].strip()
		if "dut.0.ns_test_addr" in line:
			ns_test_addr += line[(line.find("=") + 1):line.find(";")].strip()

if s_addr is '':
	print("Secure Address is not configured in tbsa_tgt.cfg!!!")
	sys.exit(1)
if ns_addr is '':
	print("Non-secure Address is not configured in tbsa_tgt.cfg!!!")
	sys.exit(1)
if nsc_addr is '':
	print("NSC Address is not configured in tbsa_tgt.cfg!!!")
	sys.exit(1)
if s_test_addr is '':
	print("Secure Test Address is not configured in tbsa_tgt.cfg!!!")
	sys.exit(1)
if ns_test_addr is '':
	print("Non-secure Test Address is not configured in tbsa_tgt.cfg!!!")
	sys.exit(1)

# Create outupt linker scripts from templates
copyfile(src_tbsa_template, dst_tbsa_linker)
copyfile(src_test_template, dst_test_s_linker)
copyfile(src_test_template, dst_test_ns_linker)

# Updating tbsa linker script with address from target configuration file
update_addr(dst_tbsa_linker, "TBSA_TEXT_START", \
                              s_addr)
update_addr(dst_tbsa_linker, "TBSA_TEXT_LENGTH", \
                              TBSA_TEXT_LENGTH)
update_addr(dst_tbsa_linker, "TBSA_DATA_START", \
                              hex(int(s_addr, 16) + \
                              (int(TBSA_TEXT_LENGTH[:TBSA_TEXT_LENGTH.find("K")]) * 1024)))
update_addr(dst_tbsa_linker, "TBSA_DATA_LENGTH", \
                              TBSA_DATA_LENGTH)
update_addr(dst_tbsa_linker, "TBSA_NS_DATA_START", \
                              ns_addr)
update_addr(dst_tbsa_linker, "TBSA_NS_DATA_LENGTH", \
                              TBSA_NS_DATA_LENGTH)
update_addr(dst_tbsa_linker, "TBSA_NS_TEXT_START", \
                              hex(int(ns_addr, 16) + \
                              (int(TBSA_NS_DATA_LENGTH[:TBSA_NS_DATA_LENGTH.find("K")]) * 1024)))
update_addr(dst_tbsa_linker, "TBSA_NS_TEXT_LENGTH", \
                              TBSA_NS_TEXT_LENGTH)
update_addr(dst_tbsa_linker, "TBSA_NSC_TEXT_START", \
                              nsc_addr)
update_addr(dst_tbsa_linker, "TBSA_NSC_TEXT_LENGTH", \
                              TBSA_NSC_TEXT_LENGTH)

# Updating S test linker script with address from target configuration file
update_addr(dst_test_s_linker, "TBSA_TEST_INFO_START", \
                                s_test_addr)
update_addr(dst_test_s_linker, "TBSA_TEST_INFO_LENGTH", \
                                TBSA_TEST_INFO_LENGTH)
update_addr(dst_test_s_linker, "TBSA_TEST_TEXT_START", \
                                hex(int(s_test_addr,16) + \
                                (int(TBSA_TEST_INFO_LENGTH[:TBSA_TEST_INFO_LENGTH.find("K")]) * 1024)))
update_addr(dst_test_s_linker, "TBSA_TEST_TEXT_LENGTH", \
                                TBSA_TEST_TEXT_LENGTH)
update_addr(dst_test_s_linker, "TBSA_TEST_DATA_START", \
                                hex(int(s_test_addr, 16) + \
                                (int(TBSA_TEST_INFO_LENGTH[:TBSA_TEST_INFO_LENGTH.find("K")]) * 1024) + \
                                (int(TBSA_TEST_TEXT_LENGTH[:TBSA_TEST_TEXT_LENGTH.find("K")]) * 1024)))
update_addr(dst_test_s_linker, "TBSA_TEST_DATA_LENGTH", \
                                TBSA_TEST_DATA_LENGTH)

# Updating NS test linker script with address from target configuration file
update_addr(dst_test_ns_linker, "TBSA_TEST_INFO_START", \
                                 ns_test_addr)
update_addr(dst_test_ns_linker, "TBSA_TEST_INFO_LENGTH", \
                                 TBSA_TEST_INFO_LENGTH)
update_addr(dst_test_ns_linker, "TBSA_TEST_TEXT_START", \
                                 hex(int(ns_test_addr,16) + \
                                 (int(TBSA_TEST_INFO_LENGTH[:TBSA_TEST_INFO_LENGTH.find("K")]) * 1024)))
update_addr(dst_test_ns_linker, "TBSA_TEST_TEXT_LENGTH", \
                                 TBSA_TEST_TEXT_LENGTH)
update_addr(dst_test_ns_linker, "TBSA_TEST_DATA_START", \
                                 hex(int(ns_test_addr, 16) + \
                                 (int(TBSA_TEST_INFO_LENGTH[:TBSA_TEST_INFO_LENGTH.find("K")]) * 1024) + \
                                 (int(TBSA_TEST_TEXT_LENGTH[:TBSA_TEST_TEXT_LENGTH.find("K")]) * 1024)))
update_addr(dst_test_ns_linker, "TBSA_TEST_DATA_LENGTH", \
                                 TBSA_TEST_DATA_LENGTH)
