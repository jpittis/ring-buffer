CC = gcc
CFLAGS = -c -Wall -std=c11 -g -pthread
default: test

test: test.o ring_buffer.o
	$(CC) $(CFLAGS) test.o ring_buffer.o -o test

run: test
	./test

clean:
	-rm test.o
	-rm ring_buffer.o
	-rm test

