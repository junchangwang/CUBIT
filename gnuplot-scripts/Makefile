PLOT=gnuplot 
all: make_dir make_dir_zipf make_dir_earth fence getvalue intro thres core_experiments tpch figure_single_sz figure_test_latency \
	figure_multiple_sz figure_multiple_sz_ub+cubit-lk+cubit-lf \
	figure_multiple_sz_earth figure_multiple_sz_ub+cubit-lk+cubit-lf_earth \
	figure_multiple_sz_zipf figure_multiple_sz_ub+cubit-lk+cubit-lf_zipf \
	figure_multiple_sz_ub+cubit-lk+cubit-lf_naive \
	figure_multiple_sz_ub+cubit-lk+cubit-lf_naive_zipf \
	figure_multiple_sz_ub+cubit-lk+cubit-lf_naive_earth \
	figure_multiple_sz_MT
	#temporarily removed:  mem upbit_vs_scan
make_dir:
	mkdir -p ../graphs
	mkdir -p ../graphs/multiple
#	mkdir -p ../graphs/multiple_ub+cubit-lk+cubit-lf
#	mkdir -p ../graphs/multiple_ub+cubit-lk+cubit-lf_naive

# make_dir_lk:
# 	mkdir -p ../graphs_lk
# 	mkdir -p ../graphs_lk/multiple_lk

make_dir_zipf:
	mkdir -p ../graphs_zipf
	mkdir -p ../graphs_zipf/multiple
#	mkdir -p ../graphs_zipf/multiple_ub+cubit-lk+cubit-lf
#	mkdir -p ../graphs_zipf/multiple_ub+cubit-lk+cubit-lf_naive

make_dir_earth:
	mkdir -p ../graphs_earth
	mkdir -p ../graphs_earth/multiple
#	mkdir -p ../graphs_earth/multiple_ub+cubit-lk+cubit-lf
#	mkdir -p ../graphs_earth/multiple_ub+cubit-lk+cubit-lf_naive

make_dir_1B:
	mkdir -p ../graphs_1B
	mkdir -p ../graphs_1B/multiple
#	mkdir -p ../graphs_1B/multiple_ub+cubit-lk+cubit-lf
#	mkdir -p ../graphs_1B/multiple_ub+cubit-lk+cubit-lf_naive

make_dir_MT:
	mkdir -p ../graphs_MT

make_dir_WPMT:
	mkdir -p ../graphs_WPMT

make_dir_Seg:
	mkdir -p ../graphs_Seg

make_dir_Para:
	mkdir -p ../graphs_Para

#Needs: ../dat/getvalue.dat
#Figure 21
getvalue: getvalue.gnu ../dat/getvalue.dat
	$(PLOT) getvalue.gnu
#Needs: ../dat/intro_U_D.dat ../dat/intro_EB.dat ../dat/intro_Q_D.dat ../dat/intro_Q_A.dat ../dat/breakdown_throughput.dat ../dat/figure_1_ucb.dat ../dat/figure_1_ub.dat
#Figure 1, 5, 6, 9
intro: intro.gnu ../dat/intro_U_D.dat ../dat/intro_EB.dat ../dat/intro_Q_D.dat ../dat/intro_Q_A.dat ../dat/breakdown_throughput.dat ../dat/figure_1_ucb.dat ../dat/figure_1_ub.dat
	$(PLOT) intro.gnu
#Needs: ../dat/thres.dat ../dat/thres_0.2.dat ../dat/threshold_normalized.dat
#Figure 19a, 19b, 19c
thres: thres.gnu ../dat/thres.dat ../dat/thres_0.2.dat ../dat/threshold_normalized.dat
	$(PLOT) thres.gnu
