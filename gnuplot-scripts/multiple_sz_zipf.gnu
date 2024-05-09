#all_throughput_core
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -0.8,0,0 "Throughput (op/s)" font 'Linux Libertine O,29'
set xlabel offset 0,0.8,0 font 'Linux Libertine O,29' "Number of cores"
set xtics ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32)
set yrange [0:1800]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs_zipf/multiple/Figure_multiple_throughput_core.png"
set key font ",23" reverse Left top left # font ",25"
set xtics offset 0,0.2,0 font ',29' #("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
set ytics offset 0.5,0,0 font ',29' ("0" 0,"500" 500, "1000" 1000, "1500" 1500) 


plot  "../dat/figure_cubit-lk_throughput_core.dat" title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_throughput_core.dat" title "UpBit" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_throughput_core.dat" title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_throughput_core.dat" title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set output "../graphs_zipf/multiple/Figure_multiple_throughput_core.eps"
replot


#all_latency_core_Query
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -0.8,0,0 "Query latency (ms)" font 'Linux Libertine O,29'
set xlabel offset 0,0.8,0 font 'Linux Libertine O,29' "Number of cores"
set xtics ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32)
set yrange [0:150]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs_zipf/multiple/Figure_multiple_latency_core_Query.png"
set key font ",25" reverse Left top left
set xtics offset 0,0.2,0 font ',29' #("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
set ytics offset 0.5,0,0 font ',29' ("0" 0,"50" 50, "100" 100, "150" 150)  


plot  "../dat/figure_cubit-lk_latency_core.dat" every 5::0::30 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::0::30 title "UpBit" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_core.dat" every 5::0::30 title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_core.dat" every 5::0::30 title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set output "../graphs_zipf/multiple/Figure_multiple_latency_core_Query.eps"
replot

#all_latency_core_Update
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -0.8,0,0 "Update latency (ms)" font 'Linux Libertine O,29'
set xlabel offset 0,0.8,0 font 'Linux Libertine O,29' "Number of cores"
set xtics ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32)
set yrange [0:600]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs_zipf/multiple/Figure_multiple_latency_core_Update.png"
set key font ",25" reverse Left top left
set xtics offset 0,0.2,0 font ',29' #("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
set ytics offset 0.5,0,0 font ',29' ("0" 0,"200" 200, "400" 400, "600" 600) 


plot  "../dat/figure_cubit-lk_latency_core.dat" every 5::1::31 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::1::31 title "UpBit" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_core.dat" every 5::1::31 title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_core.dat" every 5::1::31 title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set output "../graphs_zipf/multiple/Figure_multiple_latency_core_Update.eps"
replot

#all_latency_core_Insert
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -0.8,0,0 "Insert latency (ms)" font 'Linux Libertine O,29'
set xlabel offset 0,0.8,0 font 'Linux Libertine O,29' "Number of cores"
set xtics ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32)
set yrange [0:600]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs_zipf/multiple/Figure_multiple_latency_core_Insert.png"
set key font ",25" reverse Left top left
set xtics offset 0,0.2,0 font ',29' #("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
set ytics offset 0.5,0,0 font ',29' ("0" 0,"200" 200, "400" 400, "600" 600) 


plot  "../dat/figure_cubit-lk_latency_core.dat" every 5::2::32 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::2::32 title "UpBit" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_core.dat" every 5::2::32 title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_core.dat" every 5::2::32 title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set output "../graphs_zipf/multiple/Figure_multiple_latency_core_Insert.eps"
replot

#all_latency_core_Delete
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -0.8,0,0 "Delete latency (ms)" font 'Linux Libertine O,29'
set xlabel offset 0,0.8,0 font 'Linux Libertine O,29' "Number of cores"
set xtics ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32)
set yrange [0:600]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs_zipf/multiple/Figure_multiple_latency_core_Delete.png"
set key font ",25" reverse Left top left
set xtics offset 0,0.2,0 font ',29' #("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
set ytics offset 0.5,0,0 font ',29' ("0" 0,"200" 200, "400" 400, "600" 600) 


plot  "../dat/figure_cubit-lk_latency_core.dat" every 5::3::33 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::3::33 title "UpBit" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_core.dat" every 5::3::33 title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_core.dat" every 5::3::33 title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set output "../graphs_zipf/multiple/Figure_multiple_latency_core_Delete.eps"
replot

#all_latency_core_Overall
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -0.8,0,0 "Overall latency (ms)" font 'Linux Libertine O,29'
set xlabel offset 0,0.8,0 font 'Linux Libertine O,29' "Number of cores"
set xtics ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32)
set yrange [0:600]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs_zipf/multiple/Figure_multiple_latency_core_Overall.png"
set key font ",25" reverse Left top left
set xtics offset 0,0.2,0 font ',29' #("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
set ytics offset 0.5,0,0 font ',29' ("0" 0,"200" 200, "400" 400, "600" 600) 


plot  "../dat/figure_cubit-lk_latency_core.dat" every 5::4::34 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::4::34 title "UpBit" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_core.dat" every 5::4::34 title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_core.dat" every 5::4::34 title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set output "../graphs_zipf/multiple/Figure_multiple_latency_core_Overall.eps"
replot
