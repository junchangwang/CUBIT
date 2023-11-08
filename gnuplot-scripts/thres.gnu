#Needs: dat/thres.dat dat/thres_0.2.dat dat/threshold_normalized.dat
#Figure 19a, 19b, 19c


set term pdf size 4, 2
set output "../graphs/Figure19b.pdf"

set yrange [0:0.7]
set y2range [0:15]
set style data histograms
set boxwidth 0.85 absolute
# set logscale y
set ylabel "Update Latency (ms)" offset -1,0,0
set y2label "Read Latency (ms)" offset 1,0,0
set xlabel "Merging threshold" offset 0,-1.7,0
set origin 0,0.05
set size 1.0,0.95

set key font ",18" font ",10"
set xtics font ",10"
set ytics font ",10"
set y2tics font ",10"
set xlabel font ",10"
set ylabel font ",10"
set y2label font ",10"
set label font ",10"

set bmargin 2.5
set label "1" at screen 0.14, screen 0.11 font ",10"
set label "5" at screen 0.23, screen 0.11 font ",10"
set label "10" at screen 0.31, screen 0.11 font ",10"
set label "20" at screen 0.39, screen 0.11 font ",10"
set label "50" at screen 0.48, screen 0.11 font ",10"
set label "100" at screen 0.56, screen 0.11 font ",10"
set label "200" at screen 0.64, screen 0.11 font ",10"
set label "500" at screen 0.73, screen 0.11 font ",10"
set label "1000" at screen 0.81, screen 0.11 font ",10"

unset xtics
set ytics nomirror

plot '../dat/thres.dat' every 2    using ($2):($1/1e3) t "Update" with boxes fs solid 1.0 border rgb "black" lc rgb "black", \
     '../dat/thres.dat' every 2::1 using ($2):($1/1e3) t "Read" with boxes fill pattern 2 border rgb "black" lc rgb "black" axes x1y2



reset
set term pdf size 4, 2
set output "../graphs/Figure19a.pdf"

set yrange [0:0.7]
set y2range [0:15]
set style data histograms
set boxwidth 0.85 absolute
# set logscale y
set ylabel "Update Latency (ms)" offset -1,0,0
set y2label "Read Latency (ms)" offset 1,0,0
set xlabel "Merging threshold" offset 0,-1.7,0

set origin 0,0.05
set size 1.0,0.95

set key font ",18" font ",10"
set xtics font ",10"
set ytics font ",10"
set y2tics font ",10"
set xlabel font ",10"
set ylabel font ",10"
set y2label font ",10"
set label font ",10"

set bmargin 2.5
set label "1" at screen 0.14, screen 0.11 font ",10"
set label "5" at screen 0.22, screen 0.11 font ",10"
set label "10" at screen 0.30, screen 0.11 font ",10"
set label "20" at screen 0.39, screen 0.11 font ",10"
set label "50" at screen 0.48, screen 0.11 font ",10"
set label "100" at screen 0.56, screen 0.11 font ",10"
set label "200" at screen 0.65, screen 0.11 font ",10"
set label "500" at screen 0.73, screen 0.11 font ",10"
set label "1000" at screen 0.81, screen 0.11 font ",10"


unset xtics
set ytics nomirror

plot '../dat/thres_0.2.dat' every 2    using ($2):($1/1e3) t "Update" with boxes fs solid 1.0 border rgb "black" lc rgb "black", \
     '../dat/thres_0.2.dat' every 2::1 using ($2):($1/1e3) t "Read" with boxes fill pattern 2 border rgb "black" lc rgb "black" axes x1y2








reset

set style line 1 lc rgb 'black' lt 1 lw 2 pt -1
set style line 2 lc rgb 'black' lt 0 lw 2 pt 1 ps 1.5
set style line 3 lc rgb 'black' lt 0 lw 2 pt 6 ps 1.5
set style line 4 lc rgb 'black' lt 0 lw 2 pt 2 ps 1.5
set style line 5 lc rgb 'black' lt -1 lw 2


set term pdf size 4, 2
set output "../graphs/Figure19c.pdf"

set key font ",18" bottom right


set yrange [0:1.2]
set xrange [1:1000]
set logscale x
#set style data histograms
#set boxwidth 0.85 absolute
# set logscale y
set ylabel "Normalized Latency" offset -1,0,0
set xlabel "Merging threshold"


set key font ",18" font ",10"
set xtics font ",10"
set ytics font ",10"
set xlabel font ",10"
set ylabel font ",10"
#set label font ",10"

set bmargin 4

#set origin 0,0.05
set size 0.99,0.98




#unset xtics
#set ytics nomirror

plot '../dat/threshold_normalized.dat' using 1:2 t "Workload with 50% updates" ls 2 w lp, \
     '../dat/threshold_normalized.dat' using 1:3 t "Workload with 25% updates" ls 3 w lp
