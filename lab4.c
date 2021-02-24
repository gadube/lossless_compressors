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

#define TRUE 1
#define FALSE 0

typedef struct node
{
	int frequency;
	unsigned char c;
} node_t;

int find_frequency(unsigned char* data, long int filesize, unsigned int** freq);
void print_frequency(unsigned int *frequency_list);
void print_frequency_nodes(node_t *frequency_list, int unique);

void parse_input(int argc, char** argv, char* filename, char* compress, char* decompress);
void read_file(char* filename, unsigned char** data, long int *filesize);
void write_file(char* filename, unsigned char* data, long int filesize);
void huffman_a(unsigned char* data, unsigned char **archive, long int filesize, long int *archivesize);
void huffman_x(unsigned char** data, unsigned char* archive, long int *filesize, long int archivesize);
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

void decompress(char* file)
{
	unsigned char* data, *archive;
	long int filesize, archivesize;
	int len;
	char outfile[80];

	read_file(file, &archive, &archivesize);

	// compress with RLE
	printf("Decompressing file... %s\n", file);

	huffman_x(&data, archive, &filesize, archivesize);

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

	huffman_a(data, &archive, filesize, &archivesize);

	// append file with *.a file extension
	strcpy(outfile,file);
	strcat(outfile,".a");

//	write_file(outfile, archive, archivesize);

//	free(data);
//	free(archive);

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
		printf("Usage: ./rle <-x|-c> filename\n");
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
		printf("Usage: ./rle <-x|-c> filename\n");
		exit(0);
	}

	return;
}

void print_frequency(unsigned int *frequency_list)
{
	debug("Char:\tFrequency:%c",'\n');
	for (int i = 0; i < 256; i++)
	{
		if (frequency_list[i] == 0) continue;
		debug("%u\t%u\n", i, frequency_list[i]);
	}
}

void print_frequency_nodes(node_t *frequency_list, int unique)
{
	debug("Char:\tFrequency:%c",'\n');
	for (int i = 0; i < unique; i++)
	{
		debug("%u\t%u\n", frequency_list[i].c, frequency_list[i].frequency);
	}
}

int find_frequency(unsigned char* data, long int filesize, unsigned int** freq)
{
	int nonzeros = 0;
	// calculate frequencies of each char
	for (int i = 0; i < filesize; i++)
	{
		(*freq)[data[i]]++;
	}

#ifdef DEBUG
	print_frequency(*freq);
#endif

	// find nonzero frequencies
	for (int i = 0; i < 256; i++)
	{
		if ((*freq)[i] == 0) continue;
		nonzeros++;
	}

	return nonzeros;
}

node_t create_node(unsigned char ch, unsigned int freq)
{
	node_t N;
	N.c = ch;
	N.frequency = freq;

	return N;
}

void combine(node_t **array, int left, int middle, int right)
{
	int leftsize = middle - left  + 1;
	int rightsize = right - middle;
	int i, j, k;

	node_t Left[leftsize];
	node_t Right[rightsize];

	for (i = 0; i < leftsize; i++)
	{
		Left[i] = (*array)[left + i];
	}
	for (i = 0; i < rightsize; i++)
	{
		Right[i] = (*array)[middle + 1 + i];
	}

	for (i = 0, j = 0, k = left; i < leftsize && j < rightsize; k++)
	{
		if (Left[i].frequency <= Right[j].frequency)
		{
			(*array)[k] = Left[i];
			i++;
		}
		else
		{
			(*array)[k] = Right[j];
			j++;
		}
	}

	for (; i < leftsize; i++, k++)
	{
		(*array)[k] = Left[i];
	}

	for (; j < rightsize; j++, k++)
	{
		(*array)[k] = Right[j];
	}

	return;
}

void merge_sort(node_t **array, int left, int right)
{
	if (left < right)
	{
		int center = (left + right) / 2;

		merge_sort(array, left, center);
		merge_sort(array, center + 1, right);

		combine(array, left, center, right);
	}

	return;
}

void sort(node_t** array, int num_nodes)
{
	// get min item to top of heap
	print_frequency_nodes(*array, num_nodes);

	merge_sort(array, 0, num_nodes - 1);
	print_frequency_nodes(*array, num_nodes);
	return;
}

node_t* create_pq(unsigned int* freq, int num_nodes)
{
	int i, j = 0;
	node_t* array = (node_t *)malloc(sizeof(node_t) * num_nodes);

	for (i = 0; i < 256; i++)
	{
		if (freq[i] == 0) continue;
		array[j] = create_node(i, freq[i]);
		j++;
	}

	sort(&array, num_nodes);

	return array;
}

node_t* build_huffman_tree(unsigned char* data, long int filesize)
{
	unsigned int *frequencies = (unsigned int *)calloc(sizeof(unsigned int), 256);
	int unique = 0;

	unique = find_frequency(data, filesize, &frequencies);
	node_t *A = create_pq(frequencies, unique);

	free(frequencies);
	return A;
}

void huffman_a(unsigned char* data, unsigned char **archive, long int filesize, long int *archivesize)
{
	node_t* huff_tree;

	huff_tree = build_huffman_tree(data, filesize);

	return;
}

void huffman_x(unsigned char** data, unsigned char* archive, long int *filesize, long int archivesize)
{
	return;
}
