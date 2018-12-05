# A template Makefile to compile a report from latex and gnuplot.

# These are the tools that we will use, not strictly needed but if we
# want to change something it is easer to do it here.

GNUPLOT = gnuplot
CC = gcc

# The default thing that we should make if nothing else specified.

all: both.png
	open both.png

# Specific dependencies, we know that the report depends on the plot
# and the source since both of those are included in the report. 

random.o: random.c
	$(CC) -o $@ $<

optimal.o: optimal.c
	$(CC) -o $@ $<

lru.o: lru.c
	$(CC) -o $@ $<

# The data depends on the executable benchmark and this is how we generate it.

optimal.dat: optimal.o
	./optimal.o 100000 100 > $@
random.dat: random.o 
	./random.o 100000 100 > $@
lru.dat: lru.o
	./lru.o 100000 100 > $@


both.png: random.dat optimal.dat lru.dat
	$(GNUPLOT) both.p

############### Specific for random and optimal
random: random.png
	open random.png
optimal: optimal.png
	open optimal.png
lru: lru.png
	open lru.png

random.png: random.dat
	$(GNUPLOT) random.p

optimal.png: optimal.dat
	$(GNUPLOT) optimal.p

lru.png: lru.dat
	$(GNUPLOT) lru.p

# remove also the targets that we have generated
clean:
	rm -f *.dat *.o