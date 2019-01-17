#!/usr/bin/env perl
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


$build=$ARGV[0];
$tests_list=$ARGV[1];
$suite=$ARGV[2];

&gen_test_entry_info();
&gen_secure_tests_list() if ($suite eq "ipc");

sub gen_test_entry_info
{
    my $test_num                    = 0;
    my $uniq_test_string            = '';
    my $max_test_per_suite          = 200;
    my $suite_base                  = 0;
    my $test_entry_list             = "$build/val/test_entry_list.inc";
    my $test_entry_fn_declare_list  = "$build/val/test_entry_fn_declare_list.inc";

    print "\n>>>> Generating tests list by referring testsuite.db \n";

    if ($suite eq "crypto")
    {
        $suite_base = 1;
    }
    elsif ($suite eq "protected_storage")
    {
        $suite_base = 2;
    }
    elsif ($suite eq "internal_trusted_storage")
    {
        $suite_base = 3;
    }
    elsif ($suite eq "initial_attestation")
    {
        $suite_base = 4;
    }

    open(IN, $tests_list) or die "Unable to open $tests_list $!";
    open(OUT1, '>', $test_entry_fn_declare_list) or die "Unable to open: $!";
    open(OUT2, '>', $test_entry_list) or die "Unable to open: $!";

        while(<IN>) {
        if($_ !~ /^\//) {# exclude commented lines if any
            chomp($_);
            if($_ =~ /^test_(.+)/) {
                $uniq_test_string = $1;
                $uniq_test_string =~ s/s/p/ if ($suite eq "protected_storage");
                print OUT1 "void test_entry_$uniq_test_string(val_api_t *val_api, psa_api_t *psa_api);\n";
                if($_ =~ /^test_\w0*(\d+)/) {
                    $test_num = $1 + ($max_test_per_suite * $suite_base);
                    print OUT2 "\t{$test_num, &test_entry_$uniq_test_string},\n";
                }
            }
        }
    }
    close IN;
    close OUT1;
    close OUT2;
    print "Output files are:
    $test_entry_list,
    $test_entry_fn_declare_list\n";
}

sub gen_secure_tests_list
{
    my $test_num = 0;
    my $test_num_prev = 0;
    my $client_tests_list_declare = "$build/partition/client_tests_list_declare.inc";
    my $client_tests_list         = "$build/partition/client_tests_list.inc";
    my $server_tests_list_declare = "$build/partition/server_tests_list_declare.inc";
    my $server_tests_list         = "$build/partition/server_tests_list.inc";

    print "\n>>>> Generating secure tests list files by referring testsuite.db \n";

    open(IN, $tests_list) or die "Unable to open $tests_list $!";
    open(OUT1, '>', $client_tests_list_declare) or die "Unable to open: $!";
    open(OUT2, '>', $client_tests_list) or die "Unable to open: $!";
    open(OUT3, '>', $server_tests_list_declare) or die "Unable to open: $!";
    open(OUT4, '>', $server_tests_list) or die "Unable to open: $!";

    while(<IN>) {
        if($_ !~ /^\//) {# exclude commented lines if any
            chomp($_);
            if($_ =~ /^test_\w(\d+)/) {
                $test_num = $1;
                print OUT1 " extern client_test_t $_\_client_tests_list[];\n";
                print OUT3 " extern server_test_t $_\_server_tests_list[];\n";

                if ($test_num - $test_num_prev > 1)
                {
                    for ($i = $test_num_prev; $i < ($test_num - 1); $i++)
                    {
                        print OUT2 "\tNULL,\n";
                        print OUT4 "\tNULL,\n";
                    }
                }
                print OUT2 "\t$_\_client_tests_list,\n";
                print OUT4 "\t$_\_server_tests_list,\n";
            }
        }
        $test_num_prev = $test_num;
    }
    close IN;
    close OUT1;
    close OUT2;
    close OUT3;
    close OUT4;

    print "Output files are:
    $client_tests_list_declare,
    $client_tests_list,
    $server_tests_list_declare,
    $server_tests_list \n\n";
}
