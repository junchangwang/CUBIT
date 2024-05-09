# Needs:  dat/getvalue.dat

reset
set term pdf font ",12"

set output "../graphs/Figure21.pdf"

set ylabel "Time ({/Symbol m}s)" offset 0.8,0,0
set xlabel "Number of threads" 

set yrange [0:]

set boxwidth 0.5 absolute

plot  "../dat/getvalue.dat" using ($2/10000):xtic(1) title "Get Value" with boxes fs solid 1.0 border rgb "black" lc rgb "grey"
