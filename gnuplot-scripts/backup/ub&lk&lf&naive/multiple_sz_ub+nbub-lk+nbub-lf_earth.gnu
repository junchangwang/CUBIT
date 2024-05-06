#all_throughput_core
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel "Throughput (op/s)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' "Number of cores"
set xtics ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32)
set yrange [0:]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_throughput_core_ub+cubit-lk+cubit-lf.png"
set key font ",18" reverse Left top left

plot "../dat/figure_cubit-lf_throughput_core.dat" title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
    "../dat/figure_cubit-lk_throughput_core.dat" title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_throughput_core.dat" title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

set terminal eps font 'Linux Libertine O,17' 
set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_throughput_core_ub+cubit-lk+cubit-lf.eps"
replot


#all_latency_core_Query
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel "Query latency (ms)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' "Number of cores"
set xtics ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32)
set yrange [0:]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_core_Query_ub+cubit-lk+cubit-lf.png"
set key font ",18" reverse Left top left

plot "../dat/figure_cubit-lf_latency_core.dat" every 5::0::30 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_cubit-lk_latency_core.dat" every 5::0::30 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::0::30 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

set terminal eps font 'Linux Libertine O,17' 
set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_core_Query_ub+cubit-lk+cubit-lf.eps"
replot

#all_latency_core_Update
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel "Update latency (ms)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' "Number of cores"
set xtics ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32)
set yrange [0:]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_core_Update_ub+cubit-lk+cubit-lf.png"
set key font ",18" reverse Left top left

plot "../dat/figure_cubit-lf_latency_core.dat" every 5::1::31 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_cubit-lk_latency_core.dat" every 5::1::31 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::1::31 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

set terminal eps font 'Linux Libertine O,17' 
set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_core_Update_ub+cubit-lk+cubit-lf.eps"
replot

#all_latency_core_Insert
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel "Insert latency (ms)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' "Number of cores"
set xtics ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32)
set yrange [0:]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_core_Insert_ub+cubit-lk+cubit-lf.png"
set key font ",18" reverse Left top left

plot  "../dat/figure_cubit-lf_latency_core.dat" every 5::2::32 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_cubit-lk_latency_core.dat" every 5::2::32 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::2::32 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

set terminal eps font 'Linux Libertine O,17' 
set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_core_Insert_ub+cubit-lk+cubit-lf.eps"
replot

#all_latency_core_Delete
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel "Delete latency (ms)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' "Number of cores"
set xtics ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32)
set yrange [0:]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_core_Delete_ub+cubit-lk+cubit-lf.png"
set key font ",18" reverse Left top left

plot "../dat/figure_cubit-lf_latency_core.dat" every 5::3::33 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_cubit-lk_latency_core.dat" every 5::3::33 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::3::33 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

set terminal eps font 'Linux Libertine O,17' 
set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_core_Delete_ub+cubit-lk+cubit-lf.eps"
replot

#all_latency_core_All
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel "Overall latency (ms)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' "Number of cores"
set xtics ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32)
set yrange [0:]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_core_Overall_ub+cubit-lk+cubit-lf.png"
set key font ",18" reverse Left top left

plot "../dat/figure_cubit-lf_latency_core.dat" every 5::4::34 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_cubit-lk_latency_core.dat" every 5::4::34 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::4::34 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

set terminal eps font 'Linux Libertine O,17' 
set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_core_Overall_ub+cubit-lk+cubit-lf.eps"
replot

################################## UDI ratio

# #all_throughput_ratio
# reset
# set size 0.99,1.0
# #set term pdf font ",10" size 5.2, 2.4
# set ylabel "Throughput (op/s)" font 'Linux Libertine O,25'
# set xlabel font 'Linux Libertine O,25' "UDI ratio"
# set yrange [0:]
# set terminal png font 'Linux Libertine O,15' size 400, 500
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_throughput_ratio_ub+cubit-lk+cubit-lf.png"
# set key font ",18" reverse Left top

# plot  "../dat/figure_cubit-lf_throughput_ratio.dat" title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#     "../dat/figure_cubit-lk_throughput_ratio.dat" title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_throughput_ratio.dat" title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_throughput_ratio_ub+cubit-lk+cubit-lf.eps"
# replot


