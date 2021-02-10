#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#define NOT_DEBUG
#ifdef DEBUG
#define debug(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#else
#define debug(fmt, ...)
#endif

#define ENTRIES 4096

#define TRUE 1
#define FALSE 0

void parse_input(int argc, char** argv, char* filename, char* compress, char* decompress);
void read_file(char* filename, unsigned char** data, long int *filesize);
void write_file(char* filename, unsigned char* data, long int filesize);
void lzw_a(unsigned char* data, unsigned char **archive, long int filesize, long int *archivesize);
void lzw_x(unsigned char** data, unsigned char* archive, long int *filesize, long int archivesize);
void print_stats(char* infile, char* outfile, long int filesize, long int archivesize);
void compress(char* file);
void decompress(char* file);

int main (int argc, char** argv)
{
	char comp = 0, decomp = 0;
	char file[80];

	parse_input(argc,argv,file,&comp,&decomp);

	if (comp == 1) compress(file);
	else if (decomp == 1) decompress(file);

	return 0;
}

void lzw_a(unsigned char* data, unsigned char **archive, long int filesize, long int *archivesize)
{

	return;
}

void lzw_x(unsigned char** data, unsigned char* archive, long int *filesize, long int archivesize)
{

	return;
}

void decompress(char* file)
{
	unsigned char* data, *archive;
	long int filesize, archivesize;
	int len;
	char outfile[80];

	read_file(file, &archive, &archivesize);

	// compress with RLE
	printf("Decompressing file... %s\n", file);

	lzw_x(&data, archive, &filesize, archivesize);

	// append file with *.a file extension
	strcpy(outfile,file);
	if (strstr(file, ".a") != NULL)
	{
		len = strlen(outfile);
		strcpy(&outfile[len - 2],&outfile[2 + len - 2]); //get rid of archive extension
	}

	write_file(outfile, data, filesize);

	free(archive);
	free(data);

	print_stats(file, outfile, filesize, archivesize);

	return;
}

void compress(char* file)
{
	unsigned char* data, *archive;
	long int filesize, archivesize;
	char outfile[80];

	read_file(file, &data, &filesize);

	// compress with RLE
	printf("Compressing file... %s\n", file);

	lzw_a(data, &archive, filesize, &archivesize);

	// append file with *.a file extension
	strcpy(outfile,file);
	strcat(outfile,".a");

	write_file(outfile, archive, archivesize);

	free(data);
	free(archive);

	print_stats(file, outfile, filesize, archivesize);

	return;
}

void print_stats(char* infile, char* outfile, long int filesize, long int archivesize)
{
	printf("\nOriginal file: \t\t%s\n", infile);
	printf("Compressed file: \t%s\n", outfile);
	printf("Original size: \t\t%ld\n", filesize);
	printf("Compressed size: \t%ld\n", archivesize);
	printf("Compression ratio: \t%0.4f\n", (float)filesize/archivesize);

	return;
}

void read_file(char* filename, unsigned char** data, long int *filesize)
{
	FILE* fp = NULL;

	fp = fopen(filename,"rb");
	if (fp == NULL)
	{
		printf("Unable to open file %s\n", filename);
		exit(0);
	}

	// find filesize and alloc mem
	fseek(fp, 0, SEEK_END);
	*filesize = (long int)ftell(fp);
	fseek(fp, 0, SEEK_SET);

	debug("Reading %ld bytes from %s\n", *filesize, filename);

	// read data
	*data = (unsigned char *)malloc((*filesize) * sizeof(unsigned char));
	fread(*data, sizeof(unsigned char), *filesize, fp);
	fclose(fp);

	return;
}

void write_file(char* filename, unsigned char* data, long int filesize)
{
	FILE* fp = NULL;

	fp = fopen(filename, "wb");
	if (fp == NULL)
	{
		printf("Unable to open file %s\n", filename);
		exit(0);
	}

	debug("Writing %ld bytes to %s\n", filesize, filename);

	fwrite(data, sizeof(unsigned char), filesize, fp);
	fclose(fp);

	return;
}

void parse_input(int argc, char** argv, char* filename, char* compress, char* decompress)
{
	int opt;
	if (argc <= 2)
	{
		printf("Usage: ./lzw <-x|-c> filename\n");
		exit(0);
	}

	while ((opt = getopt(argc, argv, ":ax")) != -1)
	{
		switch(opt)
		{
			case 'a':
				*compress = 1;
				break;
			case 'x':
				*decompress = 1;
				break;
		}
	}

	assert((*compress != 1 || *decompress != 1) && "Cannot compress and decompress simultaneously... duh\n");

	// determine input/output filename
	if (argc == 3) strcpy(filename, argv[2]);
	else
	{
		printf("Usage: ./lzw <-x|-c> filename\n");
		exit(0);
	}

	return;
}
