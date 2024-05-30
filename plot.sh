#!/bin/bash

chmod -R 755 eva-scripts
chmod -R 755 gnuplot-scripts
mkdir -p ./output

# core
if [ -z "$1" ] || [ "$1" == "core" ]
then
    rm -rf ./output/plot_core
    mkdir  ./output/plot_core
    mv ./graphs_core_0 ./graphs_0
    mv ./graphs_core_1 ./graphs_1
    mv ./graphs_core_2 ./graphs_2
    ./eva-scripts/gen_graphs_core.sh graphs_0 &> ./output/plot_core/graphs_0.output
    ./eva-scripts/gen_graphs_core.sh graphs_1 &> ./output/plot_core/graphs_1.output
    ./eva-scripts/gen_graphs_core.sh graphs_2 &> ./output/plot_core/graphs_2.output
    mv ./graphs_0 ./graphs_core_0
    mv ./graphs_1 ./graphs_core_1
    mv ./graphs_2 ./graphs_core_2

# 1B
elif [ "$1" == "1B" ]
then
    rm -rf ./output/plot_1B
    mkdir  ./output/plot_1B
    ./eva-scripts/gen_graphs_1B.sh graphs_1B_0 &> ./output/plot_1B/graphs_1B_0.output
    ./eva-scripts/gen_graphs_1B.sh graphs_1B_1 &> ./output/plot_1B/graphs_1B_1.output

# earth
elif [ "$1" == "earth" ]
then
    rm -rf ./output/plot_earth
    mkdir  ./output/plot_earth
    ./eva-scripts/gen_graphs_earth.sh graphs_earth_0 &> ./output/plot_earth/graphs_earth_0.output
    ./eva-scripts/gen_graphs_earth.sh graphs_earth_1 &> ./output/plot_earth/graphs_earth_1.output

# MT
elif [ "$1" == "MT" ]
then
    rm -rf ./output/plot_MT
    mkdir  ./output/plot_MT
    ./eva-scripts/gen_graphs_MT.sh graphs_MT_0 &> ./output/plot_MT/graphs_MT_0.output
    ./eva-scripts/gen_graphs_MT.sh graphs_MT_1 &> ./output/plot_MT/graphs_MT_1.output

# Para
elif [ "$1" == "Para" ]
then
    rm -rf ./output/plot_Para
    mkdir  ./output/plot_Para
    ./eva-scripts/gen_graphs_Para.sh graphs_Para_0 &> ./output/plot_Para/graphs_Para_0.output
    ./eva-scripts/gen_graphs_Para.sh graphs_Para_1 &> ./output/plot_Para/graphs_Para_1.output

# Seg
elif [ "$1" == "Seg" ]
then
    rm -rf ./output/plot_Seg
    mkdir  ./output/plot_Seg
    ./eva-scripts/gen_graphs_Seg.sh graphs_Seg_0 &> ./output/plot_Seg/graphs_Seg_0.output
    ./eva-scripts/gen_graphs_Seg.sh graphs_Seg_1 &> ./output/plot_Seg/graphs_Seg_1.output

# WPMT
elif [ "$1" == "WPMT" ]
then
    rm -rf ./output/plot_WPMT
    mkdir  ./output/plot_WPMT
    ./eva-scripts/gen_graphs_WPMT.sh graphs_WPMT_0 &> ./output/plot_WPMT/graphs_WPMT_0.output
    ./eva-scripts/gen_graphs_WPMT.sh graphs_WPMT_1 &> ./output/plot_WPMT/graphs_WPMT_1.output
    ./eva-scripts/gen_graphs_WPMT.sh graphs_WPMT_2 &> ./output/plot_WPMT/graphs_WPMT_2.output

