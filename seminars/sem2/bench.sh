gcc -c buddy.c &&
gcc -c rand.c &&
gcc -o bench buddy.o rand.o bench.c &&
./bench