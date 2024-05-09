#all_throughput_cofont 'Linux Libertine O,15're
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel offset -1,0,0 "Throughput (op/s)" font 'Linux Libertine O,29'
set xlabel offset 0,0.6,0 font 'Linux Libertine O,29' "Number of worker threads"
set xtics ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32)
set yrange [0:19000]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs_earth/multiple/Figure_multiple_throughput_core.png"
set key font ",25" reverse Left top left
set xtics offset 0,0.2,0 font ',29' #("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
set ytics offset 0.5,0,0 font ',29' ("0" 0,"4k" 4000, "8k" 8000, "12k" 12000, "16k" 16000) 

plot  "../dat/figure_cubit-lk_throughput_core.dat" title "CUBIT" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_throughput_core.dat" title "UpBit^{+}" lc rgb "sea-green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_throughput_core.dat" title "In-place^{+}" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_throughput_core.dat" title "UCB^{+}" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set label 2 "{/:Bold (b)}" at 15, 16900 font 'Linux Libertine O,29'

set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set lmargin 8  
set rmargin 0.2 
set tmargin 0.3  
set output "../graphs_earth/multiple/Figure_multiple_throughput_core.eps"
replot
