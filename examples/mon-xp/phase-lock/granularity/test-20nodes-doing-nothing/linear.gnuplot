set grid
unset key

set xlabel "Simulation time (µs)"
set ylabel "CPU time for each node (µs)"

A=183449786
B=304255655
C=422750569
H=5e7

set arrow from A,0 to A, H nohead lc rgb 'red'
set arrow from B,0 to B, H nohead lc rgb 'red'
set arrow from C,0 to C, H nohead lc rgb 'red'

plot 'linear.data' using 3:($4 / 16)

pause -1
