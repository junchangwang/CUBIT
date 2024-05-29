#!/bin/bash

chmod -R 755 eva-scripts
chmod -R 755 gnuplot-scripts

if [ -z $1 ]
then
    echo "Please specify the correct expriment!"
    exit
fi

# core
if [ $1 == "core" ] || [ $1 == "ALL" ]
then
    ./eva-scripts/gen_graphs_core.sh graphs_0
    ./eva-scripts/gen_graphs_core.sh graphs_1
    ./eva-scripts/gen_graphs_core.sh graphs_2

# 1B
elif [ $1 == "1B" ] || [ $1 == "ALL" ]
then
    ./eva-scripts/gen_graphs_1B.sh graphs_1B_0
    ./eva-scripts/gen_graphs_1B.sh graphs_1B_1

# earth
elif [ $1 == "earth" ] || [ $1 == "ALL" ]
then
    ./eva-scripts/gen_graphs_earth.sh graphs_earth_0
    ./eva-scripts/gen_graphs_earth.sh graphs_earth_1

# MT
elif [ $1 == "MT" ] || [ $1 == "ALL" ]
then
    ./eva-scripts/gen_graphs_MT.sh graphs_MT_0
    ./eva-scripts/gen_graphs_MT.sh graphs_MT_1

# Para
elif [ $1 == "Para" ] || [ $1 == "ALL" ]
then
    ./eva-scripts/gen_graphs_Para.sh graphs_Para_0
    ./eva-scripts/gen_graphs_Para.sh graphs_Para_1

# Seg
elif [ $1 == "Seg" ] || [ $1 == "ALL" ]
then
    ./eva-scripts/gen_graphs_Seg.sh graphs_Seg_0
    ./eva-scripts/gen_graphs_Seg.sh graphs_Seg_1

# WPMT
elif [ $1 == "WPMT" ] || [ $1 == "ALL" ]
then
    ./eva-scripts/gen_graphs_WPMT.sh graphs_WPMT_0
    ./eva-scripts/gen_graphs_WPMT.sh graphs_WPMT_1
    ./eva-scripts/gen_graphs_WPMT.sh graphs_WPMT_2

# zipf
elif [ $1 == "zipf" ] || [ $1 == "ALL" ]
then
    ./eva-scripts/gen_graphs_zipf.sh graphs_zipf_0
    ./eva-scripts/gen_graphs_zipf.sh graphs_zipf_1
    ./eva-scripts/gen_graphs_zipf.sh graphs_zipf_2

else
    echo "Please specify the correct expriment!"
fi

exit