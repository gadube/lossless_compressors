CC=gcc
CFLAGS=-Wall -g #-DDEBUG
LDFLAGS=

all:
	make rle
	make lzw
	make huffman
	make test

rle:
	$(CC) $(CFLAGS) rle.c -o rle
lzw:
	$(CC) $(CFLAGS) lzw.c -o lzw
huffman:
	$(CC) $(CFLAGS) huffman.c -o huffman
test:
	$(CC) $(CFLAGS) maketest.c -o test

clean:
	rm -rf test
	rm -rf rle
	rm -rf lzw
	rm -rf huffman
	rm -rf *.bin
	rm -rf *.a
	rm -rf *.u
