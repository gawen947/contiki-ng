set key top left

set grid

set xlabel "Workload (cyc.)"
set ylabel "Total power optimization (%)"

set arrow from 655350,0 to 655350,12 nohead lc rgb "gray"

# 14 + 18 = total enabled time
# 32 + 36 = total disabled time
plot 'data/mW/isr-combined_ALL.data' using 1:(100*(($32+$36)-($14+$18)) / ($32+$36)) title "ISR opt. enabled" with linespoints pt 7 ps 0.5


pause -1
