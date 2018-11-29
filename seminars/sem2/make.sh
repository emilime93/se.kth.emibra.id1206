# Make script for my own test on macOS
gcc -c buddy.c &&
gcc -o test.out buddy.o test.c &&
./test.out