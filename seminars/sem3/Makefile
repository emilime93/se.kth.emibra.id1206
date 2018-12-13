CC = gcc

# Executes all tests
all: #vanilla cond locks
	$(info Run one of: vanilla, cond, lock)

# Benchmark
bench: bench.o
	./$<
bench.o: bench.c
	$(CC) $< -o $@ green.o -lpthread

# Vanilla test
vanilla: vanilla.o
	./$<
vanilla.o: vanilla.c green.o green.h
	$(CC) $< -o $@ green.o -g

# Conditional test
cond: cond.o
	./$<
cond.o: cond.c green.o green.h
	$(CC) $< -o $@ green.o -g

# Lock Test
locks: locks.o
	./$<
locks.o: locks.c green.o green.h
	$(CC) $< -o $@ green.o -g

green.o: green.c
	$(CC) $< -c -g

clean:
	rm -f *.o