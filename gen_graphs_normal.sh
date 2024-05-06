#!/bin/bash

if [ -n "$1" ]; then
	echo "To process directory $1"
else
	echo "Must specify the directory containing experimental results."
	exit
fi

rm -fr dat
rm -fr dat_tmp
rm -fr graphs
rm -fr $1/graphs

cp -r $1/dat $1/dat_tmp ./

cd gnuplot-scripts
make make_dir
make figure_multiple_sz
# make figure_multiple_sz_ub+cubit-lk+cubit-lf

cd ../
python3 cdf_sz.py > graphs/cdf_output.txt

python3 figure1.py

mv graphs $1
rm -fr dat dat_tmp

echo "Done!"
