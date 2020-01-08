#!/usr/bin/python
#/** @file
# * Copyright (c) 2019, Arm Limited or its affiliates. All rights reserved.
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
import re
import shutil
from argparse import ArgumentParser

driver_linker_pattern = \
'   ],\n'\
'   "linker_pattern": {\n'\
'     "library_list": [\n'\
'     "*driver_partition.a"\n'\
'   ]\n'\
' }\n'
client_linker_pattern = \
'   ],\n'\
'   "linker_pattern": {\n'\
'     "library_list": [\n'\
'     "*client_partition.a"\n'\
'   ]\n'\
' }\n'
server_linker_pattern = \
'   ],\n'\
'   "linker_pattern": {\n'\
'     "library_list": [\n'\
'     "*server_partition.a"\n'\
'   ]\n'\
' }\n'

def update_manifest_file(manifest_dir_path, sp_heap_mem_supp, add_linker_pattern):
	"""
	- Remove heap_size field from manifest if dynamic memory function is not supported.
	- TFM needs special linker pattern field(other than PSA FF defined field)
	  in manifest to place partition code/data at appropriate places in the memory.
	  This routine appends these linker pattern to the available manifest files.
	"""
	linker_pattern_present = 0
	driver_partition_manifest = manifest_dir_path+"/common/driver_partition_psa.json"
	client_partition_manifest = manifest_dir_path+"/ipc/client_partition_psa.json"
	server_partition_manifest = manifest_dir_path+"/ipc/server_partition_psa.json"
	file_pattern_dict = {driver_partition_manifest:driver_linker_pattern,
						 client_partition_manifest:client_linker_pattern,
						 server_partition_manifest:server_linker_pattern}

	for file,pattern in file_pattern_dict.items():
		print("Updating "+file+" manifest file")
		f_i = open(file, 'r')
		f_o = open(file+".update", 'w')
		f1 = f_i.readlines()
		for line in f1:
				if (add_linker_pattern == "1"):
					if (linker_pattern_present == 1):
						f_o.write(line)
						continue
					if (re.findall('linker_pattern', line)):
						linker_pattern_present = 1
						f_o.write(line)
						continue
					if (re.findall(']$', line)):
						f_o.write(pattern)
						continue

				if (sp_heap_mem_supp == "0"):
					if (re.findall('heap_size', line)):
						continue
				f_o.write(line)
		shutil.move(file+".update", file)

def argparse():
	"""
	Parse the command line argument
	"""
	parser = ArgumentParser(description='Utility to update manifest files')
	parser.add_argument('--manifest_dir_path', help="Absolute path for manifest directory",
                        type=str, action="store", dest='manifest_dir_path')
	parser.add_argument('--sp_heap_mem_supp',
						help="Pass 1 if platform supports dynamic memory functions \
						for secure partition otherwise 0. Default is 1. Passing 0 \
						will remove the heap_size field from the manifest",
                        type=str, action="store", dest='sp_heap_mem_supp')
	parser.add_argument('--add_linker_pattern', help="Pass 1 if platform is Trusted Firmware-M otherwise 0.\
						This updates the manifest files with TFM required linker pattern",
                        type=str, action="store", dest='add_linker_pattern')
	options = parser.parse_args()

	if (len(sys.argv) != 4):
		print("\n Invaild number of arguments. Refer help message.")
		sys.exit(1)

	return options

#main code starts here
if __name__ == "__main__":
	options = argparse()
	update_manifest_file(options.manifest_dir_path, options.sp_heap_mem_supp, options.add_linker_pattern)