#Needs: ../dat/delete_new.dat ../dat/update_aggregate.dat ../dat/update_aggregate_fp.dat ../dat/insert_new.dat ../dat/mix_new.dat ../dat/mix_1000.dat ../dat/mix_zipf.dat ../dat/mix_earth_10.dat ../dat/mix_1B.dat ../dat/multi_latency.dat ../dat/update_breakup.dat
#Figure 10, 11,12a, 12b, 12c, 13, 14, 15, 16, 17, 22, 24
core_experiments: core_experiments.gnu ../dat/delete_new.dat ../dat/update_aggregate.dat ../dat/update_aggregate_fp.dat ../dat/insert_new.dat ../dat/mix_new.dat ../dat/mix_1000.dat ../dat/mix_zipf.dat ../dat/mix_earth_10.dat ../dat/mix_1B.dat ../dat/multi_latency.dat ../dat/update_breakup.dat
	$(PLOT) core_experiments.gnu
#Needs: ../dat/tpch_sel.dat
#Figure 27
tpch: tpch.gnu ../dat/tpch_sel.dat
	$(PLOT) tpch.gnu
#############
# THESE RECIPES MAY NOT BE EASY TO REPRODUCE
#Needs: ../dat/upbit_vs_scan.dat
#Figure 18
upbit_vs_scan: upbit_vs_scan.gnu ../dat/upbit_vs_scan.dat
	$(PLOT) upbit_vs_scan.gnu
#Needs: ../dat/mem_vm.dat ../dat/mem_ub.dat ../dat/mem_fp.dat ../dat/mem_eb.dat
#Figure 25
mem: mem.gnu ../dat/mem_vm.dat ../dat/mem_ub.dat ../dat/mem_fp.dat ../dat/mem_eb.dat
	$(PLOT) mem.gnu
#Needs: ../dat/fp_ub_su.dat ../dat/fp_ucb.dat ../dat/fp_ub.dat
#Figure 20, 23, 26
fence: fence.gnu ../dat/fp_ub_su.dat ../dat/fp_ucb.dat ../dat/fp_ub.dat
	$(PLOT) fence.gnu
# figure_single_sz: single_sz.gnu ../dat/figure_naive_throughput_core.dat ../dat/figure_naive_throughput_ratio.dat \
# 	   ../dat/figure_naive_latency_core.dat ../dat/figure_naive_latency_ratio.dat\
# 	   ../dat/figure_ucb_throughput_core.dat ../dat/figure_ucb_throughput_ratio.dat\
# 	   ../dat/figure_ucb_latency_core.dat ../dat/figure_ucb_latency_ratio.dat\
# 	   ../dat/figure_ub_throughput_core.dat ../dat/figure_ub_throughput_ratio.dat\
# 	   ../dat/figure_ub_latency_core.dat ../dat/figure_ub_latency_ratio.dat\
# 	   	../dat/figure_cubit-lk_throughput_core.dat ../dat/figure_cubit-lk_throughput_ratio.dat\
# 	   ../dat/figure_cubit-lk_latency_core.dat ../dat/figure_cubit-lk_latency_ratio.dat
# 	$(PLOT) single_sz.gnu

figure_multiple_sz: multiple_sz.gnu ../dat/figure_naive_throughput_core.dat ../dat/figure_naive_throughput_ratio.dat \
	   ../dat/figure_naive_latency_core.dat ../dat/figure_naive_latency_ratio.dat\
	   ../dat/figure_naive_latency_cardinality.dat ../dat/figure_naive_latency_cardinality.dat\
	   ../dat/figure_ucb_throughput_core.dat ../dat/figure_ucb_throughput_ratio.dat\
	   ../dat/figure_ucb_latency_core.dat ../dat/figure_ucb_latency_ratio.dat\
	   ../dat/figure_ucb_latency_cardinality.dat ../dat/figure_ucb_latency_cardinality.dat\
	   ../dat/figure_ub_throughput_core.dat ../dat/figure_ub_throughput_ratio.dat\
	   ../dat/figure_ub_latency_core.dat ../dat/figure_ub_latency_ratio.dat\
	   ../dat/figure_ub_latency_cardinality.dat ../dat/figure_ub_latency_cardinality.dat\
	   	../dat/figure_cubit-lk_throughput_core.dat ../dat/figure_cubit-lk_throughput_ratio.dat\
	   ../dat/figure_cubit-lk_latency_core.dat ../dat/figure_cubit-lk_latency_ratio.dat \
	   ../dat/figure_cubit-lk_latency_cardinality.dat ../dat/figure_cubit-lk_latency_cardinality.dat\
	   	../dat/figure_cubit-lf_throughput_core.dat ../dat/figure_cubit-lf_throughput_ratio.dat\
	   ../dat/figure_cubit-lf_latency_core.dat ../dat/figure_cubit-lf_latency_ratio.dat \
	   ../dat/figure_cubit-lf_latency_cardinality.dat ../dat/figure_cubit-lf_latency_cardinality.dat
	$(PLOT) multiple_sz.gnu