# zipf
elif [ "$1" == "zipf" ]
then
    rm -rf ./output/plot_zipf
    mkdir  ./output/plot_zipf
    ./eva-scripts/gen_graphs_zipf.sh graphs_zipf_0 &> ./output/plot_zipf/graphs_zipf_0.output
    ./eva-scripts/gen_graphs_zipf.sh graphs_zipf_1 &> ./output/plot_zipf/graphs_zipf_1.output
    ./eva-scripts/gen_graphs_zipf.sh graphs_zipf_2 &> ./output/plot_zipf/graphs_zipf_2.output

elif [ "$1" == "all" ]
then
    # core
    rm -rf ./output/plot_core
    mkdir  ./output/plot_core
    ./eva-scripts/gen_graphs_core.sh graphs_0 &> ./output/plot_core/graphs_0.output
    ./eva-scripts/gen_graphs_core.sh graphs_1 &> ./output/plot_core/graphs_1.output
    ./eva-scripts/gen_graphs_core.sh graphs_2 &> ./output/plot_core/graphs_2.output

    # 1B
    rm -rf ./output/plot_1B
    mkdir  ./output/plot_1B
    ./eva-scripts/gen_graphs_1B.sh graphs_1B_0 &> ./output/plot_1B/graphs_1B_0.output
    ./eva-scripts/gen_graphs_1B.sh graphs_1B_1 &> ./output/plot_1B/graphs_1B_1.output

    # earth
    rm -rf ./output/plot_earth
    mkdir  ./output/plot_earth
    ./eva-scripts/gen_graphs_earth.sh graphs_earth_0 &> ./output/plot_earth/graphs_earth_0.output
    ./eva-scripts/gen_graphs_earth.sh graphs_earth_1 &> ./output/plot_earth/graphs_earth_1.output

    # MT
    rm -rf ./output/plot_MT
    mkdir  ./output/plot_MT
    ./eva-scripts/gen_graphs_MT.sh graphs_MT_0 &> ./output/plot_MT/graphs_MT_0.output
    ./eva-scripts/gen_graphs_MT.sh graphs_MT_1 &> ./output/plot_MT/graphs_MT_1.output

    # Para
    rm -rf ./output/plot_Para
    mkdir  ./output/plot_Para
    ./eva-scripts/gen_graphs_Para.sh graphs_Para_0 &> ./output/plot_Para/graphs_Para_0.output
    ./eva-scripts/gen_graphs_Para.sh graphs_Para_1 &> ./output/plot_Para/graphs_Para_1.output

    # Seg
    rm -rf ./output/plot_Seg
    mkdir  ./output/plot_Seg
    ./eva-scripts/gen_graphs_Seg.sh graphs_Seg_0 &> ./output/plot_Seg/graphs_Seg_0.output
    ./eva-scripts/gen_graphs_Seg.sh graphs_Seg_1 &> ./output/plot_Seg/graphs_Seg_1.output

    # WPMT
    rm -rf ./output/plot_WPMT
    mkdir  ./output/plot_WPMT
    ./eva-scripts/gen_graphs_WPMT.sh graphs_WPMT_0 &> ./output/plot_WPMT/graphs_WPMT_0.output
    ./eva-scripts/gen_graphs_WPMT.sh graphs_WPMT_1 &> ./output/plot_WPMT/graphs_WPMT_1.output
    ./eva-scripts/gen_graphs_WPMT.sh graphs_WPMT_2 &> ./output/plot_WPMT/graphs_WPMT_2.output

    # zipf
    rm -rf ./output/plot_zipf
    mkdir  ./output/plot_zipf
    ./eva-scripts/gen_graphs_zipf.sh graphs_zipf_0 &> ./output/plot_zipf/graphs_zipf_0.output
    ./eva-scripts/gen_graphs_zipf.sh graphs_zipf_1 &> ./output/plot_zipf/graphs_zipf_1.output
    ./eva-scripts/gen_graphs_zipf.sh graphs_zipf_2 &> ./output/plot_zipf/graphs_zipf_2.output

else
    echo "Please specify the correct experiment!"
    exit
fi

exit