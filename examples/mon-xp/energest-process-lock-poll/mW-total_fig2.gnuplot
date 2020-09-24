set term eps enhanced dashed size 10cm, 6.5cm
set output "mW-total2.eps"

set key top left

set grid

set linetype 1 dashtype 2
set linetype 2 dashtype 1

set xlabel "Process P1 workload (cyc.)"
set ylabel "Total consumption (mW)"

set xrange [0:5000]
set yrange [6.6:6.8]
set format y "%3.2f"
set format x "%.0s%c"

plot 'data/mW/isr-enabled_ALL.data' using 1:($14+$18) title "ISR opt. enabled" with linespoints lc 0 lt 2 pt 2 ps 0.5, \
     'data/mW/isr-disabled_ALL.data' using 1:($14+$18) title "ISR opt. disabled" with linespoints lc 0 lt 1 pt 4 ps 0.5
