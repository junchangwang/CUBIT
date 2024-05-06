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
set output "../graphs_zipf/multiple/Figure_multiple_throughput_core.png"
set key font ",18" reverse Left top left

plot  "../dat/figure_cubit-lf_throughput_core.dat" title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_cubit-lk_throughput_core.dat" title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_throughput_core.dat" title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_throughput_core.dat" title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_throughput_core.dat" title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps font 'Linux Libertine O,17' 
set output "../graphs_zipf/multiple/Figure_multiple_throughput_core.eps"
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
set output "../graphs_zipf/multiple/Figure_multiple_latency_core_Query.png"
set key font ",18" reverse Left top left

plot  "../dat/figure_cubit-lf_latency_core.dat" every 5::0::30 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_cubit-lk_latency_core.dat" every 5::0::30 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::0::30 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_core.dat" every 5::0::30 title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_core.dat" every 5::0::30 title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps font 'Linux Libertine O,17' 
set output "../graphs_zipf/multiple/Figure_multiple_latency_core_Query.eps"
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
set output "../graphs_zipf/multiple/Figure_multiple_latency_core_Update.png"
set key font ",18" reverse Left top left

plot  "../dat/figure_cubit-lf_latency_core.dat" every 5::1::31 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_cubit-lk_latency_core.dat" every 5::1::31 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::1::31 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_core.dat" every 5::1::31 title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_core.dat" every 5::1::31 title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps font 'Linux Libertine O,17' 
set output "../graphs_zipf/multiple/Figure_multiple_latency_core_Update.eps"
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
set output "../graphs_zipf/multiple/Figure_multiple_latency_core_Insert.png"
set key font ",18" reverse Left top left

plot  "../dat/figure_cubit-lf_latency_core.dat" every 5::2::32 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_cubit-lk_latency_core.dat" every 5::2::32 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::2::32 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_core.dat" every 5::2::32 title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_core.dat" every 5::2::32 title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps font 'Linux Libertine O,17' 
set output "../graphs_zipf/multiple/Figure_multiple_latency_core_Insert.eps"
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
set output "../graphs_zipf/multiple/Figure_multiple_latency_core_Delete.png"
set key font ",18" reverse Left top left

plot  "../dat/figure_cubit-lf_latency_core.dat" every 5::3::33 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_cubit-lk_latency_core.dat" every 5::3::33 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::3::33 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_core.dat" every 5::3::33 title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_core.dat" every 5::3::33 title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps font 'Linux Libertine O,17' 
set output "../graphs_zipf/multiple/Figure_multiple_latency_core_Delete.eps"
replot

#all_latency_core_Overall
reset
set size 0.99,1.0
#set term pdf font ",10" size 5.2, 2.4
set ylabel "Overall latency (ms)" font 'Linux Libertine O,25'
set xlabel font 'Linux Libertine O,25' "Number of cores"
set xtics ("1" 1, "2" 2 1, "4" 4, "8" 8, "16" 16, "24" 24, "32" 32)
set yrange [0:]
set xrange [0:35]
set terminal png font 'Linux Libertine O,15' size 400, 500
set output "../graphs_zipf/multiple/Figure_multiple_latency_core_Overall.png"
set key font ",18" reverse Left top left

plot  "../dat/figure_cubit-lf_latency_core.dat" every 5::4::34 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_cubit-lk_latency_core.dat" every 5::4::34 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
      "../dat/figure_ub_latency_core.dat" every 5::4::34 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
      "../dat/figure_naive_latency_core.dat" every 5::4::34 title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
      "../dat/figure_ucb_latency_core.dat" every 5::4::34 title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints

set terminal eps font 'Linux Libertine O,17' 
set output "../graphs_zipf/multiple/Figure_multiple_latency_core_Overall.eps"
replot

# ############################################### for UDI ratio

