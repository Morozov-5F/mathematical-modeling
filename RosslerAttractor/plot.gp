set term aqua enhanced font "CMU Serif, 14" enhanced title "Rössler attractor"

set title "Rössler attractor" font "CMU Serif, 20"

unset key

set size square
set grid

# set view 60, 60

set xlabel "x"
set ylabel "y"
set zlabel "z"

splot "data.dat" using 1:2:3 w l
