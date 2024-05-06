#all_throughput_core
reset
set size 0.99,1
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -1.4,0  "Throughput (op/s)" font 'Linux Libertine O,29'
set xlabel offset 0,0.6,0 font 'Linux Libertine O,29' "Number of worker threads" 
#1, 2, 4, 8, 16, 24, 32
set xtics offset 0,0.2,0 font ',29' ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
set ytics offset 0.5,0,0 font ',29' ("0" 0, "500" 500, "1000" 1000, "1500" 1500, "2000" 2000) 
set yrange [0:2200]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 500, 600
set output "../graphs/multiple/Figure_multiple_throughput_core.png"
set key font ",25" reverse Left top left

plot  "../dat/figure_cubit-lk_throughput_core.dat" title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_throughput_core.dat" title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_throughput_core.dat" title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_throughput_core.dat" title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints


set terminal eps size 4.5, 2.8 font 'Linux Libertine O,20' 
set lmargin 9.3 
set rmargin 0.2 
set tmargin 0.3  
set output "../graphs/multiple/Figure_multiple_throughput_core.eps"
replot

#all_throughput_ratio
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -1.4,0,0  "Throughput (op/s)" font 'Linux Libertine O,29'
set xlabel offset 0,0.6,0 font 'Linux Libertine O,29' "UDI ratio"
set yrange [0:2000]
set xrange [0:]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs/multiple/Figure_multiple_throughput_ratio.png"
set key horizontal font ",25" reverse Left top
set xtics offset 0,0.2,0 font ',29' ("0" 0, "" 0.01, "2%%" 0.02, "5%%" 0.05, "10%%" 0.1, "20%%" 0.2) 
set ytics offset 0.5,0,0 font ',29' ("0" 0, "500" 500, "1000" 1000, "1500" 1500, "2000" 2000) 

plot "../dat/figure_cubit-lk_throughput_ratio.dat" title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
    "../dat/figure_ub_throughput_ratio.dat" title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
    "../dat/figure_naive_throughput_ratio.dat" title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
    "../dat/figure_ucb_throughput_ratio.dat" title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set label 2 "{/:Bold (a)}" at 0.175, 850 font 'Linux Libertine O,29'

set terminal eps size 4.5, 2.8 font 'Linux Libertine O,20' 
set lmargin 9.2 
set rmargin 1.6
set tmargin 0.3  
set output "../graphs/multiple/Figure_multiple_throughput_ratio.eps"
replot


#all_latency_core_Query
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -1.1,0,0 "Query latency (ms)" font 'Linux Libertine O,29'
set xlabel offset 0,0.6,0 font 'Linux Libertine O,29' "Number of worker threads"
set xtics ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32)
set yrange [0:260]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs/multiple/Figure_multiple_latency_core_Query.png"
set key font ",25" reverse Left top left
set xtics offset 0,0.2,0 font ',29' #("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
set ytics offset 0.5,0,0 font ',29' ("0" 0,"50" 50, "100" 100, "150" 150, "200" 200, "250" 250) 

plot  "../dat/figure_cubit-lk_latency_core.dat" every 5::0::30 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::0::30 title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_core.dat" every 5::0::30 title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_core.dat" every 5::0::30 title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 4.5, 2.8 font 'Linux Libertine O,20' 
set lmargin 8 
set rmargin 0.2 
set tmargin 0.3  
set output "../graphs/multiple/Figure_multiple_latency_core_Query.eps"
replot

#all_latency_core_Update
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -0.6,0,0  "Update latency (ms)" font 'Linux Libertine O,29'
set xlabel offset 0,0.6,0 font 'Linux Libertine O,29' "Number of worker threads"
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


plot  "../dat/figure_cubit-lk_latency_core.dat" every 5::1::31 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::1::31 title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_core.dat" every 5::1::31 title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_core.dat" every 5::1::31 title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints
      

set terminal eps size 4.5, 2.8 font 'Linux Libertine O,20' 
set lmargin 8.4   
set rmargin 0.2 
set tmargin 0.6  
set output "../graphs/multiple/Figure_multiple_latency_core_Update.eps"
replot

#all_latency_core_Insert
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -0.8,0,0  "Insert latency (ms)" font 'Linux Libertine O,29'
set xlabel offset 0,0.6,0 font 'Linux Libertine O,29' "Number of worker threads"
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


plot  "../dat/figure_cubit-lk_latency_core.dat" every 5::2::32 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::2::32 title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_core.dat" every 5::2::32 title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_core.dat" every 5::2::32 title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 4.5, 2.8 font 'Linux Libertine O,20' 
set lmargin 8.6  
set rmargin 0.2 
set tmargin 0.6  
set output "../graphs/multiple/Figure_multiple_latency_core_Insert.eps"
replot

