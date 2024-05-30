#!/bin/bash

if [ -n "$1" ]; then
	echo "To process directory $1"
else
	echo "Must specify the directory containing experimental results."
	exit
fi

rm -rf dat
rm -rf dat_tmp_Seg
rm -rf graphs_Seg
rm -rf $1/graphs_Seg

cp -r $1/dat $1/dat_tmp_Seg ./

cd gnuplot-scripts
make make_dir_Seg
make figure_multiple_Seg

cd ../
mv graphs_Seg $1
rm -rf dat dat_tmp_Seg

echo "Done!"
