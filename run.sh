#!/bin/bash

if [ -z $1 ]
then
    echo "Please specify the correct expriment!"
    exit
fi

if [ $1 == "ALL" ]
then
    python3 ./eva-scripts/run_core.py
    python3 ./eva-scripts/run_1B.py
    python3 ./eva-scripts/run_earth.py
    python3 ./eva-scripts/run_MT.py
    python3 ./eva-scripts/run_Para.py
    python3 ./eva-scripts/run_Seg.py
    python3 ./eva-scripts/run_WPMT.py
    python3 ./eva-scripts/run_zipf.py

elif [ -a ./eva-scripts/run_$1.py ]
then
    python3 ./eva-scripts/run_$1.py

else
    echo "Please specify the correct experiment!"
fi

exit