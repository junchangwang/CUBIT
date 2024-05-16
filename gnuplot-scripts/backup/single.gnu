################################################# naive ####################################################
#naive_throughput_core
reset
set size 0.99,1.0
set term pdf font ",10" size 5.2, 2.4
set ylabel "Throughput (op/s)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' "Number of cores"
#set yrange [0:40]
#set xrange [0:16]
set output "../graphs/single/Figure_naive_throughput_core.pdf"
set key font ",18" reverse Left top

plot  "../dat/figure_naive_throughput_core.dat" title "naive" lc rgb "black" ps 1 pt 8 with linespoints


#naive_throughput_ratio
reset
set size 0.99,1.0
set term pdf font ",10" size 5.2, 2.4
set ylabel "Throughput (op/s)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' "UDI ratio"
#set yrange [0:40]
set output "../graphs/single/Figure_naive_throughput_ratio.pdf"
set key font ",18" reverse Left top

plot  "../dat/figure_naive_throughput_ratio.dat" title "naive" lc rgb "black" ps 1 pt 5 with linespoints


#ucb_latency_core
reset
set size 0.99,1.0
set term pdf font ",10" size 5.2, 2.4
set ylabel "latency (ms)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' " number of cores"
#set yrange [0:2900000]
#set xrange [0:20]
set output "../graphs/single/Figure_naive_latency_core.pdf"
set key font ",18" reverse Left top

plot  "../dat/figure_naive_latency_core.dat" every 5::0::20 title "Q" lc rgb "black" ps 1 pt 6 with linespoints,\
      "../dat/figure_naive_latency_core.dat" every 5::1::21  title "U" lc rgb "black" ps 1 pt 7 with linespoints, \
      "../dat/figure_naive_latency_core.dat" every 5::2::22 title "I" lc rgb "black" ps 1 pt 8 with linespoints, \
      "../dat/figure_naive_latency_core.dat" every 5::3::23 title "D" lc rgb "black" ps 1 pt 9 with linespoints,\
      "../dat/figure_naive_latency_core.dat" every 5::4::24 title "All" lc rgb "black" ps 1 pt 10 with linespoints

#naive_latency_ratio
reset
set size 0.99,1.0
set term pdf font ",10" size 5.2, 2.4
set ylabel "latency (ms)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' "UDI ratio"
#set yrange [0:2900000]
#set xrange [0:20]
set output "../graphs/single/Figure_naive_latency_ratio.pdf"
set key font ",18" reverse Left top

plot  "../dat/figure_naive_latency_ratio.dat" every 5::0::20 title "Q" lc rgb "black" ps 1 pt 6 with linespoints,\
      "../dat/figure_naive_latency_ratio.dat" every 5::1::21 title "U" lc rgb "black" ps 1 pt 7 with linespoints, \
      "../dat/figure_naive_latency_ratio.dat" every 5::2::22 title "I" lc rgb "black" ps 1 pt 8 with linespoints,\
      "../dat/figure_naive_latency_ratio.dat" every 5::3::23 title "D" lc rgb "black" ps 1 pt 9 with linespoints,\
      "../dat/figure_naive_latency_ratio.dat" every 5::4::24 title "All" lc rgb "black" ps 1 pt 10 with linespoints

################################################# ucb ####################################################

#ucb_throughput_core
reset
set size 0.99,1.0
set term pdf font ",10" size 5.2, 2.4
set ylabel "Throughput (op/s)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' "Number of cores"
#set yrange [0:40]
#set xrange [0:16]
set output "../graphs/single/Figure_ucb_throughput_core.pdf"
set key font ",18" reverse Left top

plot  "../dat/figure_ucb_throughput_core.dat" title "UCB" lc rgb "black" ps 1 pt 8 with linespoints


#ucb_throughput_ratio
reset
set size 0.99,1.0
set term pdf font ",10" size 5.2, 2.4
set ylabel "Throughput (op/s)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' "UDI ratio"
#set yrange [0:40]
#set xrange [0:16]
set output "../graphs/single/Figure_ucb_throughput_ratio.pdf"
set key font ",18" reverse Left top

plot  "../dat/figure_ucb_throughput_ratio.dat" title "UCB" lc rgb "black" ps 1 pt 5 with linespoints

