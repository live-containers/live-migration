#!/usr/bin/gnuplot
#load "styles.inc"
set terminal postscript color eps enhanced font 22 size 6,2
set output 'tcp_established_downtime_microbenchmark.eps'
set datafile separator " "

# Tics and labels
#set xtic scale 0
#set xrange [-0.25:29.25]
set grid y
set ytics (1, 2, 3, 4)
set yrange [0:5]
set ylabel "Throughput [Gbps]" offset 0, 0.0
set xlabel "Time [s]"

# Top titles
set arrow from 10, graph 0 to 10, graph 1 nohead dt 4
set arrow from 12, graph 0 to 12, graph 1 nohead dt 4
#set title "{/Bold Simple Counter}" offset -12.0,-0.75
#set label at screen 0.54, screen 0.92 "{/Bold Redis Database}" offset 0,-0.5
## Bottom titles
set label at screen 0.46, screen 0.96 "{/Bold Checkpoint}" font ",16"
set label at screen 0.58, screen 0.96 "{/Bold Restore}" font ",16"
#set label at screen 0.31, screen 0.055 "{/Bold Increment}"
#set label at screen 0.51, screen 0.055 "{/Bold No Shuffle}"
#set label at screen 0.71, screen 0.055 "{/Bold Shuffle}"
set key at screen 0.34,screen 1.00 vertical maxrows 1 sample 1.2 
plot 'criu_client.dat' using 1:($2 * 10 / 1024 * 8) w lp lc rgb "#fd9103" title "criu" ,\
     'runc_client.dat' using 1:($2 * 10 / 1024 * 8) w lp lc rgb "#BC1B36" title "runc"

!epstopdf 'tcp_established_downtime_microbenchmark.eps'
!rm 'tcp_established_downtime_microbenchmark.eps'