# figure_multiple_sz_ub+cubit-lk+cubit-lf:../dat/figure_ub_throughput_core.dat ../dat/figure_ub_throughput_ratio.dat\
# 	   ../dat/figure_ub_latency_core.dat ../dat/figure_ub_latency_ratio.dat\
# 	   	../dat/figure_ub_latency_cardinality.dat ../dat/figure_ub_latency_cardinality.dat\
# 	   	../dat/figure_cubit-lk_throughput_core.dat ../dat/figure_cubit-lk_throughput_ratio.dat\
# 	   ../dat/figure_cubit-lk_latency_core.dat ../dat/figure_cubit-lk_latency_ratio.dat \
# 	   ../dat/figure_cubit-lk_latency_cardinality.dat ../dat/figure_cubit-lk_latency_cardinality.dat\
# 	   	../dat/figure_cubit-lf_throughput_core.dat ../dat/figure_cubit-lf_throughput_ratio.dat\
# 	   ../dat/figure_cubit-lf_latency_core.dat ../dat/figure_cubit-lf_latency_ratio.dat \
# 		../dat/figure_cubit-lf_latency_cardinality.dat ../dat/figure_cubit-lf_latency_cardinality.dat
# 	$(PLOT) multiple_sz_ub+cubit-lk+cubit-lf.gnu

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

# figure_multiple_sz_ub+cubit-lk+cubit-lf_earth:../dat/figure_ub_throughput_core.dat \
# 	   ../dat/figure_ub_latency_core.dat \
# 	   	../dat/figure_cubit-lk_throughput_core.dat \
# 	   ../dat/figure_cubit-lk_latency_core.dat  \
# 	   	../dat/figure_cubit-lf_throughput_core.dat \
# 	   ../dat/figure_cubit-lf_latency_core.dat 
# 	$(PLOT) multiple_sz_ub+cubit-lk+cubit-lf_earth.gnu

figure_multiple_sz_zipf: multiple_sz.gnu ../dat/figure_naive_throughput_core.dat ../dat/figure_naive_latency_core.dat \
	   ../dat/figure_ucb_throughput_core.dat ../dat/figure_ucb_latency_core.dat \
	   ../dat/figure_ub_throughput_core.dat ../dat/figure_ub_latency_core.dat \
	   	../dat/figure_cubit-lk_throughput_core.dat ../dat/figure_cubit-lk_latency_core.dat \
	   	../dat/figure_cubit-lf_throughput_core.dat ../dat/figure_cubit-lf_latency_core.dat 
	$(PLOT) multiple_sz_zipf.gnu

# figure_multiple_sz_ub+cubit-lk+cubit-lf_zipf:../dat/figure_ub_throughput_core.dat ../dat/figure_ub_latency_core.dat \
# 	   	../dat/figure_cubit-lk_throughput_core.dat ../dat/figure_cubit-lk_latency_core.dat \
# 	   	../dat/figure_cubit-lf_throughput_core.dat ../dat/figure_cubit-lf_latency_core.dat 
# 	$(PLOT) multiple_sz_ub+cubit-lk+cubit-lf_zipf.gnu


