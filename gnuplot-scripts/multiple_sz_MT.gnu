set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set output "../graphs_MT/Figure_for_MT.eps"

set yrange [0:35]
set y2range [0:2.4] # 2.4 for ratio=0.1; 2.8 for ratio=0.2
set style data histograms
set boxwidth 1 absolute
# set logscale y
set ylabel "Query Latency (ms)" offset -0.4,0,0 
set y2label "UDI Latency (ms)" offset -1.2,0,0
set xlabel "Merging threshold" offset 0,-0.8,0  
set origin 0,0.05
set size 1.0,0.95

set label 2 "{/:Bold (b)}" at 6.5, 32 font 'Linux Libertine O,29'

set key font ",25" 
set xtics font ",29" 
set ytics font ",29"
set y2tics font ",29" offset -0.8,0,0
set xlabel font ",29"
set ylabel font ",29"
set y2label font ",29"

set label "1" at screen 0.19, screen 0.17 font ",29"
set label "4" at screen 0.33, screen 0.17 font ",29"
set label "16" at screen 0.47, screen 0.17 font ",29"
set label "32" at screen 0.61, screen 0.17 font ",29"
set label "64" at screen 0.76, screen 0.17 font ",29"

unset xtics
set ytics nomirror

set lmargin 7.4
set rmargin 8
set tmargin 0.3 
set bmargin 2.2


set key top right

plot '../dat/figure_cubit-lk_latency_MT.dat' every 2    using ($2):($1) t "Query" with boxes fs solid 1.0 border rgb "black" lc rgb "black", \
     '../dat/figure_cubit-lk_latency_MT.dat' every 2::1 using ($2):($1) t "UDI" with boxes fill pattern 7 border rgb "black" lc rgb "black" axes x1y2
