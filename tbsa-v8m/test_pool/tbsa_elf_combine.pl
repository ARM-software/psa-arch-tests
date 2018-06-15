#!/usr/bin/perl
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

$test_pool_path = $ARGV[0];
$output_elf = 'tbsa_test_combined.bin';
my @all_elf_paths;

print "Combining test ELFs\n";

# Final output elf file
open(OUT, '>:raw', $output_elf) or die "Unable to open: $!";

# Collecting all elf file paths
find sub {
    return if -d;
    if($File::Find::name =~ /\.c\.(elf)$/) {
	# Store directory of elf for each test
	push @all_elf_paths, dirname($File::Find::name); 
    }
}, $test_pool_path;

# Remove duplicate elf folders since they were loaded twice for NS & S
@all_elf_paths = uniq(@all_elf_paths);

@all_elf_paths = sort @all_elf_paths;

foreach $thispath(@all_elf_paths) {
    # Clear array consisting of ELFs for this test
    undef @this_test_elfs;
    
    # Find ELFs for this test
    find sub {
	return if -d;
	if($File::Find::name =~ /.(elf)$/) {
	    push @this_test_elfs, $File::Find::name;
	}
    }, $thispath;
    
    # Segregate NS and S ELFs
    foreach $thiself(@this_test_elfs) {
	if($thiself =~ /non_secure(\S+)elf/) {
	    $this_ns_elf = $thiself;
	}
	else {
	    $this_s_elf = $thiself;
	}
    }

    # Find elf sizes
    $this_ns_elf_size = -s $this_ns_elf;
    $this_s_elf_size = -s $this_s_elf;

    # Get readelf program header info from either elf and process it
    $out = `readelf -l $this_ns_elf`;
    if($out =~ /LOAD(\s+)(0[xX][0-9a-fA-F]+)/) {
	$program_ns_header = hex($2);
    }
    $out = `readelf -l $this_s_elf`;
    if($out =~ /LOAD(\s+)(0[xX][0-9a-fA-F]+)/) {
	$program_s_header = hex($2);
    }

    # Determining test_id from elf at location pointed by program header
    open(TEST_NS_ID, '<:raw', $this_ns_elf) or die "Unable to open: $!";
    seek(TEST_NS_ID, $program_ns_header, 0);
    read TEST_NS_ID, $test_id_ns_raw, 4;
    open(TEST_S_ID, '<:raw', $this_s_elf) or die "Unable to open: $!";
    seek(TEST_S_ID, $program_s_header, 0);
    read TEST_S_ID, $test_id_s_raw, 4;

    if($test_id_s_raw ne $test_id_ns_raw) {
        die "\nTest ID creation is not same in $this_s_elf and $this_ns_elf!!!\n\n";
    } else {
        $test_id = unpack('L<', $test_id_s_raw);
        close TEST_S_ID;
        close TEST_NS_ID;
    }

    printf("test_id\t%4d : ", $test_id);
    print "$this_s_elf\t$this_s_elf_size\t: $this_ns_elf\t$this_ns_elf_size\n";

    # 'L' unsigned-32 ; '<' Little endian
    print OUT pack('L<', 0xFACEFACE);
    print OUT pack('L<', $test_id);
    print OUT pack('L<', $this_s_elf_size);
    print OUT pack('L<', $this_ns_elf_size);
    
    # Temporarily closing final ELF to concatenate ELFs
    close OUT;
    system("cat $this_s_elf >> $output_elf");
    system("cat $this_ns_elf >> $output_elf");
    # Open final ELF again
    open(OUT, '>>:raw', $output_elf) or die "Unable to open: $!";
}
print OUT pack('L<', 0xC3C3C3C3);
close OUT;
