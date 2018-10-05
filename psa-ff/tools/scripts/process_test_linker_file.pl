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

print "\n>>>> Updating linker file... \n";

#inputs
$source=$ARGV[0];
$suite_out=$ARGV[1];
$target=$ARGV[2];
$toolchain=$ARGV[3];

$targetConfigPath = "$source/platform/targets/$target/target.cfg";
$linker_in = "";
$linker_out = "";
$ns_test_start_addr = undef;

if($toolchain eq "GNUARM")
{
    $linker_in  = "$source/tools/makefiles/test.linker";
    $linker_out = "$suite_out/.test.linker";
}

open(IN, $targetConfigPath) or die "Unable to open $targetConfigPath $!";
while(<IN>) {
    if($_ !~ /^\//) {# exclude commented lines
        if($_ =~ /\.ns_test_addr(\s*)\=(\s*)(.+)(\s*)\;/) {
            $ns_test_start_addr = $3;
        }
    }
}
close IN;
if(defined($ns_test_start_addr))
{
    open(IN, $linker_in) or die "Unable to open $linker_in $!";
    open(OUT, '>', $linker_out) or die "Unable to open: $!";
    while(<IN>) {
        if($_ =~ /^TEST_START/){
            print OUT "TEST_START = $ns_test_start_addr;\n";
        }else{
            print OUT "$_";
        }
    }
    close IN;
    close OUT;
    print "linker file - $linker_out\n"
}
else
{
    die ("Error: ns_test_addr is not found in target.cfg file\n");
}