#all_latency_core_Delete
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -0.8,0,0 "Delete latency (ms)" font 'Linux Libertine O,29'
set xlabel offset 0,0.8,0 font 'Linux Libertine O,29' "Number of worker threads"
set xtics ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32)
set yrange [0:1500]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs/multiple/Figure_multiple_latency_core_Delete.png"
set key font ",25" reverse Left top left
set xtics offset 0,0.2,0 font ',29' #("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
set ytics offset 0.5,0,0 font ',29' ("0" 0, "500" 500, "1000" 1000, "1500" 1500)


plot  "../dat/figure_cubit-lk_latency_core.dat" every 5::3::33 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::3::33 title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_core.dat" every 5::3::33 title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_core.dat" every 5::3::33 title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set output "../graphs/multiple/Figure_multiple_latency_core_Delete.eps"
replot

#all_latency_core_Overall
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -0.8,0,0 "Overall latency (ms)" font 'Linux Libertine O,29'
set xlabel offset 0,0.8,0 font 'Linux Libertine O,29' "Number of worker threads"
set xtics ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32)
set yrange [0:1500]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs/multiple/Figure_multiple_latency_core_Overall.png"
set key font ",25" reverse Left top left
set xtics offset 0,0.2,0 font ',29' #("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
set ytics offset 0.5,0,0 font ',29' ("0" 0, "500" 500, "1000" 1000, "1500" 1500) 


plot  "../dat/figure_cubit-lk_latency_core.dat" every 5::4::34 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::4::34 title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_core.dat" every 5::4::34 title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_core.dat" every 5::4::34 title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set output "../graphs/multiple/Figure_multiple_latency_core_Overall.eps"
replot


#all_latency_ratio_Query
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -1,0,0 "Query latency (ms)" font 'Linux Libertine O,29'
set xlabel offset 0,0.6,0 font 'Linux Libertine O,29' "UDI ratio"
set yrange [0:250]
set xrange [0:]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs/multiple/Figure_multiple_latency_ratio_Query.png"
set key font ",25" reverse Left top left
set xtics offset 0,0.2,0 font ',29' ("0" 0, "" 0.01, "2%%" 0.02, "5%%" 0.05, "10%%" 0.1, "20%%" 0.2) 
set ytics offset 0.5,0,0 font ',29' ("0" 0,"50" 50, "100" 100, "150" 150, "200" 200, "250" 250) 


plot  "../dat/figure_cubit-lk_latency_ratio.dat" every 5::0::30 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_ratio.dat" every 5::0::30 title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_ratio.dat" every 5::0::30 title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_ratio.dat" every 5::0::30 title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set label 2 "{/:Bold (b)}" at 0.09, 230 font 'Linux Libertine O,29'

set terminal eps size 4.5, 2.8 font 'Linux Libertine O,20' 
set lmargin 8 
set rmargin 1.6 
set tmargin 0.3  
set output "../graphs/multiple/Figure_multiple_latency_ratio_Query.eps"
replot

#all_latency_ratio_Update
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -0.8,0,0 "Update latency (ms)" font 'Linux Libertine O,29'
set xlabel offset 0,0.8,0 font 'Linux Libertine O,29' "UDI ratio"
set yrange [0:2500]
set xrange [0:]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs/multiple/Figure_multiple_latency_ratio_Update.png"
set key font ",25" reverse Left top
set xtics offset 0,0.2,0 font ',29' ("0" 0, "0.05" 0.05, "0.1" 0.1, "0.15" 0.15, "0.2" 0.2) 
set ytics offset 0.5,0,0 font ',29' ("0" 0,"800" 800, "1600" 1600, "2400" 2400) 


plot  "../dat/figure_cubit-lk_latency_ratio.dat" every 5::1::31 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_ratio.dat" every 5::1::31 title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_ratio.dat" every 5::1::31 title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_ratio.dat" every 5::1::31 title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set output "../graphs/multiple/Figure_multiple_latency_ratio_Update.eps"
replot

#all_latency_ratio_Insert
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -0.8,0,0 "Insert latency (ms)" font 'Linux Libertine O,29'
set xlabel offset 0,0.8,0 font 'Linux Libertine O,29' "UDI ratio"
set yrange [0:]
set xrange [0:]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs/multiple/Figure_multiple_latency_ratio_Insert.png"
set key font ",25" reverse Left top
set xtics offset 0,0.2,0 font ',29' ("0" 0, "0.05" 0.05, "0.1" 0.1, "0.15" 0.15, "0.2" 0.2) 
set ytics offset 0.5,0,0 font ',29' #("0" 0,"200" 200, "400" 400, "600" 600) 


