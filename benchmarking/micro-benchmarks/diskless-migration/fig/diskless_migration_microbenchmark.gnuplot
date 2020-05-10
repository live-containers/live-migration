#!/usr/bin/gnuplot
#load "styles.inc"
set terminal postscript color eps enhanced font 22 size 6,2
set output 'diskless_migration_microbenchmark.eps'
set datafile separator " "
set style fill solid 0.85 border -1
set boxwidth 0.85 

set bmargin 2.5
set lmargin 6
set rmargin 6

# Tics and labels
set xtic scale 0
set xrange [-0.25:39.25]
set ytics nomirror (0.2, 0.4, 0.6, 0.8)
set yrange [0:1]
set grid y
set y2range [0:30]
set y2tics (6, 12, 18, 24)
set xtics ("criu" 2, "runc" 7, "criu" 12, "runc" 17, "criu" 22, "runc" 27, "criu" 32, "runc" 37) offset 0,0.25
set ylabel "Time Elapsed [s]" offset 1.5, 0.0

# Top titles
set arrow from 19.5, graph 0 to 19.5, graph 1 nohead
set title "{/Bold Simple Counter}" offset -12.5,-0.75
set label at screen 0.60, screen 0.92 "{/Bold Redis Database}" offset 0,-0.5
# Bottom titles
set label at screen 0.15, screen 0.055 "{/Bold Local}"
set label at screen 0.34, screen 0.055 "{/Bold Remote}"
set label at screen 0.57, screen 0.055 "{/Bold Local}"
set label at screen 0.77, screen 0.055 "{/Bold Remote}"
set key at screen 0.99,screen 1.00 vertical maxrows 1 width -5.5 sample 1.2 font ",14"
plot 'diskless_migration_microbenchmark.dat' \
       using ($1-1.5):($2/1000) every :::0::0 w boxes lc rgb "#fd9103" title "Diskless w/ Page-Server" ,\
    '' using ($1-1.5):($2/1000):($3/1000) every :::0::0 w yerrorbars lc rgb "#fd9103" notitle ,\
    '' using ($1-0.5):($4/1000) every :::0::0 w boxes lc rgb "#f05e16" title "Diskless w/out Page-Server",\
    '' using ($1-0.5):($4/1000):($5/1000) every :::0::0 w yerrorbars lc rgb "#f05e16" notitle,\
    '' using ($1+0.5):($6/1000) every :::0::0 w boxes lc rgb "#8803FD" title "File-Based w/ Page-Server",\
    '' using ($1+0.5):($6/1000):($7/1000) every :::0::0 w yerrorbars lc rgb "#8803FD" notitle,\
    '' using ($1+1.5):($8/1000) every :::0::0 w boxes lc rgb "#BC1B36" title "File-Based w/out Page-Server",\
    '' using ($1+1.5):($8/1000):($9/1000) every :::0::0 w yerrorbars lc rgb "#BC1B36" axes x1y2 notitle,\
    '' using ($1-1.5):($2/1000) every :::1::1 w boxes lc rgb "#fd9103" axes x1y2 notitle,\
    '' using ($1-1.5):($2/1000):($3/1000) every :::1::1 w yerrorbars lc rgb "#fd9103" axes x1y2 notitle,\
    '' using ($1-0.5):($4/1000) every :::1::1 w boxes lc rgb "#f05e16" axes x1y2 notitle,\
    '' using ($1-0.5):($4/1000):($5/1000) every :::1::1 w yerrorbars lc rgb "#f05e16" axes x1y2 notitle,\
    '' using ($1+0.5):($6/1000) every :::1::1 w boxes lc rgb "#8803FD" axes x1y2 notitle,\
    '' using ($1+0.5):($6/1000):($7/1000) every :::1::1 w yerrorbars lc rgb "#8803FD" axes x1y2 notitle,\
    '' using ($1+1.5):($8/1000) every :::1::1 w boxes lc rgb "#BC1B36" axes x1y2 notitle,\
    '' using ($1+1.5):($8/1000):($9/1000) every :::1::1 w yerrorbars lc rgb "#BC1B36" axes x1y2 notitle

!epstopdf 'diskless_migration_microbenchmark.eps'
!rm 'diskless_migration_microbenchmark.eps'
