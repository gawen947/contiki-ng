#!/bin/sh

if [ $# != 5 ]
then
  echo "usage: $0 blackouts-file bin-width distrib-file plot-file"
  exit 1
fi

blackout_file="$1"
bin_width="$2"
distrib_file="$3"
plot_file="$4"

cat "$blackout_file" | grep -oE "time_dist=.* s$" | sed 's/time_dist=//g' | sed 's/ s//g' | python3 compute-distrib.py > "$distrib_file"


