#!/bin/bash

chmod -R 755 eva-scripts
chmod -R 755 gnuplot-scripts
mkdir -p ./output

# core
if [ -z "$1" ] || [ "$1" == "core" ] || [ "$1" == "all" ]; then
    mv ./graphs_core_0 ./graphs_0
    mv ./graphs_core_1 ./graphs_1
    mv ./graphs_core_2 ./graphs_2
    ./eva-scripts/gen_graphs_core.sh graphs_0 2>  ./output/plot_core.output
    ./eva-scripts/gen_graphs_core.sh graphs_1 2>> ./output/plot_core.output
    ./eva-scripts/gen_graphs_core.sh graphs_2 2>> ./output/plot_core.output
    mv ./graphs_0 ./graphs_core_0
    mv ./graphs_1 ./graphs_core_1
    mv ./graphs_2 ./graphs_core_2
fi

# 1B
if [ "$1" == "1B" ] || [ "$1" == "all" ]; then
    ./eva-scripts/gen_graphs_1B.sh graphs_1B_0 2>  ./output/plot_1B.output
    ./eva-scripts/gen_graphs_1B.sh graphs_1B_1 2>> ./output/plot_1B.output
fi

# earth
if [ "$1" == "earth" ] || [ "$1" == "all" ]; then
    ./eva-scripts/gen_graphs_earth.sh graphs_earth_0 2>  ./output/plot_earth.output
    ./eva-scripts/gen_graphs_earth.sh graphs_earth_1 2>> ./output/plot_earth.output
fi

# MT
if [ "$1" == "MT" ] || [ "$1" == "all" ]; then
    ./eva-scripts/gen_graphs_MT.sh graphs_MT_0 2>  ./output/plot_MT.output
    ./eva-scripts/gen_graphs_MT.sh graphs_MT_1 2>> ./output/plot_MT.output
fi

# Para
if [ "$1" == "Para" ] || [ "$1" == "all" ]; then
    ./eva-scripts/gen_graphs_Para.sh graphs_Para_0 2>  ./output/plot_Para.output
    ./eva-scripts/gen_graphs_Para.sh graphs_Para_1 2>> ./output/plot_Para.output
fi

# Seg
if [ "$1" == "Seg" ] || [ "$1" == "all" ]; then
    ./eva-scripts/gen_graphs_Seg.sh graphs_Seg_0 2>  ./output/plot_Seg.output
    ./eva-scripts/gen_graphs_Seg.sh graphs_Seg_1 2>> ./output/plot_Seg.output
fi

# WPMT
if [ "$1" == "WPMT" ] || [ "$1" == "all" ]; then
    ./eva-scripts/gen_graphs_WPMT.sh graphs_WPMT_0 2>  ./output/plot_WPMT.output
    ./eva-scripts/gen_graphs_WPMT.sh graphs_WPMT_1 2>> ./output/plot_WPMT.output
    ./eva-scripts/gen_graphs_WPMT.sh graphs_WPMT_2 2>> ./output/plot_WPMT.output
fi

# zipf
if [ "$1" == "zipf" ] || [ "$1" == "all" ]; then
    ./eva-scripts/gen_graphs_zipf.sh graphs_zipf_0 2>  ./output/plot_zipf.output
    ./eva-scripts/gen_graphs_zipf.sh graphs_zipf_1 2>> ./output/plot_zipf.output
    ./eva-scripts/gen_graphs_zipf.sh graphs_zipf_2 2>> ./output/plot_zipf.output
else
    echo "Please specify the correct experiment!"
    exit
fi

echo "Done!!!"

exit
