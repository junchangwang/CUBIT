#!/bin/bash

if [ -n "$1" ]; then
	echo "To process directory $1"
else
	echo "Must specify the directory containing experimental results."
	exit
fi

rm -rf dat
rm -rf dat_tmp_MT
rm -rf graphs_MT
rm -rf $1/graphs_MT

cp -r $1/dat $1/dat_tmp_MT ./

cd gnuplot-scripts
make make_dir_MT
make figure_multiple_MT

cd ../
mv graphs_MT $1
rm -rf dat dat_tmp_MT

echo "Done!"
