#!/bin/bash

if [ -n "$1" ]; then
	echo "To process directory $1"
else
	echo "Must specify the directory containing experimental results."
	exit
fi

rm -fr dat
rm -fr dat_tmp
rm -fr graphs_core
rm -fr $1/graphs_core

cp -r $1/dat $1/dat_tmp ./

cd gnuplot-scripts
make make_dir
make figure_multiple
# make figure_multiple_ub+cubit-lk+cubit-lf

cd ../
python3 eva-scripts/cdf_core.py > graphs_core/cdf_output.txt

python3 figure1.py

mv graphs_core $1/graphs_core
rm -fr dat dat_tmp

echo "Done!"
