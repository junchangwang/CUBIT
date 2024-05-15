#!/bin/bash

if [ -n "$1" ]; then
	echo "To process directory $1"
else
	echo "Must specify the directory containing experimental results."
	exit
fi

rm -fr dat
rm -fr dat_tmp_WPMT
rm -fr graphs_WPMT
rm -fr $1/graphs_WPMT

cp -r $1/dat $1/dat_tmp_WPMT ./

cd gnuplot-scripts
make make_dir_WPMT
make figure_multiple_WPMT

cd ../
mv graphs_WPMT $1
rm -fr dat dat_tmp_WPMT

echo "Done!"
