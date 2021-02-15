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

typedef struct table
{
	unsigned short int key;
	unsigned char* val;
	int sym_length;
}table_t;

void parse_input(int argc, char** argv, char* filename, char* compress, char* decompress);
void read_file(char* filename, unsigned char** data, long int *filesize);
void write_file(char* filename, unsigned char* data, long int filesize);
void lzw_a(unsigned char* data, unsigned char **archive, long int filesize, long int *archivesize);
void lzw_x(unsigned char** data, unsigned char* archive, long int *filesize, long int archivesize);
void print_stats(char* infile, char* outfile, long int filesize, long int archivesize);
void init_table(table_t** table, unsigned short int* tablesize);
void print_table(table_t* table,unsigned short int from, unsigned short int to, unsigned short int tablesize);
void destroy_table(table_t** table, unsigned short int tablesize);
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

char find_val(table_t table, unsigned short int length, unsigned char* value, unsigned short int val_len, unsigned short int k)
{
	for (int i = 0; i < length; i++)
	{
		for (int j = 0; j < val_len; j++)
		{
		}
	}
	return 0;
}

void array_cat(unsigned char **A, unsigned char c, int *len)
{
	(*len)++;
	*A = realloc(*A, *len * sizeof(unsigned char));
	(*A)[*len - 1] = c;
	return;
}

void init_table(table_t** table, unsigned short int* tablesize)
{
	*table = (table_t *)malloc(ENTRIES * sizeof(table_t));
	for (unsigned short int i = 0; i < ENTRIES; i++)
	{
		(*table)[i].key = i;
		(*table)[i].val = NULL;
		(*table)[i].sym_length = 0;
	}
	for (int i = 0; i < 256; i++)
	{
		(*table)[i].val = (unsigned char *)malloc(sizeof(unsigned char));
		(*table)[i].val[0] = i;
		(*table)[i].sym_length++;
		debug("Adding value %c for key %d\n",(*table)[i].val[0],(*table)[i].key);
	}
	*tablesize = 256;


	return;
}

void destroy_table(table_t** table, unsigned short int tablesize)
{
	for (unsigned short int i = 0; i < tablesize; i++)
	{
		free((*table)[i].val);
		(*table)[i].val = NULL;
	}
	free(*table);
	table = NULL;
	return;
}

void print_table(table_t* table,unsigned short int from, unsigned short int to, unsigned short int tablesize)
{
	assert(to <= tablesize && "TABLE IS NOT LARGE ENOUGH TO PRINT REQUESTED VALUES");
	for (int i = from; i < to; i++)
	{
		debug("Key: %d \tVal: ",table[i].key);
		for (int j = 0; j < table[i].sym_length; j++)
		{
			debug("%c ",table[i].val[j]);
		}
		debug("%c",'\n');
	}
	return;
}

void lzw_a(unsigned char* data, unsigned char **archive, long int filesize, long int *archivesize)
{
	table_t* table = NULL;
	unsigned short int tablesize = 0;
	unsigned short int keyval;
	unsigned char *p, c;

	*archive = (unsigned char *)malloc(filesize * 2 * sizeof(unsigned char));

	init_table(&table, &tablesize);
	print_table(table, 255, 300, tablesize);

	p = data[0];
	for (int i = 0; i < filesize; i++)
	{
		c = data[i + 1];
		if (find_val(table,&keyval))
		{
		}
		else
		{
		}

	}

	destroy_table(&table, tablesize);


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

//	write_file(outfile, archive, archivesize);

	free(data);
	free(archive);

//	print_stats(file, outfile, filesize, archivesize);

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
