CC=gcc
CFLAGS=-Wall -g
LDFLAGS=

all:
	test
	lab

lab:
	$(CC) $(CFLAGS) lab2.c -o rle
test:
	$(CC) $(CFLAGS) maketest.c -o test

clean:
	rm -rf test
	rm -rf rle
	rm -rf *.bin
	rm -rf *.rle
