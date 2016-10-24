set term pngcairo truecolor font "CMU Serif, 20" size 2560, 1600

set out "direct.png"

set grid
set autoscale xy

set title "Predator-prey simulation" font "CMU Serif, 30"

set xlabel  "Time, s"
set ylabel  "Population"

set key out

set ytics nomirror
set tics out

plot "data.dat" using 1:2 title 'Preys' w l,\
	 "data.dat" using 1:3 title 'Predators' w l

# Phase portrait
set out "phase.png"

set size square
unset key
set title   "Phase portrait"
set xlabel  "Prey population, species"
set ylabel  "Predator population, species"

set xrange [0:4]
set yrange [-0.25:2.5]

plot "data.dat" using 2:3 w l