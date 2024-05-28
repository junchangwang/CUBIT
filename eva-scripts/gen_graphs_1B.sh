#!/bin/bash

if [ -n "$1" ]; then
	echo "To process directory $1"
else
	echo "Must specify the directory containing experimental results."
	exit
fi

rm -fr dat
rm -fr dat_tmp_1B
rm -fr graphs_1B
rm -fr $1/graphs_1B

cp -r $1/dat $1/dat_tmp_1B ./

cd gnuplot-scripts
make make_dir_1B
make figure_multiple_1B
# make figure_multiple_ub+cubit-lk+cubit-lf_1B

cd ../
python3 eva-scripts/cdf_1B.py > graphs_1B/cdf_output.txt

python3 eva-scripts/figure1.py

mv graphs_1B $1
rm -fr dat dat_tmp_1B

echo "Done!"
