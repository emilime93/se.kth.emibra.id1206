#Make script the extended test in linux
gcc -c -g buddy.c &&
gcc -c -g rand.c &&
gcc -o bench.out rand.o buddy.o bench.c -g -lm &&
./bench.out