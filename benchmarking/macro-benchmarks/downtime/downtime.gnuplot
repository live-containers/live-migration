#!/usr/bin/gnuplot
set terminal postscript color eps enhanced font 22 size 6,2
set output 'downtime.eps'
set datafile separator " "
set style data histograms
set style histogram rowstacked
set style fill solid 0.65 border -1
set boxwidth 0.65 relative

set yrange [0:100]
set y2range [0:700]
set ytics (0, 25, 50, 75, 100)
set y2tics (0, 200, 400, 600)
set grid y
unset xtics
set xlabel "Dump Threshold Relative to Initial Memory Size [%]" 
set ylabel "Execution Time [%]" offset 1.5, 0.0
set y2label "App. Downtime [ms]" offset -1, 0.0 rotate by 270
#set xtics ("10^0" 0, "10^1" 1, "10^2" 2, "10^3" 3, "10^4" 4, "10^5" 5, "10^6" 6, "10^7" 7)
set xtics 

#set multiplot layout 1,2 \
#    margins 0.1, 0.99, 0.25, 0.8 \
#    spacing 0.0

# Line Styles: check line and point types in PostScript terminal here
# https://stackoverflow.com/questions/19412382/gnuplot-line-types
set style line 01 lt 1 lc rgb "#fd9103" pt 4
set style line 11 lt 1 dt 2 lc rgb "#FD9103" pt 4
set style line 02 lt 1 lc rgb "#8803FD" pt 8
set style line 12 lt 2 dt 2 lc rgb "#8803FD" pt 8
set style line 03 lt 1 lw 3 lc rgb "#BC1B36" pt 4
set style line 13 lt 2 lw 3 dt 2 lc rgb "#BC1B36" pt 19

# Plot 0
set key at screen 0.87,screen 1.00 vertical maxrows 1 sample 1.2 width -0.5 font "Arial ,20"
plot 'downtime_processed.dat' \
    using 4:xticlabels(1) every 2::1 with histograms lc rgb "#8803FD" title "dump" ,\
    '' using ($2+$3):xticlabels(1) every 2::1 with histograms lc rgb "#fd9103" title "prepare" ,\
    '' using 5:xticlabels(1) every 2::1 with histograms lc rgb "#BC1B36" title "transfer" ,\
    '' using 6:xticlabels(1) every 2::1 with histograms lc rgb "#366AAE" title "restore" ,\
    100 with lines linecolor rgb "black"  notitle ,\
    '' using 2:xticlabels(1) every 2::0 with lp axes x1y2 ls 13 title "downtime";

!epstopdf 'downtime.eps'
!rm 'downtime.eps'
