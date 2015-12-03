CC = gcc
CFLAGS = -Wall -g 
default: test

test: test.o ring_buffer.o
	$(CC) $(CFLAGS) test.o ring_buffer.o -o test -pthread


run: test
	./test

clean:
	-rm test.o
	-rm ring_buffer.o
	-rm test

