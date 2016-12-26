set term pngcairo enhanced size 1024,768 font "CMU Serif, 12"

set out "matrix.png"

set title "Lorenz attractor" font "CMU Serif, 20"

unset key

set size square

set xrange [-1:]
set yrange [-1:]

# set palette defined ( 0 0 0 0, 1 1 1 1 )
# set palette rgb 23,28,3
# set pal def (0 "grey", 1 "black")
set grid
set palette model CMY rgbformulae 7,5,15

plot "data.dat" matrix with image
