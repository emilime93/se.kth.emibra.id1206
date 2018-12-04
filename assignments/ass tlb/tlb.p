set terminal png
set output "tlb.png"

set title "TLB benchmark, 4 KiByte pages, 10 Gi operations"

# opsitionering av line-label
set key left top

set xlabel "number of pages"
set ylabel "time in s"

# use log scale if doubling
set logscale x 2

#set yrange [0.002:0.005]

plot "tlb64.dat" u 1:2 w linespoints title "pagesize 64", \
     "tlb256.dat" u 1:2 w linespoints title "pagesize 256", \
     "tlb512.dat" u 1:2 w linespoints title "pagesize 512", \
     "tlb1024.dat" u 1:2 w linespoints title "pageize 1024"
#line linepoints points
# u 1:2 betyder första kolumnen = x, andra columnen = y