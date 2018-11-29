#Make script for my own test in linux
gcc -c buddy.c &&
gcc -o test.out buddy.o test.c -lm
&& ./test.out