CC=gcc
CFLAGS=-Wall -g -DDEBUG
LDFLAGS=

all:
	make lab
	make test

lab:
	$(CC) $(CFLAGS) lab2.c -o rle
test:
	$(CC) $(CFLAGS) maketest.c -o test

clean:
	rm -rf test
	rm -rf rle
	rm -rf *.bin
	rm -rf *.a
