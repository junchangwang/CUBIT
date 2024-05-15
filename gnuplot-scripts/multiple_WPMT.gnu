set terminal eps size 5, 3 font 'Linux Libertine O,20' 
set output "../graphs_WPMT/Figure_for_WPMT.eps"

# For 20% figure
# set yrange [0:32] # 
# set y2range [0:3.8] # 
set yrange [0:26] # 
set y2range [0:2.5] # 
set style data histograms
set boxwidth 1 absolute
# set logscale y
set ylabel "Query Latency (ms)" offset -0.7,0,0 font ",29"
set y2label "UDI Latency (ms)" offset -0.8,0,0 font ",29"
set xlabel "Workers per merging thread" offset 0,-0.8,0  font ",29"
set origin 0,0.05
set size 1.0,0.95

set key font ",25" 
set xtics font ",29"
set ytics font ",29"
set y2tics font ",29" offset -0.7,0,0

set label "1" at screen 0.18, screen 0.17 font ",29"
set label "2" at screen 0.33, screen 0.17 font ",29"
set label "4" at screen 0.48, screen 0.17 font ",29"
set label "8" at screen 0.63, screen 0.17 font ",29"
set label "16" at screen 0.76, screen 0.17 font ",29"

set label 17 "{/:Bold (a)}" at 6.5, 24 font 'Linux Libertine O,29'
# set label 17 "{/:Bold (b)}" at 7.5, 29 font 'Linux Libertine O,29'

set lmargin 7
set rmargin 8.2
set tmargin 0.4 
set bmargin 2.2

unset xtics
set ytics nomirror

# set xtics offset 0,0.2,0 font ',24' #("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32) 
# set ytics ("0" 0,"4" 4, "8" 8, "12" 12 ,"16" 16) 
set ytics ("0" 0, "8" 8, "16" 16 ,"24" 24) 
# set y2tics ("0" 0,"4" 4, "8" 8, "12" 12 ) 

set key top

plot '../dat/figure_cubit-lf_latency_WPMT.dat' every 2    using ($2):($1) t "Query" with boxes fs solid 1.0 border rgb "black" lc rgb "black", \
     '../dat/figure_cubit-lf_latency_WPMT.dat' every 2::1 using ($2):($1) t "UDI" with boxes fill pattern 7 border rgb "black" lc rgb "black" axes x1y2
