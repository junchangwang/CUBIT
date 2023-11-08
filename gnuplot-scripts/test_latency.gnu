#all_latency_core_Query
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel "Query latency (ms)" offset 1.5,0,0
set xlabel "Number of cores"
#set yrange [0:40]
#set xrange [0:16]
set terminal png
set output "../graphs/figure_test_latency/Figure_multiple_latency_core_Query_ub+nbub-lk+nbub-lf.png"
set key font ",18" top

plot "../dat/figure_nbub-lf_latency_core.dat" every 5::0::20 title "nbub-lf" lc rgb "blue" ps 1 pt 9 with linespoints,\
      "../dat/figure_nbub-lk_latency_core.dat" every 5::0::20 title "nbub-lk" lc rgb "blue" ps 1 pt 8 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::0::20 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

set terminal eps
set output "../graphs/figure_test_latency/Figure_multiple_latency_core_Query_ub+nbub-lk+nbub-lf.eps"
replot

#all_latency_core_Update
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel "Update latency (ms)" offset 1.5,0,0
set xlabel "Number of cores"
#set yrange [0:40]
#set xrange [0:16]
set terminal png
set output "../graphs/figure_test_latency/Figure_multiple_latency_core_Update_ub+nbub-lk+nbub-lf.png"
set key font ",18" top

plot "../dat/figure_nbub-lf_latency_core.dat" every 5::1::21 title "nbub-lf" lc rgb "blue" ps 1 pt 9 with linespoints,\
      "../dat/figure_nbub-lk_latency_core.dat" every 5::1::21 title "nbub-lk" lc rgb "blue" ps 1 pt 8 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::1::21 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

set terminal eps
set output "../graphs/figure_test_latency/Figure_multiple_latency_core_Update_ub+nbub-lk+nbub-lf.eps"
replot

#all_latency_core_Insert
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel "Insert latency (ms)" offset 1.5,0,0
set xlabel "Number of cores"
#set yrange [0:40]
#set xrange [0:16]
set terminal png
set output "../graphs/figure_test_latency/Figure_multiple_latency_core_Insert_ub+nbub-lk+nbub-lf.png"
set key font ",18" top

plot  "../dat/figure_nbub-lf_latency_core.dat" every 5::2::22 title "nbub-lf" lc rgb "blue" ps 1 pt 9 with linespoints,\
      "../dat/figure_nbub-lk_latency_core.dat" every 5::2::22 title "nbub-lk" lc rgb "blue" ps 1 pt 8 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::2::22 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

set terminal eps
set output "../graphs/figure_test_latency/Figure_multiple_latency_core_Insert_ub+nbub-lk+nbub-lf.eps"
replot

#all_latency_core_Delete
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel "Delete latency (ms)" offset 1.5,0,0
set xlabel "Number of cores"
#set yrange [0:40]
#set xrange [0:16]
set terminal png
set output "../graphs/figure_test_latency/Figure_multiple_latency_core_Delete_ub+nbub-lk+nbub-lf.png"
set key font ",18" top

plot "../dat/figure_nbub-lf_latency_core.dat" every 5::3::23 title "nbub-lf" lc rgb "blue" ps 1 pt 9 with linespoints,\
      "../dat/figure_nbub-lk_latency_core.dat" every 5::3::23 title "nbub-lk" lc rgb "blue" ps 1 pt 8 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::3::23 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

set terminal eps
set output "../graphs/figure_test_latency/Figure_multiple_latency_core_Delete_ub+nbub-lk+nbub-lf.eps"
replot

#all_latency_core_All
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel "Overall latency (ms)" offset 1.5,0,0
set xlabel "Number of cores"
#set yrange [0:40]
#set xrange [0:16]
set terminal png
set output "../graphs/figure_test_latency/Figure_multiple_latency_core_Overall_ub+nbub-lk+nbub-lf.png"
set key font ",18" top

plot "../dat/figure_nbub-lf_latency_core.dat" every 5::4::24 title "nbub-lf" lc rgb "blue" ps 1 pt 9 with linespoints,\
      "../dat/figure_nbub-lk_latency_core.dat" every 5::4::24 title "nbub-lk" lc rgb "blue" ps 1 pt 8 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::4::24 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

set terminal eps
set output "../graphs/figure_test_latency/Figure_multiple_latency_core_Overall_ub+nbub-lk+nbub-lf.eps"
replot


