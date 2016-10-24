set term postscript color enhanced

set out "plot.ps"

set grid
set autoscale xy

set title "String oscillations"

set xlabel  "Time, s"
set ylabel  "Amplitude, m"

set key out

set ytics nomirror
set tics out

set yrange [-2:2]

plot "out/0.dat" using 1:2 title 'Amplitude' w l,\