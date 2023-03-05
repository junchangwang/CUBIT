#!/bin/bash

CARDINALITY=100
NoTUPLES=100000000
#IDXPATH=/home/yan/codes/nicolas/100M/
#DATAPATH=/home/yan/codes/nicolas/a_100M
IDXPATH="$PWD/100M_100/"

./build/nicolas -m getvalue --num-threads 16 -c $CARDINALITY -i $IDXPATH -n 100000000 --fence-pointer yes -a ub > getvalue_16
./build/nicolas -m getvalue --num-threads 8 -c $CARDINALITY -i $IDXPATH -n 100000000 --fence-pointer yes -a ub > getvalue_8
./build/nicolas -m getvalue --num-threads 4 -c $CARDINALITY -i $IDXPATH -n 100000000 --fence-pointer yes -a ub > getvalue_4
./build/nicolas -m getvalue --num-threads 2 -c $CARDINALITY -i $IDXPATH -n 100000000 --fence-pointer yes -a ub > getvalue_2
./build/nicolas -m getvalue --num-threads 1 -c $CARDINALITY -i $IDXPATH -n 100000000 --fence-pointer yes -a ub > getvalue_1
