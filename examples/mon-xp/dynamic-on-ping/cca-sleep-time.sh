#!/bin/sh
echo "# CCA_SLEEP_TIME T_C" > t_c.data
for i in $(seq 0 3)
do
  export CST="$i"
  time=$(rpnc "$i" 1 . 32768 / 1e6 .)
  time sh xp.sh 7200 6789 1 1000 24 1.0
  result=$(cat "results/timing/ccc0-1.avg+std" | head -n 1)
  echo "$time $result" >> t_c.data
done

