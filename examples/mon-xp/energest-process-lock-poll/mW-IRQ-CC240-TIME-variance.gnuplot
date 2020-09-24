set key top left

set grid

set xlabel "Workload (cyc.)"
set ylabel "IRQ sum cyc."

set arrow from 655350,0 to 655350,12 nohead lc rgb "gray"

plot 'data/isr-enabled_5k-200k_CC240_IRQ_TIME.data'  using 1:22 title "ISR opt. enabled" with linespoints pt 7 ps 0.5, \
     'data/isr-disabled_5k-200k_CC240_IRQ_TIME.data' using 1:22 title "ISR opt. disabled" with linespoints pt 7 ps 0.5



pause -1
