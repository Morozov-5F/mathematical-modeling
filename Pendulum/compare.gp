set term postscript color enhanced

set out "plot_approx.ps"

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

set title   "Phase portrait"
set xlabel  "Angle, radians"
set ylabel  "Velocity, radians per second"

unset y2label
set xrange [-5:5]
set yrange [-5:5]

plot "general_model1.dat" using 2:3 title "General" axes x1y1 w l,\
	 "approx_model1.dat" using ($2 - pi):3 title "Approximate" axes x1y1 w l

plot "general_model2.dat" using 2:3 title "General" axes x1y1 w l,\
	 "approx_model2.dat" using ($2 - pi/2):3 title "Approximate" axes x1y1 w l

plot "general_model3.dat" using 2:3 title "General" axes x1y1 w l,\
	 "approx_model3.dat" using ($2 - pi/2):3 title "Approximate" axes x1y1 w l