# #all_latency_ratio_Query
# reset
# set size 0.99,1.0
# #set term pdf font ",10" size 5.2, 2.4
# set ylabel "Query latency (ms)" font 'Linux Libertine O,25'
# set xlabel font 'Linux Libertine O,25' "UDI ratio"
# set yrange [0:]
# set terminal png font 'Linux Libertine O,15' size 400, 500
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_ratio_Query_ub+cubit-lk+cubit-lf.png"
# set key font ",18" reverse Left top left

# plot "../dat/figure_cubit-lf_latency_ratio.dat" every 5::0::30 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_latency_ratio.dat" every 5::0::30 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_latency_ratio.dat" every 5::0::30 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_ratio_Query_ub+cubit-lk+cubit-lf.eps"
# replot

# #all_latency_ratio_Update
# reset
# set size 0.99,1.0
# #set term pdf font ",10" size 5.2, 2.4
# set ylabel "Update latency (ms)" font 'Linux Libertine O,25'
# set xlabel font 'Linux Libertine O,25' "UDI ratio"
# set yrange [0:]
# set terminal png font 'Linux Libertine O,15' size 400, 500
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_ratio_Update_ub+cubit-lk+cubit-lf.png"
# set key font ",18" reverse Left top left

# plot "../dat/figure_cubit-lf_latency_ratio.dat" every 5::1::31 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_latency_ratio.dat" every 5::1::31 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_latency_ratio.dat" every 5::1::31 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_ratio_Update_ub+cubit-lk+cubit-lf.eps"
# replot

# #all_latency_ratio_Insert
# reset
# set size 0.99,1.0
# #set term pdf font ",10" size 5.2, 2.4
# set ylabel "Insert latency (ms)" font 'Linux Libertine O,25'
# set xlabel font 'Linux Libertine O,25' "UDI ratio"
# set yrange [0:]
# set terminal png font 'Linux Libertine O,15' size 400, 500
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_ratio_Insert_ub+cubit-lk+cubit-lf.png"
# set key font ",18" reverse Left top left

# plot "../dat/figure_cubit-lf_latency_ratio.dat" every 5::2::32 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_latency_ratio.dat" every 5::2::32 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_latency_ratio.dat" every 5::2::32 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_ratio_Insert_ub+cubit-lk+cubit-lf.eps"
# replot

# #all_latency_ratio_Delete
# reset
# set size 0.99,1.0
# #set term pdf font ",10" size 5.2, 2.4
# set ylabel "Delete latency (ms)" font 'Linux Libertine O,25'
# set xlabel font 'Linux Libertine O,25' "UDI ratio"
# set yrange [0:]
# set terminal png font 'Linux Libertine O,15' size 400, 500
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_ratio_Delete_ub+cubit-lk+cubit-lf.png"
# set key font ",18" reverse Left top left

# plot "../dat/figure_cubit-lf_latency_ratio.dat" every 5::3::33 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_latency_ratio.dat" every 5::3::33 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_latency_ratio.dat" every 5::3::33 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_ratio_Delete_ub+cubit-lk+cubit-lf.eps"
# replot

# #all_latency_ratio_All
# reset
# set size 0.99,1.0
# #set term pdf font ",10" size 5.2, 2.4
# set ylabel "Overall latency (ms)" font 'Linux Libertine O,25'
# set xlabel font 'Linux Libertine O,25' "UDI ratio"
# set yrange [0:]
# set terminal png font 'Linux Libertine O,15' size 400, 500
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_ratio_Overall_ub+cubit-lk+cubit-lf.png"
# set key font ",18" reverse Left top left

# plot  "../dat/figure_cubit-lf_latency_ratio.dat" every 5::4::34 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_latency_ratio.dat" every 5::4::34 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_latency_ratio.dat" every 5::4::34 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_ratio_Overall_ub+cubit-lk+cubit-lf.eps"
# replot






# #all_throughput_cardinality
# reset
# set size 0.99,1.0
# #set term pdf font ",10" size 5.2, 2.4
# set ylabel "Throughput (op/s)" font 'Linux Libertine O,25'
# set xlabel font 'Linux Libertine O,25' "Cardinality
# set yrange [0:]
# #set xrange [0:16]
# set terminal png font 'Linux Libertine O,15' size 400, 500
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_throughput_cardinality_ub+cubit-lk+cubit-lf.png"
# set key font ",18" reverse Left top

