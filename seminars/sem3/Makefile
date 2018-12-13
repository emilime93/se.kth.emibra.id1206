CC = gcc

# Executes all tests
all: vanilla cond locks

# Vanilla test
vanilla: vanilla.o
	./$<
vanilla.o: vanilla.c green.o green.h
	$(CC) vanilla.c -o $@ green.o -g

# Conditional test
cond: cond.o
	./$<
cond.o: cond.c green.o green.h
	$(CC) cond.c -o $@ green.o -g

# Lock Test
locks: locks.o
	./$<
locks.o: locks.c green.o green.h
	$(CC) locks.c -o $@ green.o -g

green.o: green.c
	$(CC) $< -c -g

clean:
	rm -f *.o