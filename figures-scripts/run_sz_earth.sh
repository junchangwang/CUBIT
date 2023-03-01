rm -r ../dat
cp -r dat ../
cd ../gnuplot-scripts
make make_dir_earth
make figure_multiple_sz_earth
make figure_multiple_sz_ub+nbub-lk+nbub-lf_earth
# make figure_multiple_sz_ub+nbub-lk+nbub-lf_naive_earth