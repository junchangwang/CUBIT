#!/bin/bash

if [ -n "$1" ]; then
	echo "To process directory $1"
else
	echo "Must specify the directory containing experimental results."
	exit
fi

rm -fr dat
rm -fr dat_tmp
rm -fr graphs_lk
rm -fr $1/graphs_lk

cp -r $1/dat $1/dat_tmp ./

cd gnuplot-scripts
make make_dir_lk
make figure_multiple_sz_lk
# make figure_multiple_sz_ub+nbub-lk+nbub-lf

cd ../
python3 cdf_sz.py > graphs_lk/cdf_output.txt

python3 figure1.py

mv graphs_lk $1
rm -fr dat dat_tmp

echo "Done!"
