set term eps
set output "../graphs_WPMT/Figure_for_WPMT.eps"

set yrange [0:53] # 28 for ratio=0.1; 53 for ratio=0.2
set y2range [0:]
set style data histograms
set boxwidth 1 absolute
# set logscale y
set y2label "Update Latency (ms)" offset 1,0,0
set ylabel "Read Latency (ms)" offset -0.7,0,0
set xlabel "Workers per merging thread" offset 0,-1.7,0 
set origin 0,0.05
set size 1.0,0.95

set key font ",18" 
set xtics font ",17"
set ytics font ",17"
set y2tics font ",17"
set xlabel font ",25"
set ylabel font ",25"
set y2label font ",25"

set bmargin 2.5
set label "1" at screen 0.18, screen 0.11 font ",17"
set label "2" at screen 0.33, screen 0.11 font ",17"
set label "4" at screen 0.47, screen 0.11 font ",17"
set label "8" at screen 0.63, screen 0.11 font ",17"
set label "16" at screen 0.76, screen 0.11 font ",17"

unset xtics
set ytics nomirror

set key reverse Left top

plot '../dat/figure_cubit-lf_latency_WPMT.dat' every 2    using ($2):($1) t "Read" with boxes fs solid 1.0 border rgb "black" lc rgb "black", \
     '../dat/figure_cubit-lf_latency_WPMT.dat' every 2::1 using ($2):($1) t "Update" with boxes fill pattern 7 border rgb "black" lc rgb "black" axes x1y2
