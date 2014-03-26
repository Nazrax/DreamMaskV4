#!/usr/bin/perl

use warnings;
use strict;

my $last;
while (<>) {
 my ($time, $rest) = (/^(\S+)(.*)/);
 if (defined $last && $time < $last) {
  $time += $last;
 } else {
  $last = $time;
 }
 print "$time$rest\n";
}
