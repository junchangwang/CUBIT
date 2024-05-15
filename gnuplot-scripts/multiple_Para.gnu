set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set output "../graphs_Para/Para.eps"

set yrange [0:25]
set y2range [0:16] 
set style data histograms
set boxwidth 1 absolute
# set logscale y
set ylabel "Query Latency (ms)" offset -0.2,0,0 
set y2label "UDI Latency (ms)" offset -1,0,0
set xlabel "Number of threads" offset 0,-0.6,0  
set origin 0,0.05
set size 1.0,0.95

set key font ",25" 
set xtics font ",29"
set ytics font ",29"
set y2tics font ",29" offset -1,0,0
set xlabel font ",29"
set ylabel font ",29"
set y2label font ",29"

set label "1" at screen 0.19, screen 0.17 font ",29"
set label "2" at screen 0.34, screen 0.17 font ",29"
set label "4" at screen 0.49, screen 0.17 font ",29"
set label "8" at screen 0.64, screen 0.17 font ",29"
set label "16" at screen 0.78, screen 0.17 font ",29"

set label 77 "{/:Bold (b)}" at 4.5, 23 font 'Linux Libertine O,29'

unset xtics
set ytics nomirror

set lmargin 7.4
set rmargin 7.2
set tmargin 0.4 
set bmargin 2.1

# set key top center
set key at 12.2,24.2

plot '../dat/figure_cubit-lk_latency_Para.dat' every 2    using ($2):($1) t "Query" with boxes fs solid 1.0 border rgb "black" lc rgb "black", \
     '../dat/figure_cubit-lk_latency_Para.dat' every 2::1 using ($2):($1) t "UDI" with boxes fill pattern 7 border rgb "black" lc rgb "black" axes x1y2
