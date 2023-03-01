#Needs: dat/mem_vm.dat dat/mem_ub.dat dat/mem_fp.dat dat/mem_eb.dat
#Figure 25

reset
set term pdf font ",12"
set output "../graphs/Figure25.pdf"

set key font ",18" noinvert top left

set style data histograms
set style histogram rowstacked
set boxwidth 0.72 relative
set style fill solid 1.0 border -1

set ylabel "Space Overhead (MB)" offset 0.5,0,0

set xtics font ",10"

set yrange [0:3500]

set origin 0,0.05
set size 1.0,0.95

set bmargin 2

set xtics nomirror rotate by -20 scale 0 offset -0.5,0,0

set label "Compressed" at screen 0.26, screen 0.03
set label "Uncompressed" at screen 0.64, screen 0.03

plot '../dat/mem_vm.dat' every ::::8 using ($1/1e6):xtic(2) t "Value Bitmaps" fs solid 1.0 border rgb "black" lc rgb "grey", \
     '../dat/mem_ub.dat' every ::::8 using ($1/1e6):xtic(2) t "Update Bitmaps" fs solid 1.0 border rgb "black" lc rgb "white", \
     '../dat/mem_fp.dat' every ::::8 using ($1/1e6):xtic(2) t "Fence Pointers"  fs solid 1.0 border rgb "black" lc rgb "black", \
     '../dat/mem_eb.dat' every ::::8 using ($1/1e6):xtic(2) t "Existence Bitmap"  fill pattern 2 border rgb "black" lc rgb "black"
