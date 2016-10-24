set terminal gif animate optimize delay 1 crop size 1600, 900
set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 2 # --- blue
set style line 2 lc rgb '#ad0f00' lt 1 lw 2 pt 7 ps 2 # --- red
set style line 3 lc rgb '#00ad60' lt 1 lw 2 pt 7 ps 2 # --- green
set style line 4 lc rgb '#f0ce3e' lt 1 lw 2 pt 7 ps 2 # --- yellow

unset autoscale
unset key

set title   "String oscillations"
set xlabel  "Length, m"
set ylabel  "Amplitude, m"

set grid

set yrange[-5:5]
set xrange[0:1]
set out 'anim.gif'

NUM = "`cat out/info.dat`"

name(n) = sprintf("out/%d.dat", n);
name_num(n) = sprintf("out/%d_num.dat", n);

set key out
# plot "out/0.dat" using 1:2 w l
do for [ii = 2:NUM] {
    plot name(ii) using 1:2 w l title "Exact" ls 1, \
         name_num(ii) using 1:2 w l title "Numeric" ls 2
}