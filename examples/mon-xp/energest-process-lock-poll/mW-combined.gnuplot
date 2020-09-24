set key top left

set grid

set xlabel "Workload (cyc.)"
set ylabel "Power over entire experiment (mW)"

set arrow from 655350,0 to 655350,12 nohead lc rgb "gray"

plot 'data/mW/isr-enabled_ALL.data' using 1:($14+$18) title "enabled Total" with linespoints pt 7 ps 1, \
     'data/mW/isr-enabled_ALL.data' using 1:18 title "enabled Listen" with linespoints pt 7 ps 1, \
     'data/mW/isr-enabled_ALL.data' using 1:14 title "enabled CPU" with linespoints pt 7 ps 1, \
     'data/mW/isr-disabled_ALL.data' using 1:($14+$18) title "disabled Total" with linespoints pt 6 ps 1, \
     'data/mW/isr-disabled_ALL.data' using 1:18 title "disabled Listen" with linespoints pt 6 ps 1, \
     'data/mW/isr-disabled_ALL.data' using 1:14 title "disabled CPU" with linespoints pt 6 ps 1

pause -1
