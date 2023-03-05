#IDXPATH=/home/yan/codes/nicolas/100M_100/
IDXPATH="$PWD/100M_100/"

./build/nicolas -a ub -m update --number-of-queries 160000 -r 40000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-length 10000 --fence-pointer yes --merge 1 > ub_merge_1
./build/nicolas -a ub -m update --number-of-queries 160000 -r 40000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-length 10000 --fence-pointer yes --merge 5 > ub_merge_5
./build/nicolas -a ub -m update --number-of-queries 160000 -r 40000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-length 10000 --fence-pointer yes --merge 10 > ub_merge_10
./build/nicolas -a ub -m update --number-of-queries 160000 -r 40000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-length 10000 --fence-pointer yes --merge 20 > ub_merge_20
./build/nicolas -a ub -m update --number-of-queries 160000 -r 40000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-length 10000 --show-memory yes --fence-pointer yes --merge 50 > ub_merge_50
./build/nicolas -a ub -m update --number-of-queries 160000 -r 40000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-length 10000  --fence-pointer yes --merge 100 > ub_merge_100
./build/nicolas -a ub -m update --number-of-queries 160000 -r 40000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-length 10000 --show-memory yes --fence-pointer yes --merge 200 > ub_merge_200
./build/nicolas -a ub -m update --number-of-queries 160000 -r 40000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-length 10000 --show-memory yes --fence-pointer yes --merge 500 > ub_merge_500
./build/nicolas -a ub -m update --number-of-queries 160000 -r 40000 -c 100 -i $IDXPATH -n 100000000 -v yes --fence-length 10000 --show-memory yes --fence-pointer yes --merge 1000 > ub_merge_1000
