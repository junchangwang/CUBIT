rm -r ../dat
cp -r dat ../
cd ../gnuplot-scripts
make make_dir_zipf
make figure_multiple_sz_zipf
make figure_multiple_sz_ub+nbub-lk+nbub-lf_zipf
# make figure_multiple_sz_ub+nbub-lk+nbub-lf_naive_zipf