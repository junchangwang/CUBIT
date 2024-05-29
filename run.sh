#!/bin/bash

rm -rf ./output
mkdir ./output

if [ -z $1 ]
then
    python3 ./eva-scripts/run_core.py  &> ./output/run_core.output
    exit
fi

if [ $1 == "all" ]
then
    python3 ./eva-scripts/run_core.py  &> ./output/run_core.output
    python3 ./eva-scripts/run_1B.py    &> ./output/run_1B.output
    python3 ./eva-scripts/run_earth.py &> ./output/run_earth.output
    python3 ./eva-scripts/run_MT.py    &> ./output/run_MT.output
    python3 ./eva-scripts/run_Para.py  &> ./output/run_Para.output
    python3 ./eva-scripts/run_Seg.py   &> ./output/run_Seg.output
    python3 ./eva-scripts/run_WPMT.py  &> ./output/run_WPMT.output
    python3 ./eva-scripts/run_zipf.py  &> ./output/run_zipf.output

elif [ -a ./eva-scripts/run_$1.py ]
then
    python3 ./eva-scripts/run_$1.py

else
    echo "Please specify the correct experiment!"
fi

exit