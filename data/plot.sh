#!/bin/bash

set -e

trap 'onerror' ERR

onerror() {
 echo "Whoops"
 exit 1
}


#set xrange [1.25:2.5]

INFILE="$1"
OUTFILE="${2-}"
if [[ -z $INFILE ]]; then
 echo "Usage: $0 <file> [/path/to/output.png]"
 exit 1
fi



#plot "$INFILE" using 1:3 title 'Buttons' with lines axes x1y2, '' using 1:5 title 'Left' with lines
(
 echo "set yrange [0:1024]"
 echo "set y2range [0:10]"
 echo "set xtics 0,10"
# echo "set ytics 0,50"
 if [[ ! -z ${OUTFILE} ]]; then
  if [[ -d $OUTFILE ]]; then
   OUTFILENAME=$(basename "$1" | sed 's/\..*^//')
   OUTFILE="${OUTFILE}/$OUTFILENAME.svg"
  fi
  echo "set terminal svg size 3800,2400"
  echo "set output '$OUTFILE'"
 fi
 echo "plot '$INFILE' using 1:3 title 'Buttons' with lines axes x1y2, '' using 1:5 title 'Left' with lines, '' using 1:6 title 'Right' with lines"
) | gnuplot -persist 

#eog "$OUTFILE"