# #all_throughput_ratio
# reset
# set size 0.99,1.0
# #set term pdf font ",10" size 5.2, 2.4
# set ylabel "Throughput (op/s)" font 'Linux Libertine O,25'
# set xlabel font 'Linux Libertine O,25' "UDI ratio"
# set yrange [0:]
# set terminal png font 'Linux Libertine O,15' size 400, 500
# set output "../graphs_zipf/multiple/Figure_multiple_throughput_ratio.png"
# set key font ",18" reverse Left top
# 
# plot "../dat/figure_cubit-lf_throughput_ratio.dat" title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_throughput_ratio.dat" title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#     "../dat/figure_ub_throughput_ratio.dat" title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
#     "../dat/figure_ucb_throughput_ratio.dat" title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints
#     "../dat/figure_naive_throughput_ratio.dat" title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
# 
# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_zipf/multiple/Figure_multiple_throughput_ratio.eps"
# replot
# 
# 
# #all_latency_ratio_Query
# reset
# set size 0.99,1.0
# #set term pdf font ",10" size 5.2, 2.4
# set ylabel "Query latency (ms)" font 'Linux Libertine O,25'
# set xlabel font 'Linux Libertine O,25' "UDI ratio"
# set yrange [0:]
# set terminal png font 'Linux Libertine O,15' size 400, 500
# set output "../graphs_zipf/multiple/Figure_multiple_latency_ratio_Query.png"
# set key font ",18" reverse Left top left
# 
# plot  "../dat/figure_cubit-lf_latency_ratio.dat" every 5::0::30 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_latency_ratio.dat" every 5::0::30 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_latency_ratio.dat" every 5::0::30 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
#       "../dat/figure_ucb_latency_ratio.dat" every 5::0::30 title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints
#       "../dat/figure_naive_latency_ratio.dat" every 5::0::30 title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
# 
# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_zipf/multiple/Figure_multiple_latency_ratio_Query.eps"
# replot
# 
# #all_latency_ratio_Update
# reset
# set size 0.99,1.0
# #set term pdf font ",10" size 5.2, 2.4
# set ylabel "Update latency (ms)" font 'Linux Libertine O,25'
# set xlabel font 'Linux Libertine O,25' "UDI ratio"
# set yrange [0:]
# set terminal png font 'Linux Libertine O,15' size 400, 500
# set output "../graphs_zipf/multiple/Figure_multiple_latency_ratio_Update.png"
# set key font ",18" reverse Left top
# 
# plot  "../dat/figure_cubit-lf_latency_ratio.dat" every 5::1::31 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_latency_ratio.dat" every 5::1::31 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_latency_ratio.dat" every 5::1::31 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
#       "../dat/figure_ucb_latency_ratio.dat" every 5::1::31 title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints
#       "../dat/figure_naive_latency_ratio.dat" every 5::1::31 title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
# 
# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_zipf/multiple/Figure_multiple_latency_ratio_Update.eps"
# replot
# 
# #all_latency_ratio_Insert
# reset
# set size 0.99,1.0
# #set term pdf font ",10" size 5.2, 2.4
# set ylabel "Insert latency (ms)" font 'Linux Libertine O,25'
# set xlabel font 'Linux Libertine O,25' "UDI ratio"
# set yrange [0:]
# set terminal png font 'Linux Libertine O,15' size 400, 500
# set output "../graphs_zipf/multiple/Figure_multiple_latency_ratio_Insert.png"
# set key font ",18" reverse Left top
# 
# plot  "../dat/figure_cubit-lf_latency_ratio.dat" every 5::2::32 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_latency_ratio.dat" every 5::2::32 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_latency_ratio.dat" every 5::2::32 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
#       "../dat/figure_ucb_latency_ratio.dat" every 5::2::32 title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints
#       "../dat/figure_naive_latency_ratio.dat" every 5::2::32 title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
# 
# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_zipf/multiple/Figure_multiple_latency_ratio_Insert.eps"
# replot
# 
# #all_latency_ratio_Delete
# reset
# set size 0.99,1.0
# #set term pdf font ",10" size 5.2, 2.4
# set ylabel "Delete latency (ms)" font 'Linux Libertine O,25'
# set xlabel font 'Linux Libertine O,25' "UDI ratio"
# set yrange [0:]
# set terminal png font 'Linux Libertine O,15' size 400, 500
# set output "../graphs_zipf/multiple/Figure_multiple_latency_ratio_Delete.png"
# set key font ",18" reverse Left top
# 
# plot  "../dat/figure_cubit-lf_latency_ratio.dat" every 5::3::33 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_latency_ratio.dat" every 5::3::33 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_latency_ratio.dat" every 5::3::33 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
#       "../dat/figure_ucb_latency_ratio.dat" every 5::3::33 title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints
#       "../dat/figure_naive_latency_ratio.dat" every 5::3::33 title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
# 
# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_zipf/multiple/Figure_multiple_latency_ratio_Delete.eps"
# replot
# 
# #all_latency_ratio_All
# reset
# set size 0.99,1.0
# #set term pdf font ",10" size 5.2, 2.4
# set ylabel "Overall latency (ms)" font 'Linux Libertine O,25'
# set xlabel font 'Linux Libertine O,25' "UDI ratio"
# set yrange [0:]
# set terminal png font 'Linux Libertine O,15' size 400, 500
# set output "../graphs_zipf/multiple/Figure_multiple_latency_ratio_Overall.png"
# set key font ",18" reverse Left top
# 
# plot  "../dat/figure_cubit-lf_latency_ratio.dat" every 5::4::34 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_latency_ratio.dat" every 5::4::34 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_latency_ratio.dat" every 5::4::34 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
#       "../dat/figure_ucb_latency_ratio.dat" every 5::4::34 title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints
#       "../dat/figure_naive_latency_ratio.dat" every 5::4::34 title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\
# 
# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_zipf/multiple/Figure_multiple_latency_ratio_Overall.eps"
# replot

# ############################################### for cardinality 

