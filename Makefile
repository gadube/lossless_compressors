CC=gcc
CFLAGS=-Wall -g -DDEBUG
LDFLAGS=

all:
	make lab2
	make lab3
	make test

lab2:
	$(CC) $(CFLAGS) lab2.c -o rle
lab3:
	$(CC) $(CFLAGS) lab3.c -o lzw
test:
	$(CC) $(CFLAGS) maketest.c -o test

clean:
	rm -rf test
	rm -rf rle
	rm -rf lzw
	rm -rf *.bin
	rm -rf *.a
