set term pdfcairo enhanced font "CMU Serif, 60" size 33.1in, 23.4in linewidth 5

set out "plot.pdf"

set grid
set autoscale xy

set title "Predator-prey simulation" font "CMU Serif, 80"

set xlabel  "Time, s"
set ylabel  "Population, unit"

set key out

set yrange [-0.25:3.5]

set ytics nomirror
set tics out

do for [i=1:10] {
	ttl = sprintf('Predator-prey simulation, initial conditions (%1.1f, %1.1f)', 0.8 + i / 10.0, 0.8 + i / 10.0)
	set title ttl font "CMU Serif, 80"
	plot "data_".i.".dat" using 1:2 title 'Preys' w l,\
	 	 "data_".i.".dat" using 1:3 title 'Predators' w l
}

# Phase portrait
set size square
set style fill transparent solid 0.15 border

set title   "Phase portrait"
set xlabel  "Prey population, species"
set ylabel  "Predator population, species"

set xrange [0:4]
set yrange [-0.25:2.5]

set key out

plot for [i=1:10] 'data_'.i.'.dat' using 2:3 w l title sprintf("Initial conditions (%1.1f, %1.1f)", 0.8 + i / 10.0, 0.8 + i / 10.0)