# naive & cubit-lk\lf & ub
# figure_multiple_sz_ub+cubit-lk+cubit-lf_naive: multiple_sz.gnu ../dat/figure_naive_throughput_core.dat ../dat/figure_naive_throughput_ratio.dat \
# 	   ../dat/figure_naive_latency_core.dat ../dat/figure_naive_latency_ratio.dat\
# 	   ../dat/figure_naive_latency_cardinality.dat ../dat/figure_naive_latency_cardinality.dat\
# 	   ../dat/figure_ub_throughput_core.dat ../dat/figure_ub_throughput_ratio.dat\
# 	   ../dat/figure_ub_latency_core.dat ../dat/figure_ub_latency_ratio.dat\
# 	   ../dat/figure_ub_latency_cardinality.dat ../dat/figure_ub_latency_cardinality.dat\
# 	   	../dat/figure_cubit-lk_throughput_core.dat ../dat/figure_cubit-lk_throughput_ratio.dat\
# 	   ../dat/figure_cubit-lk_latency_core.dat ../dat/figure_cubit-lk_latency_ratio.dat \
# 	   ../dat/figure_cubit-lk_latency_cardinality.dat ../dat/figure_cubit-lk_latency_cardinality.dat\
# 	   	../dat/figure_cubit-lf_throughput_core.dat ../dat/figure_cubit-lf_throughput_ratio.dat\
# 	   ../dat/figure_cubit-lf_latency_core.dat ../dat/figure_cubit-lf_latency_ratio.dat \
# 	   ../dat/figure_cubit-lf_latency_cardinality.dat ../dat/figure_cubit-lf_latency_cardinality.dat
# 	$(PLOT) multiple_sz_ub+cubit-lk+cubit-lf_naive.gnu

# figure_multiple_sz_ub+cubit-lk+cubit-lf_naive_zipf:../dat/figure_ub_throughput_core.dat ../dat/figure_ub_latency_core.dat \
# 	   	../dat/figure_cubit-lk_throughput_core.dat ../dat/figure_cubit-lk_latency_core.dat \
# 	   	../dat/figure_cubit-lf_throughput_core.dat ../dat/figure_cubit-lf_latency_core.dat \
# 	   ../dat/figure_naive_throughput_core.dat ../dat/figure_naive_latency_core.dat 
# 	$(PLOT) multiple_sz_ub+cubit-lk+cubit-lf_naive_zipf.gnu

# figure_multiple_sz_ub+cubit-lk+cubit-lf_naive_earth:../dat/figure_ub_throughput_core.dat ../dat/figure_ub_throughput_ratio.dat\
# 	   ../dat/figure_ub_latency_core.dat ../dat/figure_ub_latency_ratio.dat\
# 	   	../dat/figure_cubit-lk_throughput_core.dat ../dat/figure_cubit-lk_throughput_ratio.dat\
# 	   ../dat/figure_cubit-lk_latency_core.dat ../dat/figure_cubit-lk_latency_ratio.dat \
# 	   	../dat/figure_cubit-lf_throughput_core.dat ../dat/figure_cubit-lf_throughput_ratio.dat\
# 	   ../dat/figure_cubit-lf_latency_core.dat ../dat/figure_cubit-lf_latency_ratio.dat \
# 	   ../dat/figure_naive_throughput_core.dat ../dat/figure_naive_throughput_ratio.dat \
# 	   ../dat/figure_naive_latency_core.dat ../dat/figure_naive_latency_ratio.dat
# 	$(PLOT) multiple_sz_ub+cubit-lk+cubit-lf_naive_earth.gnu


# for 1B_100
figure_multiple_sz_1B:  ../dat/figure_naive_throughput_core.dat ../dat/figure_ucb_throughput_core.dat ../dat/figure_ub_throughput_core.dat ../dat/figure_cubit-lk_throughput_core.dat ../dat/figure_cubit-lf_throughput_core.dat 
	$(PLOT) multiple_sz_1B.gnu

# figure_multiple_sz_ub+cubit-lk+cubit-lf_1B:../dat/figure_ub_throughput_core.dat ../dat/figure_ub_latency_core.dat \
# 	   	../dat/figure_cubit-lk_throughput_core.dat ../dat/figure_cubit-lk_latency_core.dat \
# 	   	../dat/figure_cubit-lf_throughput_core.dat ../dat/figure_cubit-lf_latency_core.dat 
# 	$(PLOT) multiple_sz_ub+cubit-lk+cubit-lf_1B.gnu

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


# figure_multiple_sz_lk: ../dat/figure_naive_throughput_core.dat ../dat/figure_naive_throughput_ratio.dat \
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
# 	$(PLOT) multiple_sz_lk.gnu
