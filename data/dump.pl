#!/usr/bin/perl

use strict;
use warnings;

our $TICKS_PER_SECOND = 8;

my $in = $ARGV[0];
my $out = $in;
$out =~ s/\.raw$/.txt/;

open IH, $in or die;
open OH, ">../parsed/$out" or die "../parsed/$out $!";
select OH;

my ($hour, $minute, $second, $subsecond, $buttons) = (0,0,0,0,0);

while(1) {
  my $packet;
  my $count = sysread IH, $packet, 128;
  last unless $count == 128;

  my @packet = unpack("C[128]", $packet);
  my $i = 0;
  if ($packet[0] == 255 && $packet[1] == 238) {
    $buttons = $packet[3];
    $hour = $packet[4];
    $minute = $packet[5];
    $second = $packet[6];
    $subsecond = $packet[7];

    $i = 8;
  }

  my $lasttime;
  for(; $i<@packet; $i += 4) {
    my $fractionalSubseconds = 1.0 * $subsecond / $TICKS_PER_SECOND;
    my $shiftedSeconds = $subsecond * (60 / $TICKS_PER_SECOND);

    my $totalMinutes = $hour * 60 + $minute + 1.0 * ($second + $fractionalSubseconds) / 60;

    my $time = $hour * 3600 + $minute * 60 + $second + $fractionalSubseconds;
    if (defined $lasttime) {
      my $delta = $time - $lasttime;
      if ($delta != .125) {
        die "SKIP\n";
      }
    }
    $lasttime = $time;

    my $d1 = $packet[$i] * 256 + $packet[$i+1];
    my $d2 = $packet[$i+2] * 256 + $packet[$i+3];

    $d1 = $d1 == 65535 ? 1023 : $d1;
    $d2 = $d2 == 65535 ? 1023 : $d2;

    printf "%010f %03d %03d %02d:%02d:%02f %04d %04d\n", $totalMinutes, $i, $buttons, $hour, $minute, $second + $fractionalSubseconds, $d1, $d2;
    $subsecond++;
    if ($subsecond >= $TICKS_PER_SECOND) {
      $subsecond -= $TICKS_PER_SECOND;
      $second++;

      if ($second >= 60) {
        $second -= 60;
        $minute++;

        if ($minute >= 60) {
          $minute -= 60;
          $hour++;

          if ($hour >= 24) {
            $hour -= 24;
          }
        }
      }
    }
  }
}
select STDOUT;
close OH;