plot  "../dat/figure_cubit-lk_latency_ratio.dat" every 5::2::32 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_ratio.dat" every 5::2::32 title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_ratio.dat" every 5::2::32 title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_ratio.dat" every 5::2::32 title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set output "../graphs/multiple/Figure_multiple_latency_ratio_Insert.eps"
replot

#all_latency_ratio_Delete
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -0.8,0,0 "Delete latency (ms)" font 'Linux Libertine O,29'
set xlabel offset 0,0.8,0 font 'Linux Libertine O,29' "UDI ratio"
set yrange [0:]
set xrange [0:]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs/multiple/Figure_multiple_latency_ratio_Delete.png"
set key font ",25" reverse Left top
set xtics offset 0,0.2,0 font ',29' ("0" 0, "0.05" 0.05, "0.1" 0.1, "0.15" 0.15, "0.2" 0.2) 
set ytics offset 0.5,0,0 font ',29' #("0" 0,"200" 200, "400" 400, "600" 600)  


plot  "../dat/figure_cubit-lk_latency_ratio.dat" every 5::3::33 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_ratio.dat" every 5::3::33 title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_ratio.dat" every 5::3::33 title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_ratio.dat" every 5::3::33 title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set output "../graphs/multiple/Figure_multiple_latency_ratio_Delete.eps"
replot

#all_latency_ratio_All
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -0.8,0,0 "Overall latency (ms)" font 'Linux Libertine O,29'
set xlabel offset 0,0.8,0 font 'Linux Libertine O,29' "UDI ratio"
set yrange [0:]
set xrange [0:]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs/multiple/Figure_multiple_latency_ratio_Overall.png"
set key font ",25" reverse Left top
set xtics offset 0,0.2,0 font ',29' ("0" 0, "0.05" 0.05, "0.1" 0.1, "0.15" 0.15, "0.2" 0.2) 
set ytics offset 0.5,0,0 font ',29' #("0" 0,"200" 200, "400" 400, "600" 600) 


plot  "../dat/figure_cubit-lk_latency_ratio.dat" every 5::4::34 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_ratio.dat" every 5::4::34 title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_ratio.dat" every 5::4::34 title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_ratio.dat" every 5::4::34 title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set output "../graphs/multiple/Figure_multiple_latency_ratio_Overall.eps"
replot

############################################### for cardinality 

#all_throughput_cardinality
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -1.5,0,0  "Throughput (op/s)" font 'Linux Libertine O,29'
set xlabel offset 0,0.6,0 font 'Linux Libertine O,29' "Cardinality"
# 16, 32, 64, 128, 256
set xtics ("16" 16, "    64" 64, "256" 256, "512" 512, "1024" 1024)
set yrange [0:8100]
set xrange [0:1050]

set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs/multiple/Figure_multiple_throughput_cardinality.png"
set key font ",25" reverse Left top left
set xtics offset 0,0.2,0 font ',29' #("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
set ytics offset 0.5,0,0 font ',29' ("0" 0, "2000" 2000, "4000" 4000, "6000" 6000, "8000" 8000)  

set label 2 "{/:Bold (a)}" at 472, 7500 font 'Linux Libertine O,29'

plot  "../dat/figure_cubit-lk_throughput_cardinality.dat" title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_throughput_cardinality.dat" title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_throughput_cardinality.dat" title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_throughput_cardinality.dat" title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 4.5, 2.8 font 'Linux Libertine O,20' 
set lmargin 9.5
set rmargin 1
set tmargin 0.2 
set output "../graphs/multiple/Figure_multiple_throughput_cardinality.eps"
replot

#all_latency_cardinality_Query
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -1,0,0  "Query latency (ms)" font 'Linux Libertine O,29'
set xlabel offset 0,0.6,0 font 'Linux Libertine O,29' "Cardinality"
set xtics ("16" 16, "   32" 32 , "64" 64, "128" 128, "256" 256)
set yrange [0:300]
set xrange [10:260]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs/multiple/Figure_multiple_latency_cardinality_Query.png"
set key font ",25" reverse Left top
set xtics offset 0,0.2,0 font ',29' #("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
set ytics offset 0.5,0,0 font ',29' ("0" 0, "100" 100, "200" 200, "300" 300) 

