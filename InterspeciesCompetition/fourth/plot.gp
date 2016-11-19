set term pdfcairo enhanced font "CMU Serif, 60" size 33.1in, 23.4in linewidth 5

set out "plot.pdf"

set grid
set autoscale xy

set title "Interspecies competition simulation" font "CMU Serif, 80"

set xlabel  "Time, s"
set ylabel  "Population, unit"

set key out

set yrange [-0.25:3.5]

set ytics nomirror
set tics out

do for [i=0:120:20] {
	ttl = sprintf('Interspecies competition simulation, initial conditions (%1.1f, %1.1f)', i / 2.0, i / 2.0 - 0.5)
	set title ttl font "CMU Serif, 80"
	plot "data_".i.".dat" using 1:2 title 'First specie' w l,\
	 	 "data_".i.".dat" using 1:3 title 'Second specie' w l
}

# Phase portrait
set size square
set style fill transparent solid 0.15 border

set palette defined

set title   "Phase portrait"
set xlabel  "First specie population, species"
set ylabel  "Second specie population, species"

set xrange [-0.25:2]
set yrange [-0.25:3]

set key out
unset key

plot for [i=0:120] 'data_'.i.'.dat' using 2:3:4:5 w vec title sprintf("Initial conditions (%1.1f, %1.1f)", i / 2.0, i / 2.0 - 0.5),\
	 2 - x w l ls 0 notitle,\
	 3 - 3*x w l ls 0 notitle

set terminal gif animate size 800,600  delay 1 font "CMU Serif, 12" large
set out "animation.gif"

set title   "Phase portrait" font "CMU Serif, 20"
set xlabel  "First specie population, species"
set ylabel  "Second specie population, species"

print ">> Animating sequence began"

set style line 1 lt 1 lw 2 pt 7 ps 2 # --- blue

len = 0.1
r(x,y) = sqrt(x * x + y * y)
dx(x, y) = len * x / r(x,y)
dy(x, y) = len * y / r(x,y)

do for [i=1:199] {
	plot for [j=0:120] 'data_'.j.'.dat' using 2:3:(dx($4, $5)):(dy($4, $5)) every ::i::i+1 w vec lt 2,\
		 2 - x w l ls 0 notitle,\
	 	 3 - 3*x w l ls 0 notitle
}
