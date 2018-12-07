CC = gcc

all: switch.o
	./switch.o

switch.o: switch.c
	$(CC) -o $@ $<

clean:
	rm -f *.o