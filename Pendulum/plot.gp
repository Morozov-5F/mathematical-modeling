set term postscript color enhanced

set out "plot.ps"

set grid
set autoscale xy

set title "Pendulum oscillations"

set xlabel  "Time"
set ylabel  "Angle, radians"
set y2label "Velocity, radians per second"

set key out

set ytics nomirror
set y2tics
set tics out

plot "general.dat" using 1:2 title 'Angle' axes x1y1 w l,\
	 "general.dat" using 1:3 title 'Velocity' axes x1y2 w l

# Phase portrait
set title   "Phase portrait"
set xlabel  "Angle, radians"
set ylabel  "Velocity, radians per second"

unset y2label
set xrange [-5:5]
set yrange [-5:5]

plot "general.dat"  using 2:3 title "General case" axes x1y1 w l,\
	"harmonic.dat" using 2:3 title "Small angles" axes x1y1 w l,\
	"exp.dat"      using 2:3 title "Big angle" axes x1y1 w l