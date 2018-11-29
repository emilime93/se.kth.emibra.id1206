# Make script for the extended tests on macOS
gcc -c buddy.c &&
gcc -c rand.c &&
gcc -o bench.out buddy.o rand.o bench.c &&
./bench.out `echo $@`