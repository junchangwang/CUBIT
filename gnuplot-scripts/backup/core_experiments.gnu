#Needs: ../dat/delete_new.dat ../dat/update_aggregate.dat ../dat/update_aggregate_fp.dat ../dat/insert_new.dat ../dat/mix_new.dat ../dat/mix_1000.dat ../dat/mix_zipf.dat ../dat/mix_earth_10.dat ../dat/mix_1B.dat ../dat/multi_latency.dat ../dat/update_breakup.dat
#Outputs: Figure 10, 11,12a, 12b, 12c, 13, 14, 15, 16, 17, 22, 24

reset
set key font ",18" invert top right

set term pdf font ",12"
set output "../graphs/Figure12a.pdf"

set origin 0,0.05
set size 1.0,0.95

#set yrange [0:3e3]
set autoscale y

#set title "Breakdown of Updates"
set style data histograms
set style histogram rowstacked
set boxwidth 0.75 relative
set style fill solid 1.0 border -1
# set xtics nomirror rotate by -30 scale 0

set ylabel "Workload Latency (s)" offset 1,0,0

#unset xtics
set label "1\% update" font ",13" at screen 0.20, screen 0.04
set label "5\% update" font ",13" at screen 0.46, screen 0.04
set label "10\% update" font ",13" at screen 0.71, screen 0.04

set xtics font ",8"

plot "../dat/update_aggregate.dat" using ($2/1e6):xtic(4) title "Read" with histograms fill pattern 4 border rgb "black" lc rgb "black", \
     "../dat/update_aggregate.dat" using ($3/1e6):xtic(4) title "Update" with histograms fs solid border rgb "black" lc rgb "black"

reset
set key font ",18" invert top right

set term pdf font ",12"
set output "../graphs/Figure12b.pdf"

set origin 0,0.05
set size 1.0,0.95

set autoscale y
#set title "Breakdown of Updates"
set style data histograms
set style histogram rowstacked
set boxwidth 0.75 relative
set style fill solid 1.0 border -1
# set xtics nomirror rotate by -30 scale 0

set ylabel "Workload Latency (s)" offset 1,0,0

#unset xtics
set label "1\% delete" font ",13" at screen 0.20, screen 0.04
set label "5\% delete" font ",13" at screen 0.46, screen 0.04
set label "10\% delete" font ",13" at screen 0.71, screen 0.04

set xtics font ",8"

plot "../dat/delete_new.dat" using ($2/1e6):xtic(4) title "Read" with histograms fill pattern 4 border rgb "black" lc rgb "black", \
     "../dat/delete_new.dat" using ($3/1e6):xtic(4) title "Delete" with histograms fs solid border rgb "black" lc rgb "black"

reset
set key font ",18" invert top right

set term pdf font ",12"
set output "../graphs/Figure24.pdf"

set origin 0,0.05
set size 1.0,0.95

#set yrange [0:3e3]
set autoscale y
#set title "Breakdown of Updates"
set style data histograms
set style histogram rowstacked
set boxwidth 0.75 relative
set style fill solid 1.0 border -1
set xtics nomirror rotate by -30 scale 0

set ylabel "Workload Latency (s)" offset 1,0,0

#unset xtics
set label "1\% update" font ",13" at screen 0.20, screen 0.07
set label "5\% update" font ",13" at screen 0.47, screen 0.07
set label "10\% update" font ",13" at screen 0.72, screen 0.07

set xtics font ",8"

plot "../dat/update_aggregate_fp.dat" using ($2/1e6):xtic(4) title "Read" with histograms fill pattern 4 border rgb "black" lc rgb "black", \
     "../dat/update_aggregate_fp.dat" using ($3/1e6):xtic(4) title "Update" with histograms fs solid border rgb "black" lc rgb "black"

reset
set key font ",18" invert top right

set term pdf font ",12"
set output "../graphs/Figure12c.pdf"

set origin 0,0.05
set size 1.0,0.95

set autoscale y
#set title "Breakdown of Updates"
set style data histograms
set style histogram rowstacked
set boxwidth 0.75 relative
set style fill solid 1.0 border -1
# set xtics nomirror rotate by -30 scale 0

set ylabel "Workload Latency (s)" offset 1,0,0

#unset xtics
set label "1\% insert" font ",13" at screen 0.20, screen 0.04
set label "5\% insert" font ",13" at screen 0.46, screen 0.04
set label "10\% insert" font ",13" at screen 0.71, screen 0.04

set xtics font ",8"

plot "../dat/insert_new.dat" using ($2/1e6):xtic(4) title "Read" with histograms fill pattern 4 border rgb "black" lc rgb "black", \
     "../dat/insert_new.dat" using ($3/1e6):xtic(4) title "Insert" with histograms fs solid border rgb "black" lc rgb "black"

