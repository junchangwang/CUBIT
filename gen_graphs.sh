chmod -R 755 eva-scripts
chmod -R 755 gnuplot-scripts

./eva-scripts/gen_graphs_core.sh graphs_0
./eva-scripts/gen_graphs_core.sh graphs_1
./eva-scripts/gen_graphs_core.sh graphs_2

./eva-scripts/gen_graphs_1B.sh graphs_1B_0
./eva-scripts/gen_graphs_1B.sh graphs_1B_1

./eva-scripts/gen_graphs_earth.sh graphs_earth_0
./eva-scripts/gen_graphs_earth.sh graphs_earth_1

./eva-scripts/gen_graphs_MT.sh graphs_MT_0
./eva-scripts/gen_graphs_MT.sh graphs_MT_1

./eva-scripts/gen_graphs_Para.sh graphs_Para_0
./eva-scripts/gen_graphs_Para.sh graphs_Para_1

./eva-scripts/gen_graphs_Seg.sh graphs_Seg_0
./eva-scripts/gen_graphs_Seg.sh graphs_Seg_1

./eva-scripts/gen_graphs_WPMT.sh graphs_WPMT_0
./eva-scripts/gen_graphs_WPMT.sh graphs_WPMT_1
./eva-scripts/gen_graphs_WPMT.sh graphs_WPMT_2

./eva-scripts/gen_graphs_zipf.sh graphs_zipf_0
./eva-scripts/gen_graphs_zipf.sh graphs_zipf_1
./eva-scripts/gen_graphs_zipf.sh graphs_zipf_2

echo 'Done!'
