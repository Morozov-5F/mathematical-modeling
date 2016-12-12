set term pngcairo size 1024,768

set out 'plot.png'

set grid

set title 'Dynamical chaos'

set xlabel 'r'
set ylabel 'x'

set xrange [0:1]
set yrange [0:1]

unset key

plot "data.dat" w p pt 1 ps 0.1
