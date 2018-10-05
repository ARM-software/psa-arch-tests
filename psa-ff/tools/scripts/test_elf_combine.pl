#!/usr/bin/env perl
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

use File::Find;
use File::Basename;
use List::MoreUtils 'uniq';

print "\n>>>> Combining test ELFs... \n";

#Inputs
$suite = $ARGV[0];
$test_list_file = $ARGV[1];

@suite_out =  split("/.testlist.txt", $test_list_file);

# Final output elf file
$output_elf = "$suite_out[0]/test_elf_combine.bin";


my @all_elf_paths;
@test_list = `cat $test_list_file`;


open(OUT, '>:raw', $output_elf) or die "Unable to open: $!";

# Collecting all elf file paths

foreach $test (@test_list)
{
	chomp($test);
	push @all_elf_paths, "$suite_out[0]/$test/test.elf";
	if (!(-e "$suite_out[0]/$test/test.elf"))
	{
		print "ELF not found - $suite_out[0]/$test/test.elf\n";
		exit 1;
	}
}

foreach $elf (@all_elf_paths) {

    # Find elf sizes
    $elf_size = -s $elf;

    # Get readelf program header info from either elf and process it
    $out = `readelf -l $elf`;
    if($out =~ /LOAD(\s+)(0[xX][0-9a-fA-F]+)/)
	{
	   $program_header = hex($2);
    }

    # Determining test_id from elf at location pointed by program header
    open(TEST_NS_ID, '<:raw', $elf) or die "Unable to open: $!";
    seek(TEST_NS_ID, $program_header, 0);
    read TEST_NS_ID, $test_id_raw, 4;

    $test_id = unpack('L<', $test_id_raw);
    close TEST_NS_ID;

    printf("test_id:\t%4d, ", $test_id);
    print "ELF:$elf,\tSIZE:$elf_size\n";

    # 'L' unsigned-32 ; '<' Little endian
    print OUT pack('L<', 0xFACEFACE);
    print OUT pack('L<', $test_id);
    print OUT pack('L<', $elf_size);

    # Temporarily closing final ELF to concatenate ELFs
    close OUT;
    system("cat $elf >> $output_elf");
    # Open final ELF again
    open(OUT, '>>:raw', $output_elf) or die "Unable to open: $!";
}
print OUT pack('L<', 0xC3C3C3C3);
close OUT;
