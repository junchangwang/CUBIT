# Needs:  dat/fp_ub_su.dat  dat/fp_ucb.dat  dat/fp_ub.dat

reset
set term pdf font ",12" size 6, 2.8
set output "../graphs/Figure26.pdf"
set boxwidth 0.5 absolute
set xtics nomirror rotate by -30 scale 0
set ylabel "Latency (ms)" offset 2,0,0
set y2label "Space Overhead (UDI ratio)" offset 0.5,0,0
set logscale y2
set y2tics nomirror
set ytics nomirror

#set yrange [0:16]

set key font ",18" maxrows 1
set xtics font ",9"

set style data histogram
set style histogram cluster gap 1
set style fill solid border -1
set boxwidth 1
set xtic scale 0
set rmargin 8
set bmargin 3

#set yrange [0:35]
#set y2range [10**-7:10**2]
set format y2 "10^{%T}";

plot '../dat/fp_ub_su.dat' using ($2/1e3):xtic(1) ti col fill pattern 2 border rgb "black" lc rgb "black", \
     '../dat/fp_ub_su.dat' using ($3/1e3):xtic(1) ti col fc rgb "grey", \
     '../dat/fp_ub_su.dat' using ($4/598366184):xtic(1) ti col axes x1y2 lc "black" with linespoints


reset
set size 1.0,0.95
set origin 0,0.05
set yrange [0:30]

set pointsize 4

set term pdf
set output "../graphs/Figure23.pdf"
set boxwidth 0.5 absolute
set xtics nomirror rotate by -30 scale 0
set ylabel "Latency (ms)" offset 1,0,0
set y2label "Space Overhead (UDI ratio)" offset 1,0,0
set logscale y2
set y2tics


set format y2 "10^{%T}";

set key font ",18" font ",11"
set xtics font ",11"
set ytics font ",11"
set y2tics font ",11"
set xlabel font ",11"
set ylabel font ",11"
set y2label font ",11"

set rmargin 8
set bmargin 2.8


plot '../dat/fp_ucb.dat' every 2    using ($1/1e3):xtic(2) t "UpBit-FP" with boxes fs solid 1.0 border rgb "black" lc rgb "grey", \
     '../dat/fp_ucb.dat' every 2::1 using ($1/598366184):xtic(2) t "Space overhead" axes x1y2 lc "black" ps 1.5 with linespoints

set term pdf
set output "../graphs/Figure20.pdf"
set boxwidth 0.5 absolute
set xtics nomirror rotate by -30 scale 0
set ylabel "Latency (ms)"  offset 1,0,0
set y2label "Space Overhead (UDI ratio)"  offset 1,0,0
set logscale y2
set y2tics

set yrange [0:]

set key font ",18" font ",11"
set xtics font ",11"
set ytics font ",11"
set y2tics font ",11"
set xlabel font ",11"
set ylabel font ",11"
set y2label font ",11"

set rmargin 9
set bmargin 2.5

plot '../dat/fp_ub.dat' every 2    using ($1/1e3):xtic(2) t "UpBit" with boxes fs solid 1.0 border rgb "black" lc rgb "grey", \
     '../dat/fp_ub.dat' every 2::1 using ($1/598366184):xtic(2) t "Space overhead" axes x1y2 lc "black" ps 1.5 with linespoints