# plot "../dat/figure_cubit-lf_throughput_cardinality.dat" title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#     "../dat/figure_cubit-lk_throughput_cardinality.dat" title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_throughput_cardinality.dat" title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_throughput_cardinality_ub+cubit-lk+cubit-lf.eps"
# replot



# #all_latency_cardinality_Query
# reset
# set size 0.99,1.0
# #set term pdf font ",10" size 5.2, 2.4
# set ylabel "Query latency (ms)" font 'Linux Libertine O,25'
# set xlabel font 'Linux Libertine O,25' "Cardinality
# set yrange [0:]
# #set xrange [0:16]
# set terminal png font 'Linux Libertine O,15' size 400, 500
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_cardinality_Query_ub+cubit-lk+cubit-lf.png"
# set key font ",18" reverse Left top

# plot "../dat/figure_cubit-lf_latency_cardinality.dat" every 5::0::30 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_latency_cardinality.dat" every 5::0::30 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_latency_cardinality.dat" every 5::0::30 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_cardinality_Query_ub+cubit-lk+cubit-lf.eps"
# replot

# #all_latency_cardinality_Update
# reset
# set size 0.99,1.0
# #set term pdf font ",10" size 5.2, 2.4
# set ylabel "Update latency (ms)" font 'Linux Libertine O,25'
# set xlabel font 'Linux Libertine O,25' "Cardinality
# set yrange [0:]
# #set xrange [0:16]
# set terminal png font 'Linux Libertine O,15' size 400, 500
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_cardinality_Update_ub+cubit-lk+cubit-lf.png"
# set key font ",18" reverse Left top

# plot "../dat/figure_cubit-lf_latency_cardinality.dat" every 5::1::31 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_latency_cardinality.dat" every 5::1::31 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_latency_cardinality.dat" every 5::1::31 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_cardinality_Update_ub+cubit-lk+cubit-lf.eps"
# replot

# #all_latency_cardinality_Insert
# reset
# set size 0.99,1.0
# #set term pdf font ",10" size 5.2, 2.4
# set ylabel "Insert latency (ms)" font 'Linux Libertine O,25'
# set xlabel font 'Linux Libertine O,25' "Cardinality
# set yrange [0:]
# #set xrange [0:16]
# set terminal png font 'Linux Libertine O,15' size 400, 500
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_cardinality_Insert_ub+cubit-lk+cubit-lf.png"
# set key font ",18" reverse Left top

# plot  "../dat/figure_cubit-lf_latency_cardinality.dat" every 5::2::32 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_latency_cardinality.dat" every 5::2::32 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_latency_cardinality.dat" every 5::2::32 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_cardinality_Insert_ub+cubit-lk+cubit-lf.eps"
# replot

# #all_latency_cardinality_Delete
# reset
# set size 0.99,1.0
# #set term pdf font ",10" size 5.2, 2.4
# set ylabel "Delete latency (ms)" font 'Linux Libertine O,25'
# set xlabel font 'Linux Libertine O,25' "Cardinality
# set yrange [0:]
# #set xrange [0:16]
# set terminal png font 'Linux Libertine O,15' size 400, 500
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_cardinality_Delete_ub+cubit-lk+cubit-lf.png"
# set key font ",18" reverse Left top

# plot "../dat/figure_cubit-lf_latency_cardinality.dat" every 5::3::33 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_latency_cardinality.dat" every 5::3::33 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_latency_cardinality.dat" every 5::3::33 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_cardinality_Delete_ub+cubit-lk+cubit-lf.eps"
# replot

# #all_latency_cardinality_All
# reset
# set size 0.99,1.0
# #set term pdf font ",10" size 5.2, 2.4
# set ylabel "Overall latency (ms)" font 'Linux Libertine O,25'
# set xlabel font 'Linux Libertine O,25' "Cardinality
# set yrange [0:]
# #set xrange [0:16]
# set terminal png font 'Linux Libertine O,15' size 400, 500
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_cardinality_Overall_ub+cubit-lk+cubit-lf.png"
# set key font ",18" reverse Left top

# plot "../dat/figure_cubit-lf_latency_cardinality.dat" every 5::4::34 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_latency_cardinality.dat" every 5::4::34 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_latency_cardinality.dat" every 5::4::34 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints

# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_earth/multiple_ub+cubit-lk+cubit-lf/Figure_multiple_latency_cardinality_Overall_ub+cubit-lk+cubit-lf.eps"
# replot

