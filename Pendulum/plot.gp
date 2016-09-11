set term postscript

set out "plot.ps"

set grid
set autoscale xy

set title "Pendulum oscillations"

set xlabel "Time"
set ylabel "Amplitude, radians"
set y2label "Velocity, radians per second"

set key out

set ytics nomirror
set y2tics
set tics out

plot "data.dat" using 1:2:(1e-2) title 'Angle' axes x1y1 with errorbars,\
	"data.dat" using 1:3 title 'Velocity' axes x1y2 w l