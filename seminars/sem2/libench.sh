gcc -c buddy.c &&
gcc -c rand.c &&
gcc -o bench rand.o buddy.o bench.c -lm &&
./bench