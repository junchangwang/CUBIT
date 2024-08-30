#all_throughput_core
reset
set size 0.99,1
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -1.4,0  "Throughput (op/s)" font 'Linux Libertine O,29' textcolor rgb "blue"
set xlabel offset 0,0.6,0 font 'Linux Libertine O,29' "Number of worker threads" textcolor rgb "blue"
#1, 2, 4, 8, 16, 24, 32
set xtics offset 0,0.2,0 font ',29' ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
set ytics offset 0.5,0,0 font ',29' ("0" 0, "500" 500, "1000" 1000, "1500" 1500, "2000" 2000) 
set yrange [0:2200]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 500, 600
set output "../graphs/multiple/Figure_multiple_throughput_core_blue.png"
set key font ",25" reverse Left top left

set border lc rgb "blue"

plot  "../dat/figure_nbub-lk_throughput_core.dat" title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_throughput_core.dat" title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_throughput_core.dat" title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_throughput_core.dat" title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints


set terminal eps size 4.5, 2.8 font 'Linux Libertine O,20' 
set lmargin 9.3 
set rmargin 0.2 
set tmargin 0.3  
set output "../graphs/multiple/Figure_multiple_throughput_core_blue.eps"
replot

#all_latency_core_Query
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -1.1,0,0 "Query latency (ms)" font 'Linux Libertine O,29' textcolor rgb "blue"
set xlabel offset 0,0.6,0 font 'Linux Libertine O,29' "Number of worker threads" textcolor rgb "blue"
set xtics ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32)
set yrange [0:260]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs/multiple/Figure_multiple_latency_core_Query.png"
set key font ",25" reverse Left top left
set xtics offset 0,0.2,0 font ',29' #("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
set ytics offset 0.5,0,0 font ',29' ("0" 0,"50" 50, "100" 100, "150" 150, "200" 200, "250" 250) 

set border lc rgb "blue"

plot  "../dat/figure_nbub-lk_latency_core.dat" every 5::0::30 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::0::30 title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_core.dat" every 5::0::30 title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_core.dat" every 5::0::30 title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 4.5, 2.8 font 'Linux Libertine O,20' 
set lmargin 8 
set rmargin 0.2 
set tmargin 0.3  
set output "../graphs/multiple/Figure_multiple_latency_core_Query_blue.eps"
replot

#all_latency_core_Update
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -0.6,0,0  "Update latency (ms)" font 'Linux Libertine O,29' textcolor rgb "blue"
set xlabel offset 0,0.6,0 font 'Linux Libertine O,29' "Number of worker threads" textcolor rgb "blue"
set xtics ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32)
set logscale y 10
set ytics format "10^{%T}"
set yrange [0:100000]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs/multiple/Figure_multiple_latency_core_Update.png"
set key font ",25" reverse left top horizontal Left
set xtics offset 0,0.2,0 font ',29' #("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
set ytics offset 0.5,0,0 font ',29' #("0" 0,"500" 500, "1000" 1000, "1500" 1500) 

set border lc rgb "blue"

plot  "../dat/figure_nbub-lk_latency_core.dat" every 5::1::31 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::1::31 title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_core.dat" every 5::1::31 title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_core.dat" every 5::1::31 title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints
      

set terminal eps size 4.5, 2.8 font 'Linux Libertine O,20' 
set lmargin 8.4   
set rmargin 0.2 
set tmargin 0.6  
set output "../graphs/multiple/Figure_multiple_latency_core_Update_blue.eps"
replot

#all_latency_core_Insert
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -0.8,0,0  "Insert latency (ms)" font 'Linux Libertine O,29' textcolor rgb "blue"
set xlabel offset 0,0.6,0 font 'Linux Libertine O,29' "Number of worker threads" textcolor rgb "blue"
set xtics ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32)
set logscale y 10
set ytics format "10^{%T}"
set yrange [0.001:300000]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs/multiple/Figure_multiple_latency_core_Insert.png"
set key font ",25" reverse left top horizontal Left
set xtics offset 0,0.2,0 font ',29' #("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
set ytics offset 0.5,0,0 font ',29'  

set border lc rgb "blue"

plot  "../dat/figure_nbub-lk_latency_core.dat" every 5::2::32 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::2::32 title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_core.dat" every 5::2::32 title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_core.dat" every 5::2::32 title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 4.5, 2.8 font 'Linux Libertine O,20' 
set lmargin 8.6  
set rmargin 0.2 
set tmargin 0.6  
set output "../graphs/multiple/Figure_multiple_latency_core_Insert_blue.eps"
replot