reset
set key font ",18" invert top right

set term pdf font ",12"
set output "../graphs/Figure13.pdf"

set origin 0,0.05
set size 1.0,0.95

#set yrange [0:3e3]
#set title "Breakdown of Updates"
set style data histograms
set style histogram rowstacked
set boxwidth 0.75 relative
set style fill solid 1.0 border -1
# set xtics nomirror rotate by -30 scale 0

set ylabel "Workload Latency (s)" offset 1,0,0

#unset xtics
set label "1\% mix" font ",13" at screen 0.22, screen 0.04
set label "5\% mix" font ",13" at screen 0.48, screen 0.04
set label "10\% mix" font ",13" at screen 0.73, screen 0.04

set xtics font ",8"

plot "../dat/mix_new.dat" using ($2/1e6):xtic(4) title "Read" with histograms fill pattern 4 border rgb "black" lc rgb "black", \
     "../dat/mix_new.dat" using ($3/1e6):xtic(4) title "Mix UDI" with histograms fs solid border rgb "black" lc rgb "black"

# C = 1000
reset
set key font ",18" invert top right

set term pdf font ",12"
set output "../graphs/Figure14.pdf"

set origin 0,0.05
set size 1.0,0.95

#set yrange [0:250]
#set title "Breakdown of Updates"
set style data histograms
set style histogram rowstacked
set boxwidth 0.75 relative
set style fill solid 1.0 border -1
# set xtics nomirror rotate by -30 scale 0

set ylabel "Workload Latency (s)" offset 1,0,0

#unset xtics
set label "1\% mix" font ",13" at screen 0.24, screen 0.04
set label "5\% mix" font ",13" at screen 0.50, screen 0.04
set label "10\% mix" font ",13" at screen 0.74, screen 0.04

set xtics font ",8"

plot "../dat/mix_1000.dat" using ($2/1e6):xtic(4) title "Read" with histograms fill pattern 4 border rgb "black" lc rgb "black", \
     "../dat/mix_1000.dat" using ($3/1e6):xtic(4) title "Mix UDI" with histograms fs solid border rgb "black" lc rgb "black"

# zipf
reset
set key font ",18" invert top right

set term pdf font ",12"
set output "../graphs/Figure16.pdf"

set origin 0,0.05
set size 1.0,0.95

#set yrange [0:800]
#set title "Breakdown of Updates"
set style data histograms
set style histogram rowstacked
set boxwidth 0.75 relative
set style fill solid 1.0 border -1
# set xtics nomirror rotate by -30 scale 0

set ylabel "Workload Latency (s)" offset 1,0,0

#unset xtics
set label "1\% mix" font ",13" at screen 0.24, screen 0.04
set label "5\% mix" font ",13" at screen 0.50, screen 0.04
set label "10\% mix" font ",13" at screen 0.74, screen 0.04

set xtics font ",8"

plot "../dat/mix_zipf.dat" using ($2/1e6):xtic(4) title "Read" with histograms fill pattern 4 border rgb "black" lc rgb "black", \
     "../dat/mix_zipf.dat" using ($3/1e6):xtic(4) title "Mix UDI" with histograms fs solid border rgb "black" lc rgb "black"

# zipf
reset
set key font ",18" invert top right

set term pdf font ",12"
set output "../graphs/Figure17.pdf"

set origin 0,0.05
set size 1.0,0.95

#set yrange [0:150]
#set title "Breakdown of Updates"
set style data histograms
set style histogram rowstacked
set boxwidth 0.75 relative
set style fill solid 1.0 border -1
# set xtics nomirror rotate by -30 scale 0

set ylabel "Workload Latency (s)" offset 1,0,0

#unset xtics
set label "1\% mix" font ",13" at screen 0.24, screen 0.04
set label "5\% mix" font ",13" at screen 0.50, screen 0.04
set label "10\% mix" font ",13" at screen 0.74, screen 0.04

set xtics font ",8"

plot "../dat/mix_earth_10.dat" using ($2/1e6):xtic(4) title "Read" with histograms fill pattern 4 border rgb "black" lc rgb "black", \
     "../dat/mix_earth_10.dat" using ($3/1e6):xtic(4) title "Mix UDI" with histograms fs solid border rgb "black" lc rgb "black"

reset
set key font ",18" invert top right

set term pdf font ",12"
set output "../graphs/Figure15.pdf"

set origin 0,0.05
set size 1.0,0.95

