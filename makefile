default: test

test.o: test.c
	gcc -c test.c -o test.o

ring_buffer.o: ring_buffer.c ring_buffer.h
	gcc -c ring_buffer.c -o ring_buffer.o

test: test.o ring_buffer.o
	gcc -pthread test.o ring_buffer.o -o test

run: test
	./test

clean:
	-rm test.o
	-rm ring_buffer.o
	-rm test

