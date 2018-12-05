# Gnuplot script for plotting data in file "both.dat"

set terminal png
set output "both.png"

set title "Page replacement using both policy"

set key right center

set xlabel "frames in memory"
set ylabel "hit ratio"

set xrange [0:100]
set yrange [0:1]

plot "random.dat" u 1:2 w linespoints title "Random", \
     "optimal.dat" u 1:2 w linespoints title "Optimal", \
     "lru.dat" u 1:2 w linespoints title "LRU"