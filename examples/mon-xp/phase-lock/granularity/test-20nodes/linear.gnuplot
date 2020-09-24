set grid
unset key

set xlabel "Simulation time (µs)"
set ylabel "CPU time for each node (µs)"

A=182415881
B=374847883
C=481915985
H=5e7

set arrow from A,0 to A, H nohead lc rgb 'red'
set arrow from B,0 to B, H nohead lc rgb 'red'
set arrow from C,0 to C, H nohead lc rgb 'red'

plot 'linear.data' using 3:($4 / 16)

pause -1