#all_latency_ratio_Query
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel "Query latency (ms)" offset 1.5,0,0
set xlabel "UDI ratio"
#set yrange [0:40]
set terminal png
set output "../graphs/figure_test_latency/Figure_multiple_latency_ratio_Query_ub+nbub-lk+nbub-lf.png"
set key font ",18" top

plot "../dat/figure_nbub-lf_latency_ratio.dat" every 5::0::20 title "nbub-lf" lc rgb "blue" ps 1 pt 9 with linespoints,\
      "../dat/figure_nbub-lk_latency_ratio.dat" every 5::0::20 title "nbub-lk" lc rgb "blue" ps 1 pt 8 with linespoints,\
      "../dat/figure_ub_latency_ratio.dat" every 5::0::20 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

set terminal eps
set output "../graphs/figure_test_latency/Figure_multiple_latency_ratio_Query_ub+nbub-lk+nbub-lf.eps"
replot

#all_latency_ratio_Update
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel "Update latency (ms)" offset 1.5,0,0
set xlabel "UDI ratio"
#set yrange [0:40]
set terminal png
set output "../graphs/figure_test_latency/Figure_multiple_latency_ratio_Update_ub+nbub-lk+nbub-lf.png"
set key font ",18" top

plot "../dat/figure_nbub-lf_latency_ratio.dat" every 5::1::21 title "nbub-lf" lc rgb "blue" ps 1 pt 9 with linespoints,\
      "../dat/figure_nbub-lk_latency_ratio.dat" every 5::1::21 title "nbub-lk" lc rgb "blue" ps 1 pt 8 with linespoints,\
      "../dat/figure_ub_latency_ratio.dat" every 5::1::21 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

set terminal eps
set output "../graphs/figure_test_latency/Figure_multiple_latency_ratio_Update_ub+nbub-lk+nbub-lf.eps"
replot

#all_latency_ratio_Insert
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel "Insert latency (ms)" offset 1.5,0,0
set xlabel "UDI ratio"
#set yrange [0:40]
set terminal png
set output "../graphs/figure_test_latency/Figure_multiple_latency_ratio_Insert_ub+nbub-lk+nbub-lf.png"
set key font ",18" top

plot "../dat/figure_nbub-lf_latency_ratio.dat" every 5::2::22 title "nbub-lf" lc rgb "blue" ps 1 pt 9 with linespoints,\
      "../dat/figure_nbub-lk_latency_ratio.dat" every 5::2::22 title "nbub-lk" lc rgb "blue" ps 1 pt 8 with linespoints,\
      "../dat/figure_ub_latency_ratio.dat" every 5::2::22 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

set terminal eps
set output "../graphs/figure_test_latency/Figure_multiple_latency_ratio_Insert_ub+nbub-lk+nbub-lf.eps"
replot

#all_latency_ratio_Delete
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel "Delete latency (ms)" offset 1.5,0,0
set xlabel "UDI ratio"
#set yrange [0:40]
set terminal png
set output "../graphs/figure_test_latency/Figure_multiple_latency_ratio_Delete_ub+nbub-lk+nbub-lf.png"
set key font ",18" top

plot "../dat/figure_nbub-lf_latency_ratio.dat" every 5::3::23 title "nbub-lf" lc rgb "blue" ps 1 pt 9 with linespoints,\
      "../dat/figure_nbub-lk_latency_ratio.dat" every 5::3::23 title "nbub-lk" lc rgb "blue" ps 1 pt 8 with linespoints,\
      "../dat/figure_ub_latency_ratio.dat" every 5::3::23 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

set terminal eps
set output "../graphs/figure_test_latency/Figure_multiple_latency_ratio_Delete_ub+nbub-lk+nbub-lf.eps"
replot

#all_latency_ratio_All
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel "Overall latency (ms)" offset 1.5,0,0
set xlabel "UDI ratio"
#set yrange [0:40]
set terminal png
set output "../graphs/figure_test_latency/Figure_multiple_latency_ratio_Overall_ub+nbub-lk+nbub-lf.png"
set key font ",18" top

plot  "../dat/figure_nbub-lf_latency_ratio.dat" every 5::4::24 title "nbub-lf" lc rgb "blue" ps 1 pt 9 with linespoints,\
      "../dat/figure_nbub-lk_latency_ratio.dat" every 5::4::24 title "nbub-lk" lc rgb "blue" ps 1 pt 8 with linespoints,\
      "../dat/figure_ub_latency_ratio.dat" every 5::4::24 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

set terminal eps
set output "../graphs/figure_test_latency/Figure_multiple_latency_ratio_Overall_ub+nbub-lk+nbub-lf.eps
replot
