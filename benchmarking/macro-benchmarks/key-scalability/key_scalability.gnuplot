#!/usr/bin/gnuplot
#load "styles.inc"
set terminal postscript color eps enhanced font 22 size 6,2
set output 'key_scalability.eps'
set datafile separator " "
set style data histograms
set style histogram rowstacked
set style fill solid 0.65 border -1
set boxwidth 0.85 relative

set yrange [0:3]
set xlabel "Number of Keys loaded to Redis" offset 15,0
set ylabel "Execution Time [s]" offset 1.5, 0.0

set multiplot layout 1,2 \
    margins 0.1, 0.99, 0.25, 0.8 \
    spacing 0.0

# Line Styles: check line and point types in PostScript terminal here
# https://stackoverflow.com/questions/19412382/gnuplot-line-types
set style line 01 lt 1 lc rgb "#fd9103" pt 4
set style line 11 lt 1 dt 2 lc rgb "#FD9103" pt 4
set style line 02 lt 1 lc rgb "#8803FD" pt 8
set style line 12 lt 2 dt 2 lc rgb "#8803FD" pt 8
set style line 03 lt 1 lw 3 lc rgb "#BC1B36" pt 4
set style line 13 lt 2 lw 3 dt 2 lc rgb "#BC1B36" pt 19

# Plot 0
set title "{/Bold Naive Live Migration}" offset 0,-0.5
set xtics ("10^0" 0, "10^1" 1, "10^2" 2, "10^3" 3, "10^4" 4, "10^5" 5, "10^6" 6, "10^7" 7)
set key at screen 0.87,screen 1.00 vertical maxrows 1 sample 1.2 width -2.5 font "Arial ,20"
plot 'key_scalability_processed.dat' \
    using 1 every ::0::7 lc rgb "#fd9103" title "Checkpoint Time" ,\
    '' using 2 every ::0::7 lc rgb "#8803FD" title "Restore Time" ,\
    '' using 3 every ::0::7 w lp ls 03 title "Total Time (Add)" ,\
    '' using 3:4 every ::0::7 w yerrorbars notitle ls 13

# Plot 1
set title "{/Bold Our Library}"
unset ylabel
unset xlabel
set ytics format ""
plot 'key_scalability_processed.dat' \
    using 1 every ::8::15 lc rgb "#fd9103" notitle "Checkpoint" ,\
    '' using 2 every ::8::15 lc rgb "#8803FD" notitle "Restore" ,\
    '' using 3 every ::8::15 w lp ls 03 notitle "Total Time (Add)" ,\
    '' using 3:4 every ::8::15 w yerrorbars notitle ls 13

!epstopdf 'key_scalability.eps'
!rm 'key_scalability.eps'
