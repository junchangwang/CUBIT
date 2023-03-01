#Needs: ../dat/upbit_vs_scan.dat
#Figure 18

set style line 1 lc rgb 'black' lt 0 lw 2 pt 4 ps 3
set style line 3 lc rgb 'black' lt 0 lw 2 pt 6 ps 3
set style line 5 lc rgb 'black' lt 0 lw 2 pt 2 ps 3
set style line 8 lc rgb 'black' lt 1 lw 2 pt -1

set origin 0,0
set size 1.0,0.97
set boxwidth 0.5

set key font ",18" bottom right

set ylabel "Latency (ms)" offset 0.5,0,0
set xlabel "Selectivity"

set xrange [0.06:100]
#set yrange [1:100000]
set format y "10^{%T}";
set format x "%0.f%%";
set term pdf font ",12"
set output "../graphs/Figure18.pdf"

set logscale y
set logscale x
set xtics nomirror
set ytics nomirror

set label '1 M' at 0.1,10**3 center
set label '10 M' at 1,10**3 center
set label '100 M' at 10,0.5*10**4 center
set label '0.5 B' at 50,2*10**4 center

set xtics ("0.1%%" 0.1, "" 0.2, "" 0.3, "" 0.4, "" 0.5, "" 0.6, "" 0.7, "" 0.8, "" 0.9, "1%%" 1, "" 2, "" 3, "" 4, "" 5, "" 6, "" 7, "" 8, "" 9, "10%%" 10, "" 20, "" 30, "" 40, "" 50, "" 60, "" 70, "" 80, "" 90, "100%%" 100)

plot \
     "../dat/upbit_vs_scan.dat" using ($1):($3) title "UpBit (equality query)" ls 1 with lp, \
     "../dat/upbit_vs_scan.dat" using ($1):($2) title "UpBit (range query)" ls 3 with lp, \
     "../dat/upbit_vs_scan.dat" using ($1):(1000*$4) title "Scan" ls 5 with lp