# #all_throughput_cardinality
# reset
# set size 0.99,1.0
# #set term pdf font ",10" size 5.2, 2.4
# set ylabel "Throughput (op/s)" font 'Linux Libertine O,25'
# set xlabel font 'Linux Libertine O,25' "Cardinality
# set yrange [0:]
# #set xrange [0:16]
# set terminal png font 'Linux Libertine O,15' size 400, 500
# set output "../graphs_zipf/multiple/Figure_multiple_throughput_cardinality.png"
# set key font ",18" reverse Left top

# plot  "../dat/figure_cubit-lf_throughput_cardinality.dat" title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_throughput_cardinality.dat" title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_throughput_cardinality.dat" title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
#       "../dat/figure_ucb_throughput_cardinality.dat" title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints, \
#       "../dat/figure_naive_throughput_cardinality.dat" title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\

# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_zipf/multiple/Figure_multiple_throughput_cardinality.eps"
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
# set output "../graphs_zipf/multiple/Figure_multiple_latency_cardinality_Query.png"
# set key font ",18" reverse Left top

# plot  "../dat/figure_cubit-lf_latency_cardinality.dat" every 5::0::30 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_latency_cardinality.dat" every 5::0::30 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_latency_cardinality.dat" every 5::0::30 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
#       "../dat/figure_ucb_latency_cardinality.dat" every 5::0::30 title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints
#       "../dat/figure_naive_latency_cardinality.dat" every 5::0::30 title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\

# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_zipf/multiple/Figure_multiple_latency_cardinality_Query.eps"
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
# set output "../graphs_zipf/multiple/Figure_multiple_latency_cardinality_Update.png"
# set key font ",18" reverse Left top

# plot  "../dat/figure_cubit-lf_latency_cardinality.dat" every 5::1::31 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_latency_cardinality.dat" every 5::1::31 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_latency_cardinality.dat" every 5::1::31 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
#       "../dat/figure_ucb_latency_cardinality.dat" every 5::1::31 title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints
#       "../dat/figure_naive_latency_cardinality.dat" every 5::1::31 title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\

# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_zipf/multiple/Figure_multiple_latency_cardinality_Update.eps"
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
# set output "../graphs_zipf/multiple/Figure_multiple_latency_cardinality_Insert.png"
# set key font ",18" reverse Left top

# plot  "../dat/figure_cubit-lf_latency_cardinality.dat" every 5::2::32 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_latency_cardinality.dat" every 5::2::32 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_latency_cardinality.dat" every 5::2::32 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
#       "../dat/figure_ucb_latency_cardinality.dat" every 5::2::32 title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints
#       "../dat/figure_naive_latency_cardinality.dat" every 5::2::32 title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\

# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_zipf/multiple/Figure_multiple_latency_cardinality_Insert.eps"
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
# set output "../graphs_zipf/multiple/Figure_multiple_latency_cardinality_Delete.png"
# set key font ",18" reverse Left top

# plot  "../dat/figure_cubit-lf_latency_cardinality.dat" every 5::3::33 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_latency_cardinality.dat" every 5::3::33 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_latency_cardinality.dat" every 5::3::33 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
#       "../dat/figure_ucb_latency_cardinality.dat" every 5::3::33 title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints
#       "../dat/figure_naive_latency_cardinality.dat" every 5::3::33 title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\

# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_zipf/multiple/Figure_multiple_latency_cardinality_Delete.eps"
# replot

# #all_latency_cardinality_Overall
# reset
# set size 0.99,1.0
# #set term pdf font ",10" size 5.2, 2.4
# set ylabel "Overall latency (ms)" font 'Linux Libertine O,25'
# set xlabel font 'Linux Libertine O,25' "Cardinality
# set yrange [0:]
# #set xrange [0:16]
# set terminal png font 'Linux Libertine O,15' size 400, 500
# set output "../graphs_zipf/multiple/Figure_multiple_latency_cardinality_Overall.png"
# set key font ",18" reverse Left top

# plot  "../dat/figure_cubit-lf_latency_cardinality.dat" every 5::4::34 title "CUBIT-lf" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_cubit-lk_latency_cardinality.dat" every 5::4::34 title "CUBIT-lk" lc rgb "blue" lw 8 ps 1.5 pt 6 with linespoints,\
#       "../dat/figure_ub_latency_cardinality.dat" every 5::4::34 title "UpBit" lc rgb "green" lw 8 ps 1.5 pt 10 dt 9 with linespoints,\
#       "../dat/figure_ucb_latency_cardinality.dat" every 5::4::34 title "UCB" lc rgb "brown" lw 8 ps 1.5 pt 8 dt "-" with linespoints
#       "../dat/figure_naive_latency_cardinality.dat" every 5::4::34 title "In-place" lc rgb "black" lw 8 ps 1.5 pt 12 dt 5 with linespoints,\

# set terminal eps font 'Linux Libertine O,17' 
# set output "../graphs_zipf/multiple/Figure_multiple_latency_cardinality_Overall.eps"
# replot
