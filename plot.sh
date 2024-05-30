#!/bin/bash

rm -rf ./plot.output
script -a ./plot.output

chmod -R 755 eva-scripts
chmod -R 755 gnuplot-scripts

# core
if [ -z "$1" ] || [ "$1" == "core" ]
then
    mv graphs_core_0 graphs_0
    mv graphs_core_1 graphs_1
    mv graphs_core_2 graphs_2

    ./eva-scripts/gen_graphs_core.sh graphs_0
    ./eva-scripts/gen_graphs_core.sh graphs_1
    ./eva-scripts/gen_graphs_core.sh graphs_2

    mv graphs_0      graphs_core_0
    mv graphs_1      graphs_core_1
    mv graphs_2      graphs_core_2

# 1B
elif [ "$1" == "1B" ]
then
    ./eva-scripts/gen_graphs_1B.sh graphs_1B_0
    ./eva-scripts/gen_graphs_1B.sh graphs_1B_1

# earth
elif [ "$1" == "earth" ]
then
    ./eva-scripts/gen_graphs_earth.sh graphs_earth_0
    ./eva-scripts/gen_graphs_earth.sh graphs_earth_1

# MT
elif [ "$1" == "MT" ]
then
    ./eva-scripts/gen_graphs_MT.sh graphs_MT_0
    ./eva-scripts/gen_graphs_MT.sh graphs_MT_1

# Para
elif [ "$1" == "Para" ]
then
    ./eva-scripts/gen_graphs_Para.sh graphs_Para_0
    ./eva-scripts/gen_graphs_Para.sh graphs_Para_1

# Seg
elif [ "$1" == "Seg" ]
then
    ./eva-scripts/gen_graphs_Seg.sh graphs_Seg_0
    ./eva-scripts/gen_graphs_Seg.sh graphs_Seg_1

# WPMT
elif [ "$1" == "WPMT" ]
then
    ./eva-scripts/gen_graphs_WPMT.sh graphs_WPMT_0
    ./eva-scripts/gen_graphs_WPMT.sh graphs_WPMT_1
    ./eva-scripts/gen_graphs_WPMT.sh graphs_WPMT_2

# zipf
elif [ "$1" == "zipf" ]
then
    ./eva-scripts/gen_graphs_zipf.sh graphs_zipf_0
    ./eva-scripts/gen_graphs_zipf.sh graphs_zipf_1
    ./eva-scripts/gen_graphs_zipf.sh graphs_zipf_2

# all
elif [ "$1" == "all" ]
then
    # core
    ./eva-scripts/gen_graphs_core.sh graphs_0
    ./eva-scripts/gen_graphs_core.sh graphs_1
    ./eva-scripts/gen_graphs_core.sh graphs_2

    # 1B
    ./eva-scripts/gen_graphs_1B.sh graphs_1B_0
    ./eva-scripts/gen_graphs_1B.sh graphs_1B_1

    # earth
    ./eva-scripts/gen_graphs_earth.sh graphs_earth_0
    ./eva-scripts/gen_graphs_earth.sh graphs_earth_1

    # MT
    ./eva-scripts/gen_graphs_MT.sh graphs_MT_0
    ./eva-scripts/gen_graphs_MT.sh graphs_MT_1

    # Para
    ./eva-scripts/gen_graphs_Para.sh graphs_Para_0
    ./eva-scripts/gen_graphs_Para.sh graphs_Para_1

    # Seg
    ./eva-scripts/gen_graphs_Seg.sh graphs_Seg_0
    ./eva-scripts/gen_graphs_Seg.sh graphs_Seg_1

    # WPMT
    ./eva-scripts/gen_graphs_WPMT.sh graphs_WPMT_0
    ./eva-scripts/gen_graphs_WPMT.sh graphs_WPMT_1
    ./eva-scripts/gen_graphs_WPMT.sh graphs_WPMT_2

    # zipf
    ./eva-scripts/gen_graphs_zipf.sh graphs_zipf_0
    ./eva-scripts/gen_graphs_zipf.sh graphs_zipf_1
    ./eva-scripts/gen_graphs_zipf.sh graphs_zipf_2

else
    echo "Please specify the correct experiment!"
    exit
fi

exit