#set yrange [0:3000]
#set title "Breakdown of Updates"
set style data histograms
set style histogram rowstacked
set boxwidth 0.75 relative
set style fill solid 1.0 border -1
# set xtics nomirror rotate by -30 scale 0

set ylabel "Workload Latency (s)" offset 1,0,0

#unset xtics
set label "1\% mix" font ",13" at screen 0.24, screen 0.04
set label "5\% mix" font ",13" at screen 0.50, screen 0.04
set label "10\% mix" font ",13" at screen 0.74, screen 0.04

set xtics font ",8"

plot "../dat/mix_1B.dat" using ($2/1e6):xtic(4) title "Read" with histograms fill pattern 4 border rgb "black" lc rgb "black", \
     "../dat/mix_1B.dat" using ($3/1e6):xtic(4) title "Mix UDI" with histograms fs solid border rgb "black" lc rgb "black"


reset

set origin 0,0
set size 1.0,1
set boxwidth 0.5

set ylabel "Latency (ms)" offset 1.2,0,0
set xlabel "Number of threads"
set yrange [:1000]
set xrange [2:]
set term pdf font ",12" dashed
set termoption dash

set output "../graphs/Figure22.pdf"

set xtics (1,2,4,8,16,32,64)
set logscale x
set logscale y
 set format y "10^{%T}"

#set key font ",18" samplen 3 spacing 3 font ",8"
set key font ",18" samplen 4 spacing 1.4 font ",7" maxrows 2


set style line 1 lc rgb 'black' lt 0 lw 2 pt 4 ps 3
set style line 3 lc rgb 'black' lt 0 lw 2 pt 6 ps 3
set style line 5 lc rgb 'black' lt 0 lw 2 pt 2 ps 3
set style line 2 lc rgb 'black'  lw 2 pt 4 ps 3
set style line 4 lc rgb 'black'  lw 2 pt 6 ps 3
set style line 6 lc rgb 'black'  lw 2 pt 2 ps 3


plot "../dat/multi_latency.dat" using ($1):($9/1e3) title "In-place (Read)" ls 2 with lp, \
     "../dat/multi_latency.dat" using ($1):($10/1e3) title "In-place (Update)" ls 1 with lp, \
     "../dat/multi_latency.dat" using ($1):($6/1e3) title "UCB (Read)" ls 4 with lp , \
     "../dat/multi_latency.dat" using ($1):($7/1e3) title "UCB (Update)" ls 3 with lp, \
     "../dat/multi_latency.dat" using ($1):($3/1e3) title "UpBit (Read)" ls 6 with lp, \
     "../dat/multi_latency.dat" using ($1):($4/1e3) title "UpBit (Update)" ls 5 with lp




reset
set origin 0,0.05
set size 1.0,0.95

set term pdf font ",12"
set output "../graphs/Figure10.pdf"
set boxwidth 0.5
set style fill solid
set ylabel "Update Latency (ms)" offset 1,0,0

set xrange [0:12]
#set yrange [0:35]

unset xtics
set label "1\% update" font ",13" at screen 0.16, screen 0.04
set label "5\% update" font ",13" at screen 0.44, screen 0.04
set label "10\% update" font ",13" at screen 0.70, screen 0.04

plot "../dat/update_breakup.dat" every 3    using 1:($2/1e3) title "In-place" with boxes fs solid 1.0 border rgb "black" lc rgb "white", \
     "../dat/update_breakup.dat" every 3::1 using 1:($2/1e3) title "UCB" with boxes fs solid 1.0 border rgb "black" lc rgb "grey", \
     "../dat/update_breakup.dat" every 3::2 using 1:($2/1e3) title "UpBit" with boxes fs solid 1.0 border rgb "black" lc rgb "black"



reset
set origin 0,0.05
set size 1.0,0.95

set term pdf font ",12"
set output "../graphs/Figure11.pdf"
set boxwidth 0.5
set style fill solid
set ylabel "Read Latency (ms)" offset 1,0,0

set xrange [0:12]
set yrange [0:35]

unset xtics
set label "1\% update" font ",13" at screen 0.16, screen 0.04
set label "5\% update" font ",13" at screen 0.44, screen 0.04
set label "10\% update" font ",13" at screen 0.70, screen 0.04

plot "../dat/update_breakup.dat" every 3    using 1:($3/1e3) title "Read-optimized" with boxes fs solid 1.0 border rgb "black" lc rgb "white", \
     "../dat/update_breakup.dat" every 3::1 using 1:($3/1e3) title "UCB" with boxes fs solid 1.0 border rgb "black" lc rgb "grey", \
     "../dat/update_breakup.dat" every 3::2 using 1:($3/1e3) title "UpBit" with boxes fs solid border rgb "black" lc rgb "black"
