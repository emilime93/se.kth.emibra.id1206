CC = gcc

all: test.o
	./$<

test.o: test.c green.o green.h
	$(CC) test.c -o $@ green.o -g

green.o: green.c
	$(CC) $< -c -g

clean:
	rm -f *.o