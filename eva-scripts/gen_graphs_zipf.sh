#!/bin/bash

if [ -n "$1" ]; then
	echo "To process directory $1"
else
	echo "Must specify the directory containing experimental results."
	exit
fi

rm -fr dat
rm -fr dat_tmp_zipf
rm -fr graphs_zipf
rm -fr $1/graphs_zipf

cp -r $1/dat_zipf/ ./dat
cp -r $1/dat_tmp_zipf ./

cd gnuplot-scripts
make make_dir_zipf
make figure_multiple_zipf
# make figure_multiple_ub+cubit-lk+cubit-lf_zipf

cd ../
python3 eva-scripts/cdf_zipf.py > graphs_zipf/cdf_output.txt

mv graphs_zipf $1
rm -fr dat dat_tmp_zipf

echo "Done!"