#ucb_latency_core
reset
set size 0.99,1.0
set term pdf font ",10" size 5.2, 2.4
set ylabel "latency (ms)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' " number of cores"
#set yrange [0:2900000]
#set xrange [0:20]
set output "../graphs/single/Figure_ucb_latency_core.pdf"
set key font ",18" reverse Left top

plot  "../dat/figure_ucb_latency_core.dat" every 5::0::20 title "Q" lc rgb "black" ps 1 pt 6 with linespoints,\
      "../dat/figure_ucb_latency_core.dat" every 5::1::21  title "U" lc rgb "black" ps 1 pt 7 with linespoints, \
      "../dat/figure_ucb_latency_core.dat" every 5::2::22 title "I" lc rgb "black" ps 1 pt 8 with linespoints, \
      "../dat/figure_ucb_latency_core.dat" every 5::3::23 title "D" lc rgb "black" ps 1 pt 9 with linespoints,\
      "../dat/figure_ucb_latency_core.dat" every 5::4::24 title "All" lc rgb "black" ps 1 pt 10 with linespoints

#ucb_latency_ratio
reset
set size 0.99,1.0
set term pdf font ",10" size 5.2, 2.4
set ylabel "latency (ms)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' "UDI ratio"
#set yrange [0:2900000]
#set xrange [0:20]
set output "../graphs/single/Figure_ucb_latency_ratio.pdf"
set key font ",18" reverse Left top

plot  "../dat/figure_ucb_latency_ratio.dat" every 5::0::20 title "Q" lc rgb "black" ps 1 pt 6 with linespoints,\
      "../dat/figure_ucb_latency_ratio.dat" every 5::1::21 title "U" lc rgb "black" ps 1 pt 7 with linespoints, \
      "../dat/figure_ucb_latency_ratio.dat" every 5::2::22 title "I" lc rgb "black" ps 1 pt 8 with linespoints,\
      "../dat/figure_ucb_latency_ratio.dat" every 5::3::23 title "D" lc rgb "black" ps 1 pt 9 with linespoints,\
      "../dat/figure_ucb_latency_ratio.dat" every 5::4::24 title "All" lc rgb "black" ps 1 pt 10 with linespoints

################################################# ub ####################################################
#ub_throughput_core
reset
set size 0.99,1.0
set term pdf font ",10" size 5.2, 2.4
set ylabel "Throughput (op/s)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' "Number of cores"
#set yrange [0:40]
#set xrange [0:16]
set output "../graphs/single/Figure_ub_throughput_core.pdf"
set key font ",18" reverse Left top

plot  "../dat/figure_ub_throughput_core.dat" title "UpBit" lc rgb "black" ps 1 pt 8 with linespoints


#ub_throughput_ratio
reset
set size 0.99,1.0
set term pdf font ",10" size 5.2, 2.4
set ylabel "Throughput (op/s)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' "UDI ratio"
#set yrange [0:40]
#set xrange [0:16]
set output "../graphs/single/Figure_ub_throughput_ratio.pdf"
set key font ",18" reverse Left top

plot  "../dat/figure_ub_throughput_ratio.dat" title "UpBit" lc rgb "black" ps 1 pt 5 with linespoints

#ub_latency_core
reset
set size 0.99,1.0
set term pdf font ",10" size 5.2, 2.4
set ylabel "latency (ms)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' " number of cores"
#set yrange [0:2900000]
#set xrange [0:20]
set output "../graphs/single/Figure_ub_latency_core.pdf"
set key font ",18" reverse Left top

plot  "../dat/figure_ub_latency_core.dat" every 5::0::20 title "Q" lc rgb "black" ps 1 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::1::21  title "U" lc rgb "black" ps 1 pt 7 with linespoints, \
      "../dat/figure_ub_latency_core.dat" every 5::2::22 title "I" lc rgb "black" ps 1 pt 8 with linespoints, \
      "../dat/figure_ub_latency_core.dat" every 5::3::23 title "D" lc rgb "black" ps 1 pt 9 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::4::24 title "All" lc rgb "black" ps 1 pt 10 with linespoints

