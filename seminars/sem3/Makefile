CC = gcc

all: test.o
	./$<

test.o: test.c green.o green.h
	$(CC) test.c -o $@ green.o

green.o: green.c
	$(CC) $< -c

clean:
	rm -f *.o