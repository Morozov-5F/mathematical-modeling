set terminal gif animate optimize delay 1 crop
set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 2 # --- blue
set style line 2 lc rgb '#ad0f00' lt 1 lw 2 pt 7 ps 2 # --- red
set style line 3 lc rgb '#00ad60' lt 1 lw 2 pt 7 ps 2 # --- green
set style line 4 lc rgb '#f0ce3e' lt 1 lw 2 pt 7 ps 2 # --- yellow

unset autoscale
unset key

set title   "Phase portrait"
set xlabel  "Angle, radians"
set ylabel  "Velocity, radians per second"

set grid

set yrange[-3:3]
set xrange[-10:10]
set out 'anim.gif'

do for [ii = 1:200] {
    plot 'general1.dat' using 2:3 every ::1::ii w l ls 2, \
         'general1.dat' using 2:3 every ::ii::ii w p ls 2,\
         'general2.dat' using 2:3 every ::1::ii w l ls 2, \
         'general2.dat' using 2:3 every ::ii::ii w p ls 2,\
         'general3.dat' using 2:3 every ::1::ii w l ls 4, \
         'general3.dat' using 2:3 every ::ii::ii w p ls 4,\
         'general4.dat' using 2:3 every ::1::ii w l ls 3, \
         'general4.dat' using 2:3 every ::ii::ii w p ls 3,\
         'general5.dat' using 2:3 every ::1::ii w l ls 3, \
         'general5.dat' using 2:3 every ::ii::ii w p ls 3,\
         'general6.dat' using 2:3 every ::1::ii w l ls 1, \
         'general6.dat' using 2:3 every ::ii::ii w p ls 1,\
         'general7.dat' using 2:3 every ::1::ii w l ls 1, \
         'general7.dat' using 2:3 every ::ii::ii w p ls 1,\
         'general8.dat' using 2:3 every ::1::ii w l ls 3, \
         'general8.dat' using 2:3 every ::ii::ii w p ls 3, \
         'general9.dat' using 2:3 every ::1::ii w l ls 3, \
         'general9.dat' using 2:3 every ::ii::ii w p ls 3,\
         'general10.dat' using 2:3 every ::1::ii w l ls 4, \
         'general10.dat' using 2:3 every ::ii::ii w p ls 4,\
         'general11.dat' using 2:3 every ::1::ii w l ls 2, \
         'general11.dat' using 2:3 every ::ii::ii w p ls 2,\
         'general12.dat' using 2:3 every ::1::ii w l ls 2, \
         'general12.dat' using 2:3 every ::ii::ii w p ls 2
}
