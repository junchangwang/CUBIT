set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set output "../graphs_WPMT/Figure_for_WPMT.eps"

set yrange [0:24] # 
set y2range [0:2.2] # 
set style data histograms
set boxwidth 1 absolute
# set logscale y
set y2label "UDI Latency (ms)" offset 1,0,0
set ylabel "Query Latency (ms)" offset -0.6,0,0
set xlabel "Workers per merging thread" offset 0,-0.8,0 
set origin 0,0.05
set size 1.0,0.95

set key font ",25" 
set xtics font ",29"
set ytics font ",29"
set y2tics font ",29"
set xlabel font ",29"
set ylabel font ",29"
set y2label font ",29"

set bmargin 2.5
set label "1" at screen 0.18, screen 0.17 font ",29"
set label "2" at screen 0.31, screen 0.17 font ",29"
set label "4" at screen 0.45, screen 0.17 font ",29"
set label "8" at screen 0.59, screen 0.17 font ",29"
set label "16" at screen 0.70, screen 0.17 font ",29"

unset xtics
set ytics nomirror

# set xtics offset 0,0.2,0 font ',24' #("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
# set ytics ("0" 0,"4" 4, "8" 8, "12" 12 ,"16" 16) 
set ytics ("0" 0, "8" 8, "16" 16 ,"24" 24) #for 0.2
# set y2tics ("0" 0,"4" 4, "8" 8, "12" 12 ) 

set key reverse Left top

plot '../dat/figure_nbub-lf_latency_WPMT.dat' every 2    using ($2):($1) t "Query" with boxes fs solid 1.0 border rgb "black" lc rgb "black", \
     '../dat/figure_nbub-lf_latency_WPMT.dat' every 2::1 using ($2):($1) t "UDI" with boxes fill pattern 7 border rgb "black" lc rgb "black" axes x1y2
