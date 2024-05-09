PLOT=gnuplot 
all: make_dir make_dir_zipf make_dir_earth fence getvalue intro thres core_experiments tpch figure_single_sz figure_test_latency \
	figure_multiple_sz figure_multiple_sz_ub+cubit-lk+cubit-lf \
	figure_multiple_sz_earth figure_multiple_sz_ub+cubit-lk+cubit-lf_earth \
	figure_multiple_sz_zipf figure_multiple_sz_ub+cubit-lk+cubit-lf_zipf \
	figure_multiple_sz_ub+cubit-lk+cubit-lf_naive \
	figure_multiple_sz_ub+cubit-lk+cubit-lf_naive_zipf \
	figure_multiple_sz_ub+cubit-lk+cubit-lf_naive_earth \
	figure_multiple_sz_MT

make_dir:
	mkdir -p ../graphs
	mkdir -p ../graphs/multiple

make_dir_zipf:
	mkdir -p ../graphs_zipf
	mkdir -p ../graphs_zipf/multiple

make_dir_earth:
	mkdir -p ../graphs_earth
	mkdir -p ../graphs_earth/multiple

make_dir_1B:
	mkdir -p ../graphs_1B
	mkdir -p ../graphs_1B/multiple

make_dir_MT:
	mkdir -p ../graphs_MT

make_dir_WPMT:
	mkdir -p ../graphs_WPMT

make_dir_Seg:
	mkdir -p ../graphs_Seg

make_dir_Para:
	mkdir -p ../graphs_Para

# figure_multiple_sz: multiple_sz.gnu ../dat/figure_naive_throughput_core.dat ../dat/figure_naive_throughput_ratio.dat \
# 	   ../dat/figure_naive_latency_core.dat ../dat/figure_naive_latency_ratio.dat\
# 	   ../dat/figure_naive_latency_cardinality.dat ../dat/figure_naive_latency_cardinality.dat\
# 	   ../dat/figure_ucb_throughput_core.dat ../dat/figure_ucb_throughput_ratio.dat\
# 	   ../dat/figure_ucb_latency_core.dat ../dat/figure_ucb_latency_ratio.dat\
# 	   ../dat/figure_ucb_latency_cardinality.dat ../dat/figure_ucb_latency_cardinality.dat\
# 	   ../dat/figure_ub_throughput_core.dat ../dat/figure_ub_throughput_ratio.dat\
# 	   ../dat/figure_ub_latency_core.dat ../dat/figure_ub_latency_ratio.dat\
# 	   ../dat/figure_ub_latency_cardinality.dat ../dat/figure_ub_latency_cardinality.dat\
# 	   	../dat/figure_cubit-lk_throughput_core.dat ../dat/figure_cubit-lk_throughput_ratio.dat\
# 	   ../dat/figure_cubit-lk_latency_core.dat ../dat/figure_cubit-lk_latency_ratio.dat \
# 	   ../dat/figure_cubit-lk_latency_cardinality.dat ../dat/figure_cubit-lk_latency_cardinality.dat\
# 	   	../dat/figure_cubit-lf_throughput_core.dat ../dat/figure_cubit-lf_throughput_ratio.dat\
# 	   ../dat/figure_cubit-lf_latency_core.dat ../dat/figure_cubit-lf_latency_ratio.dat \
# 	   ../dat/figure_cubit-lf_latency_cardinality.dat ../dat/figure_cubit-lf_latency_cardinality.dat
# 	$(PLOT) multiple_sz.gnu

figure_multiple_sz: multiple_sz.gnu ../dat/figure_naive_throughput_core.dat ../dat/figure_naive_throughput_ratio.dat \
	   ../dat/figure_naive_latency_core.dat ../dat/figure_naive_latency_ratio.dat\
	   ../dat/figure_ucb_throughput_core.dat ../dat/figure_ucb_throughput_ratio.dat\
	   ../dat/figure_ucb_latency_core.dat ../dat/figure_ucb_latency_ratio.dat\
	   ../dat/figure_ub_throughput_core.dat ../dat/figure_ub_throughput_ratio.dat\
	   ../dat/figure_ub_latency_core.dat ../dat/figure_ub_latency_ratio.dat\
	   	../dat/figure_cubit-lk_throughput_core.dat ../dat/figure_cubit-lk_throughput_ratio.dat\
	   ../dat/figure_cubit-lk_latency_core.dat ../dat/figure_cubit-lk_latency_ratio.dat \
	   	../dat/figure_cubit-lf_throughput_core.dat ../dat/figure_cubit-lf_throughput_ratio.dat\
	   ../dat/figure_cubit-lf_latency_core.dat ../dat/figure_cubit-lf_latency_ratio.dat
	$(PLOT) multiple_sz.gnu

figure_test_latency: ../dat/figure_ub_latency_core.dat ../dat/figure_ub_latency_ratio.dat\
			../dat/figure_cubit-lk_latency_core.dat ../dat/figure_cubit-lk_latency_ratio.dat \
			../dat/figure_cubit-lf_latency_core.dat ../dat/figure_cubit-lf_latency_ratio.dat
	$(PLOT) test_latency.gnu

# for zipf & earth
figure_multiple_sz_earth: multiple_sz.gnu ../dat/figure_naive_throughput_core.dat  \
	   ../dat/figure_ucb_throughput_core.dat \
	   ../dat/figure_ub_throughput_core.dat .\
	   	../dat/figure_cubit-lk_throughput_core.dat \
	   	../dat/figure_cubit-lf_throughput_core.dat
	$(PLOT) multiple_sz_earth.gnu

figure_multiple_sz_zipf: multiple_sz.gnu ../dat/figure_naive_throughput_core.dat ../dat/figure_naive_latency_core.dat \
	   ../dat/figure_ucb_throughput_core.dat ../dat/figure_ucb_latency_core.dat \
	   ../dat/figure_ub_throughput_core.dat ../dat/figure_ub_latency_core.dat \
	   	../dat/figure_cubit-lk_throughput_core.dat ../dat/figure_cubit-lk_latency_core.dat \
	   	../dat/figure_cubit-lf_throughput_core.dat ../dat/figure_cubit-lf_latency_core.dat 
	$(PLOT) multiple_sz_zipf.gnu

# for 1B_100
figure_multiple_sz_1B:  ../dat/figure_naive_throughput_core.dat ../dat/figure_ucb_throughput_core.dat ../dat/figure_ub_throughput_core.dat ../dat/figure_cubit-lk_throughput_core.dat ../dat/figure_cubit-lf_throughput_core.dat 
	$(PLOT) multiple_sz_1B.gnu

# for merge threshold
figure_multiple_sz_MT: ../dat/figure_cubit-lk_latency_MT.dat 
	$(PLOT) multiple_sz_MT.gnu

# for workers per merge threshold
figure_multiple_sz_WPMT: ../dat/figure_cubit-lf_latency_WPMT.dat 
	$(PLOT) multiple_sz_WPMT.gnu

# for segments per bitvector
figure_multiple_sz_Seg: ../dat/figure_cubit-lk_latency_Seg.dat 
	$(PLOT) multiple_sz_Seg.gnu

# for parallelism for counting each bitvector
figure_multiple_sz_Para: ../dat/figure_cubit-lk_latency_Para.dat 
	$(PLOT) multiple_sz_Para.gnu
