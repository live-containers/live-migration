#!/usr/bin/gnuplot
#load "styles.inc"
set terminal postscript color eps enhanced font 22 size 6,2
set output 'tcp_established_downtime_microbenchmark.eps'
set datafile separator " "
set style fill solid 0.85 border -1
set boxwidth 0.85 

set bmargin 2.5
set rmargin 8
set lmargin 6

# Tics and labels
set xtic scale 0
set xrange [-0.25:29.25]
set grid y
set ytics (20, 40, 60, 80)
set y2tics (200, 400, 600, 800)
set yrange [0:100]
set y2range [0:1000]
set xtics ("criu" 1.5, "runc" 5, "criu" 9, "runc" 12.5, "criu" 16.5, "runc" 20, "criu" 24, "runc" 27.5) offset 0,0.25
set ylabel "Memory Dump Size [kB]" offset 1.5, 0.0
set y2label "Memory Dump Size [MB]" offset -1.5, 0.0 rotate by 270

# Top titles
set arrow from 14.5, graph 0 to 14.5, graph 1 nohead
set title "{/Bold Simple Counter}" offset -12.0,-0.75
set label at screen 0.54, screen 0.92 "{/Bold Redis Database}" offset 0,-0.5
# Bottom titles
set label at screen 0.11, screen 0.055 "{/Bold No Increment}"
set label at screen 0.31, screen 0.055 "{/Bold Increment}"
set label at screen 0.51, screen 0.055 "{/Bold No Shuffle}"
set label at screen 0.71, screen 0.055 "{/Bold Shuffle}"
set key at screen 0.84,screen 1.00 vertical maxrows 1 sample 1.2 
plot 'iterative_migration_microbenchmark.dat' \
       using ($1-1):2 every :::0::0 w boxes lc rgb "#fd9103" title "1st Pre-Dump" ,\
    '' using ($1):3 every :::0::0 w boxes lc rgb "#8803FD" title "2nd Pre-Dump",\
    '' using ($1+1):4 every :::0::0 w boxes lc rgb "#BC1B36" title "3rd Dump",\
    '' using ($1-1):2 every :::1::1 w boxes lc rgb "#fd9103" axes x1y2 notitle,\
    '' using ($1):3 every :::1::1 w boxes lc rgb "#8803FD" axes x1y2 notitle,\
    '' using ($1+1):4 every :::1::1 w boxes lc rgb "#BC1B36" axes x1y2 notitle

!epstopdf 'tcp_established_downtime_microbenchmark.eps'
!rm 'tcp_established_downtime_microbenchmark.eps'
