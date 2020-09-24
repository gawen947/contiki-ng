set term png
set output "preliminary.png"

set key box bottom right

set xlabel "Requested deviation"
set ylabel "Observed deviation"

set title "Deviation parameter bug (meta-sim with untuned models)"

set grid

plot 'preliminary-sleep.data' pt 7 with linespoints title "LPM enabled", \
     'preliminary-nosleep.data' pt 6 with linespoints title "LPM disabled"
