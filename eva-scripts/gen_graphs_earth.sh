#!/bin/bash

if [ -n "$1" ]; then
	echo "To process directory $1"
else
	echo "Must specify the directory containing experimental results."
	exit
fi

rm -rf dat
rm -rf dat_tmp_earth
rm -rf graphs_earth
rm -rf $1/graphs_earth

cp -r $1/dat $1/dat_tmp_earth ./

cd gnuplot-scripts
make make_dir_earth
make figure_multiple_earth
# make figure_multiple_ub+cubit-lk+cubit-lf_earth

cd ../
python3 eva-scripts/cdf_earth.py > graphs_earth/cdf_output.txt

mv graphs_earth $1
rm -rf dat dat_tmp_earth

echo "Done!"
