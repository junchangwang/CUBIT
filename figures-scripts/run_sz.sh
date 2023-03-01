rm -r ../dat
cp -r dat ../
cd ../gnuplot-scripts
make make_dir
make figure_multiple_sz
make figure_multiple_sz_ub+nbub-lk+nbub-lf
make figure_multiple_sz_ub+nbub-lk+nbub-lf_naive