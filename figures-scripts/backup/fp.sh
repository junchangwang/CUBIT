#IDXPATH=/home/yan/codes/nicolas/100M_100/
IDXPATH="$PWD/100M_100/"

./build/nicolas -a ub -m update --number-of-queries 10000 -r 10000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-pointer no > ub_fp_no
./build/nicolas -a ub -m update --number-of-queries 10000 -r 10000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-length 0 --fence-pointer yes > ub_fp_0
./build/nicolas -a ub -m update --number-of-queries 10000 -r 10000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-length 10 --fence-pointer yes > ub_fp_10
./build/nicolas -a ub -m update --number-of-queries 10000 -r 10000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-length 100  --fence-pointer yes > ub_fp_100
./build/nicolas -a ub -m update --number-of-queries 10000 -r 10000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-length 1000 --show-memory yes --fence-pointer yes > ub_fp_1000
./build/nicolas -a ub -m update --number-of-queries 10000 -r 10000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-length 10000 --show-memory yes --fence-pointer yes > ub_fp_10000
./build/nicolas -a ub -m update --number-of-queries 10000 -r 10000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-length 100000 --show-memory yes --fence-pointer yes > ub_fp_100000
./build/nicolas -a ub -m update --number-of-queries 10000 -r 10000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-length 1000000 --show-memory yes --fence-pointer yes > ub_fp_1000000
./build/nicolas -a ub -m update --number-of-queries 10000 -r 10000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-length 10000000 --show-memory yes --fence-pointer yes > ub_fp_10000000

./build/nicolas -a ucb -m update --number-of-queries 10000 -r 10000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-pointer no > ucb_fp_no
./build/nicolas -a ucb -m update --number-of-queries 10000 -r 10000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-length 0 --fence-pointer yes > ucb_fp_0
./build/nicolas -a ucb -m update --number-of-queries 10000 -r 10000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-length 10 --fence-pointer yes > ucb_fp_10
./build/nicolas -a ucb -m update --number-of-queries 10000 -r 10000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-length 100  --fence-pointer yes > ucb_fp_100
./build/nicolas -a ucb -m update --number-of-queries 10000 -r 10000 -c 100 -i $IDXPATH -n 100000000 -v yes --show-memory no --fence-pointer no > ucb_fp_no


./build/nicolas -a ucb -m update --number-of-queries 10000 -r 10000 -c 100 -i $IDXPATH -n 100000000 -v yes --show-memory no --fence-pointer yes --fence-length 1000000000 > ucb_fp_no
