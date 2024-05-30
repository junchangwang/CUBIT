#!/bin/bash

rm -rf ./run.output
script -a ./run.output

if [ -z "$1" ]
then
    python3 ./eva-scripts/run_core.py
fi

if [ "$1" == "all" ]
then
    python3 ./eva-scripts/run_core.py
    python3 ./eva-scripts/run_1B.py
    python3 ./eva-scripts/run_earth.py
    python3 ./eva-scripts/run_MT.py
    python3 ./eva-scripts/run_Para.py
    python3 ./eva-scripts/run_Seg.py
    python3 ./eva-scripts/run_WPMT.py
    python3 ./eva-scripts/run_zipf.py

elif [ -a ./eva-scripts/run_"$1".py ]
then
    python3 ./eva-scripts/run_"$1".py

else
    echo "Please specify the correct experiment!"
    exit
fi

mv graphs_0 graphs_core_0
mv graphs_1 graphs_core_1
mv graphs_2 graphs_core_2

exit