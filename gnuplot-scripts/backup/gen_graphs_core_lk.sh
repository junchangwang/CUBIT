#!/bin/bash

if [ -n "$1" ]; then
	echo "To process directory $1"
else
	echo "Must specify the directory containing experimental results."
	exit
fi

rm -rf dat
rm -rf dat_tmp
rm -rf graphs_lk
rm -rf $1/graphs_lk

cp -r $1/dat $1/dat_tmp ./

cd gnuplot-scripts
make make_dir_lk
make figure_multiple_lk
# make figure_multiple_ub+cubit-lk+cubit-lf

cd ../
python3 eva-scripts/cdf_core.py > graphs_lk/cdf_output.txt

python3 eva-scripts/figure1.py

mv graphs_lk $1
rm -rf dat dat_tmp

echo "Done!"