#ub_latency_ratio
reset
set size 0.99,1.0
set term pdf font ",10" size 5.2, 2.4
set ylabel "latency (ms)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' "UDI ratio"
#set yrange [0:2900000]
#set xrange [0:20]
set output "../graphs/single/Figure_ub_latency_ratio.pdf"
set key font ",18" reverse Left top

plot  "../dat/figure_ub_latency_ratio.dat" every 5::0::20 title "Q" lc rgb "black" ps 1 pt 6 with linespoints,\
      "../dat/figure_ub_latency_ratio.dat" every 5::1::21 title "U" lc rgb "black" ps 1 pt 7 with linespoints, \
      "../dat/figure_ub_latency_ratio.dat" every 5::2::22 title "I" lc rgb "black" ps 1 pt 8 with linespoints,\
      "../dat/figure_ub_latency_ratio.dat" every 5::3::23 title "D" lc rgb "black" ps 1 pt 9 with linespoints,\
      "../dat/figure_ub_latency_ratio.dat" every 5::4::24 title "All" lc rgb "black" ps 1 pt 10 with linespoints

################################################# cubit ####################################################
#cubit_throughput_core
reset
set size 0.99,1.0
set term pdf font ",10" size 5.2, 2.4
set ylabel "Throughput (op/s)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' "Number of cores"
#set yrange [0:40]
#set xrange [0:16]
set output "../graphs/single/Figure_cubit_throughput_core.pdf"
set key font ",18" reverse Left top

plot  "../dat/figure_cubit_throughput_core.dat" title "cubit" lc rgb "black" ps 1 pt 8 with linespoints


#cubit_throughput_ratio
reset
set size 0.99,1.0
set term pdf font ",10" size 5.2, 2.4
set ylabel "Throughput (op/s)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' "UDI ratio"
#set yrange [0:40]
#set xrange [0:16]
set output "../graphs/single/Figure_cubit_throughput_ratio.pdf"
set key font ",18" reverse Left top

plot  "../dat/figure_cubit_throughput_ratio.dat" title "cubit" lc rgb "black" ps 1 pt 5 with linespoints

#cubit_latency_core
reset
set size 0.99,1.0
set term pdf font ",10" size 5.2, 2.4
set ylabel "latency (ms)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' " number of cores"
#set yrange [0:2900000]
#set xrange [0:20]
set output "../graphs/single/Figure_cubit_latency_core.pdf"
set key font ",18" reverse Left top

plot  "../dat/figure_cubit_latency_core.dat" every 5::0::20 title "Q" lc rgb "black" ps 1 pt 6 with linespoints,\
      "../dat/figure_cubit_latency_core.dat" every 5::1::21  title "U" lc rgb "black" ps 1 pt 7 with linespoints, \
      "../dat/figure_cubit_latency_core.dat" every 5::2::22 title "I" lc rgb "black" ps 1 pt 8 with linespoints, \
      "../dat/figure_cubit_latency_core.dat" every 5::3::23 title "D" lc rgb "black" ps 1 pt 9 with linespoints,\
      "../dat/figure_cubit_latency_core.dat" every 5::4::24 title "All" lc rgb "black" ps 1 pt 10 with linespoints

#cubit_latency_ratio
reset
set size 0.99,1.0
set term pdf font ",10" size 5.2, 2.4
set ylabel "latency (ms)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' "UDI ratio"
#set yrange [0:2900000]
#set xrange [0:20]
set output "../graphs/single/Figure_cubit_latency_ratio.pdf"
set key font ",18" reverse Left top

plot  "../dat/figure_cubit_latency_ratio.dat" every 5::0::20 title "Q" lc rgb "black" ps 1 pt 6 with linespoints,\
      "../dat/figure_cubit_latency_ratio.dat" every 5::1::21 title "U" lc rgb "black" ps 1 pt 7 with linespoints, \
      "../dat/figure_cubit_latency_ratio.dat" every 5::2::22 title "I" lc rgb "black" ps 1 pt 8 with linespoints,\
      "../dat/figure_cubit_latency_ratio.dat" every 5::3::23 title "D" lc rgb "black" ps 1 pt 9 with linespoints,\
      "../dat/figure_cubit_latency_ratio.dat" every 5::4::24 title "All" lc rgb "black" ps 1 pt 10 with linespoints