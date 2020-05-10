#!/usr/bin/gnuplot
#load "styles.inc"
set terminal postscript color eps enhanced font 22 size 6,2
set output 'diskless_migration_microbenchmark.eps'
set datafile separator " "
set style fill solid 0.85 border -1
set boxwidth 0.85 

set bmargin 2.5
set lmargin 6

# Tics and labels
set tic scale 0
set xrange [-0.25:29.25]
set ytics (20, 40, 60, 80)
set yrange [0:12]
set xtics ("criu" 1.5, "runc" 5, "criu" 9, "runc" 12.5, "criu" 16.5, "runc" 20, "criu" 24, "runc" 27.5) offset 0,0.25
set ylabel "Memory Dump Size [kB]" offset 1.5, 0.0

# Top titles
set arrow from 14.5, graph 0 to 14.5, graph 1 nohead
set title "{/Bold Simple Counter}" offset -12.0,-0.75
set label at screen 0.54, screen 0.92 "{/Bold Redis Database}" offset 0,-0.5
# Bottom titles
set label at screen 0.11, screen 0.055 "{/Bold Local}"
set label at screen 0.31, screen 0.055 "{/Bold Remote}"
set label at screen 0.51, screen 0.055 "{/Bold Local}"
set label at screen 0.71, screen 0.055 "{/Bold Remote}"
set key at screen 0.84,screen 1.00 vertical maxrows 1 sample 1.2 
plot 'diskless_migration_microbenchmark.dat' \
       using ($1-1):($2/1000) every :::0::0 w boxes lc rgb "#fd9103" title "Diskless w/ Page-Server" ,\
    '' using ($1-1):($2/1000):($3/1000) every :::0::0 w yerrorbars lc rgb "#fd9103" notitle ,\
    '' using ($1):($4/1000) every :::0::0 w boxes lc rgb "#8803FD" title "File-Based w/ Page-Server",\
    '' using ($1):($4/1000):($5/1000) every :::0::0 w yerrorbars lc rgb "#8803FD" notitle,\
    '' using ($1+1):($6/1000) every :::0::0 w boxes lc rgb "#BC1B36" title "File-Based w/out Page-Server",\
    '' using ($1+1):($6/1000):($7/1000) every :::0::0 w yerrorbars lc rgb "#BC1B36" notitle,\
    '' using ($1-1):($2/1000) every :::1::1 w boxes lc rgb "#fd9103" notitle,\
    '' using ($1-1):($2/1000):($3/1000) every :::1::1 w yerrorbars lc rgb "#fd9103" notitle,\
    '' using ($1):($4/1000) every :::1::1 w boxes lc rgb "#fd9103" notitle,\
    '' using ($1):($4/1000):($5/1000) every :::1::1 w yerrorbars lc rgb "#fd9103" notitle,\
    '' using ($1+1):($6/1000) every :::1::1 w boxes lc rgb "#fd9103" notitle,\
    '' using ($1+1):($6/1000):($7/1000) every :::1::1 w yerrorbars lc rgb "#fd9103" notitle

!epstopdf 'diskless_migration_microbenchmark.eps'
!rm 'diskless_migration_microbenchmark.eps'
