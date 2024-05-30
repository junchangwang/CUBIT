#!/bin/bash

if [ -n "$1" ]; then
	echo "To process directory $1"
else
	echo "Must specify the directory containing experimental results."
	exit
fi

rm -rf dat
rm -rf dat_tmp_Para
rm -rf graphs_Para
rm -rf $1/graphs_Para

cp -r $1/dat $1/dat_tmp_Para ./

cd gnuplot-scripts
make make_dir_Para
make figure_multiple_Para

cd ../
mv graphs_Para $1
rm -rf dat dat_tmp_Para

echo "Done!"
