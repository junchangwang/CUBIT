#Needs: ../dat/tpch_sel.dat
#Figure 27

reset
set style line 1 lc rgb 'black' lt 0 lw 2 pt 4 ps 3
set style line 3 lc rgb 'black' lt 0 lw 2 pt 6 ps 3
set style line 5 lc rgb 'black' lt 0 lw 2 pt 2 ps 3
set style line 8 lc rgb 'black' lt 1 lw 2 pt -1

set origin 0,0
set size 1.0,0.97
set boxwidth 0.5

set key font ",18" bottom right

set ylabel "TPC-H Q6 Latency (s)" offset 2,0,0
set xlabel "Selectivity of the quantity clause of TPC-H Query 6"

set xrange [0:100]
#set yrange [1:100000]
set yrange [0:10]
#set format y "10^{%T}";
set format x "%0.f%%";
set term pdf font ",12" size 6, 2.8
set output "../graphs/Figure27.pdf"
set size 1.0,1.01

#set logscale y
#set logscale x
set xtics nomirror
set ytics nomirror

#set label '1 M' at 0.1,10**3 center
#set label '10 M' at 1,10**3 center
#set label '100 M' at 10,0.5*10**4 center
#set label '0.5 B' at 50,2*10**4 center

#set xtics ("2%%" 0.02, "10%%" 0.1, "" 0.2, "" 0.3, "" 0.4, "50%%" 0.5, "" 0.6, "" 0.7, "" 0.8, "" 0.9, "100%%" 1)

plot "../dat/tpch_sel.dat" using ($1*100):($7/1000) title "UpBit (on quantity)" ls 3 with lp, \
     "../dat/tpch_sel.dat" using ($1*100):($6/1000) title "Scan" ls 5 with lp
