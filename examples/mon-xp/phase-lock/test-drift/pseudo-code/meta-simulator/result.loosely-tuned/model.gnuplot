#set term png
#set output "model.png"

set key box bottom right

set xlabel "Requested deviation"
set ylabel "Observed/Predicted deviation"

set xrange [0:1]
set yrange [0:1]

set title "Deviation parameter bug (model to observation in meta-sim loosely tuned)"

set grid

a = 83.32222222222224 

plot 'devratio-sleep.data' pt 7 with linespoints title "Observed", \
     x*(a+1)/(x*a+1) with line lc "red" title "Predicted"
#     'devratio-sleep_model.data' pt 6 with linespoints title "Predicted"
pause -1