set label 2 "{/:Bold (b)}" at 118, 270 font 'Linux Libertine O,29'

plot  "../dat/figure_cubit-lk_latency_cardinality.dat" every 5::0::30 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_cardinality.dat" every 5::0::30 title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_cardinality.dat" every 5::0::30 title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_cardinality.dat" every 5::0::30 title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 4.5, 2.8 font 'Linux Libertine O,20' 
set lmargin 8 
set rmargin 1
set tmargin 0.3 
set output "../graphs/multiple/Figure_multiple_latency_cardinality_Query.eps"
replot

#all_latency_cardinality_Update
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -0.8,0,0 "Update latency (ms)" font 'Linux Libertine O,29'
set xlabel offset 0,0.8,0 font 'Linux Libertine O,29' "Cardinality"
set xtics ("16" 16, "   32" 32 , "64" 64, "128" 128, "256" 256)
set yrange [0:]
set xrange [10:260]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs/multiple/Figure_multiple_latency_cardinality_Update.png"
set key font ",25" reverse Left top
set xtics offset 0,0.2,0 font ',29' #("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
set ytics offset 0.5,0,0 font ',29' #("250" 250, "500" 500, "750" 750) 


plot  "../dat/figure_cubit-lk_latency_cardinality.dat" every 5::1::31 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_cardinality.dat" every 5::1::31 title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_cardinality.dat" every 5::1::31 title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_cardinality.dat" every 5::1::31 title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set output "../graphs/multiple/Figure_multiple_latency_cardinality_Update.eps"
replot

#all_latency_cardinality_Insert
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -0.8,0,0 "Insert latency (ms)" font 'Linux Libertine O,29'
set xlabel offset 0,0.8,0 font 'Linux Libertine O,29' "Cardinality"
set xtics ("16" 16, "   32" 32 , "64" 64, "128" 128, "256" 256)
set yrange [0:3000]
set xrange [10:260]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs/multiple/Figure_multiple_latency_cardinality_Insert.png"
set key font ",25" reverse Left top
set xtics offset 0,0.2,0 font ',29' #("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
set ytics offset 0.5,0,0 font ',29' ("0" 0, "1000" 1000, "2000" 2000, "3000" 3000) 


plot  "../dat/figure_cubit-lk_latency_cardinality.dat" every 5::2::32 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_cardinality.dat" every 5::2::32 title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_cardinality.dat" every 5::2::32 title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_cardinality.dat" every 5::2::32 title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set output "../graphs/multiple/Figure_multiple_latency_cardinality_Insert.eps"
replot

#all_latency_cardinality_Delete
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -0.8,0,0 "Delete latency (ms)" font 'Linux Libertine O,29'
set xlabel offset 0,0.8,0 font 'Linux Libertine O,29' "Cardinality"
set xtics ("16" 16, "   32" 32 , "64" 64, "128" 128, "256" 256)
set yrange [0:3000]
set xrange [10:260]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs/multiple/Figure_multiple_latency_cardinality_Delete.png"
set key font ",25" reverse Left top
set xtics offset 0,0.2,0 font ',29' #("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
set ytics offset 0.5,0,0 font ',29' ("0" 0, "1000" 1000, "2000" 2000, "3000" 3000) 


plot  "../dat/figure_cubit-lk_latency_cardinality.dat" every 5::3::33 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_cardinality.dat" every 5::3::33 title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_cardinality.dat" every 5::3::33 title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_cardinality.dat" every 5::3::33 title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set output "../graphs/multiple/Figure_multiple_latency_cardinality_Delete.eps"
replot

#all_latency_cardinality_Overall
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -0.8,0,0 "Overall latency (ms)" font 'Linux Libertine O,29'
set xlabel offset 0,0.8,0 font 'Linux Libertine O,29' "Cardinality"
set xtics ("16" 16, "   32" 32 , "64" 64, "128" 128, "256" 256)
set yrange [0:3000]
set xrange [10:260]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs/multiple/Figure_multiple_latency_cardinality_Overall.png"
set key font ",25" reverse Left top
set xtics offset 0,0.2,0 font ',29' #("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
set ytics offset 0.5,0,0 font ',29' ("0" 0, "1000" 1000, "2000" 2000, "3000" 3000) 


plot  "../dat/figure_cubit-lk_latency_cardinality.dat" every 5::4::34 title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_cardinality.dat" every 5::4::34 title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_cardinality.dat" every 5::4::34 title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_cardinality.dat" every 5::4::34 title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set output "../graphs/multiple/Figure_multiple_latency_cardinality_Overall.eps"
replot
