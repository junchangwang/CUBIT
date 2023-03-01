#Needs: dat/intro_U_D.dat dat/intro_EB.dat dat/intro_Q_D.dat dat/intro_Q_A.dat dat/breakdown_throughput.dat dat/figure_1_ucb.dat dat/figure_1_ub.dat

reset
set key font ",18" invert top left #horizontal

set term pdf size 6, 3.6
set output "../graphs/Figure6.pdf"

set origin 0,0.05
set size 1.0,0.95

set yrange [0:]
#set title "Breakdown of Updates"
set style data histograms
set style histogram rowstacked
set boxwidth 0.75 relative
set style fill solid 1.0 border -1
# set xtics nomirror rotate by -30 scale 0


set label "UCB 1st" at screen 0.115, screen 0.03 font ",10"
set label "UCB 1K" at screen 0.30, screen 0.03 font ",10"
set label "UCB 10K" at screen 0.475, screen 0.03 font ",10"
set label "UCB 200K" at screen 0.65, screen 0.03 font ",10"
set label "UCB 400K" at screen 0.835, screen 0.03 font ",10"
set label font ",9"


set ylabel "Latency (ms)" offset -1,0

set key font ",18" font ",11"
set xtics font ",11"
set ytics font ",11"
set xlabel font ",11"
set ylabel font ",11"
set label font ",10"

plot '../dat/intro_U_D.dat' using ($2/1e3):xtic(3) t "Decode EB" with histograms fs solid 1.0 border rgb "black" lc rgb "grey", \
     '../dat/intro_EB.dat' using ($2/1e3):xtic(3) t "Update and Encode EB" with histograms fs solid 1.0 border rgb "black" lc rgb "white", \
     '../dat/intro_Q_D.dat' using ($2/1e3):xtic(3) t "Decode VB" with histograms fs solid 1.0 border rgb "black" lc rgb "black", \
     '../dat/intro_Q_A.dat' using ($2/1e3):xtic(3) t "Bitwise AND with EB" with histograms fill pattern 2 border rgb "black" lc rgb "black"








reset
set term pdf size 6, 2.7
set output "../graphs/Figure5.pdf"

#set yrange [0:30]
# set xrange [-0.3:2.5]
set style data histograms
set boxwidth 0.85 absolute
set ylabel "Latency (ms)" offset -1,0

# set xtics ("In-place" 0.5, "UCB 1st" 3.5, "UCB 1M" 6.5)

set key font ",18" left top horizontal
set origin 0,0.05
set size 1.0,0.95

set key font ",18" font ",11"
set xtics font ",11"
set ytics font ",11"
set xlabel font ",11"
set ylabel font ",11"
set label font ",10"

set label "Overall" at screen 0.21, screen 0.03 font ",11"
set label "Update" at screen 0.48, screen 0.03 font ",11"
set label "Read" at screen 0.765, screen 0.03 font ",11"

unset xtics

plot '../dat/breakdown_throughput.dat' every 2    using 2:($1/1e3) t "In-place" with boxes fs solid 1.0 border rgb "black" lc rgb "black", \
     '../dat/breakdown_throughput.dat' every 2::1 using 2:($1/1e3) t "UCB" with boxes fill pattern 2 border rgb "black" lc rgb "black"






reset
set size 0.99,1.0
set term pdf font ",10" size 5.2, 2.4
set ylabel "Read Latency (ms)" offset 1.5,0,0
set xlabel "# updates (thousands)"
set yrange [0:40]
set xrange [0:400]
set output "../graphs/Figure1.pdf"
set key font ",18" bottom

plot  "../dat/figure_1_ucb.dat" using ($1/1000):($2/1e3) every 200 title "State-of-the-art Update-Optimized Bitmap Index" lc rgb "black" ps 1.5 pt 6 with linespoints

reset
set origin 0,0.0
set size 1.0,1.0
set term pdf font ",12"
set ylabel "Read Latency (ms)" offset 1.5,0,0
set xlabel "# updates (thousands)"
set yrange [0:40]
set xrange [0:400]
set output "../graphs/Figure9.pdf"

set key font ",18" left top

plot  "../dat/figure_1_ucb.dat" using ($1/1000):($2/1e3) every 200 title "UCB" lc rgb "black" ps 3 pt 6 with linespoints, \
      "../dat/figure_1_ub.dat" using ($1/1000):($2/1e3) every 200 title "UpBit" lc rgb "black" ps 3 pt 2 with linespoints
