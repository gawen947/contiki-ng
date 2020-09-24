set key top left

set grid

set xlabel "Workload (cyc.)"
set ylabel "Listen power over entire experiment (mW)"

set arrow from 655350,0 to 655350,6 nohead lc rgb "gray"

plot 'data/mW/isr-enabled_ALL.data' using 1:18 title "ISR opt. enabled" with linespoints pt 7 ps 0.5, \
     'data/mW/isr-disabled_ALL.data' using 1:18 title "ISR opt. disabled" with linespoints pt 7 ps 0.5

pause -1
