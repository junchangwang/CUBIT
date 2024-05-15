set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set output "../graphs_Seg/Seg.eps"

set yrange [0:28]
set y2range [0:8] 
set style data histograms
set boxwidth 1 absolute
# set logscale y
set ylabel "Query Latency (ms)" offset 0,0,0 
set y2label "UDI Latency (ms)" offset -1,0,0
set xlabel "Segments per bitvector" offset 0,-0.7,0  
set origin 0,0.05
set size 1.0,0.95

set key font ",25" 
set xtics font ",29"
set ytics font ",29"
set y2tics font ",29" offset -0.8,0,0
set xlabel font ",29"
set ylabel font ",29"
set y2label font ",29"

set label "1" at screen 0.19, screen 0.17 font ",29"
set label "10" at screen 0.33, screen 0.17 font ",29"
set label "100" at screen 0.46, screen 0.17 font ",29"
set label "1,000" at screen 0.6, screen 0.17 font ",29"
set label "10,000" at screen 0.74, screen 0.17 font ",29"

set label 77 "{/:Bold (b)}" at 6.5, 26 font 'Linux Libertine O,29'

unset xtics
set ytics nomirror

set lmargin 7.4
set rmargin 6.8
set tmargin 0.3 
set bmargin 2.2

set key top right

plot '../dat/figure_cubit-lk_latency_Seg.dat' every 2    using ($2):($1) t "Query" with boxes fs solid 1.0 border rgb "black" lc rgb "black", \
     '../dat/figure_cubit-lk_latency_Seg.dat' every 2::1 using ($2):($1) t "UDI" with boxes fill pattern 7 border rgb "black" lc rgb "black" axes x1y2
