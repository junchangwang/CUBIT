#!/bin/bash

if [ -n "$1" ]; then
	echo "To process directory $1"
else
	echo "Must specify the directory containing experimental results."
	exit
fi

rm -rf dat
rm -rf dat_tmp_WPMT
rm -rf graphs_WPMT
rm -rf $1/graphs_WPMT

cp -r $1/dat $1/dat_tmp_WPMT ./

cd gnuplot-scripts
make make_dir_WPMT
make figure_multiple_WPMT

cd ../
mv graphs_WPMT $1
rm -rf dat dat_tmp_WPMT

echo "Done!"
