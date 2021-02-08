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

#define WINDOW 4
#define MARKER 0

#define TRUE 1
#define FALSE 0

void parse_input(int argc, char** argv, char* filename, char* compress, char* decompress);
void read_file(char* filename, unsigned char** data, long int *filesize);
void write_file(char* filename, unsigned char* data, long int filesize);
void rle_a(unsigned char* data, unsigned char **archive, long int filesize, long int *archivesize);
void rle_markers_a(unsigned char* data, unsigned char **archive, long int filesize, long int *archivesize);
void rle_x(unsigned char** data, unsigned char* archive, long int *filesize, long int archivesize);
void rle_markers_x(unsigned char** data, unsigned char* archive, long int *filesize, long int archivesize);
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

void rle_markers_a(unsigned char* data, unsigned char **archive, long int filesize, long int *archivesize)
{
	int i = 0, j = 0, k = 0;
	int diffrun = 0;
	unsigned char A_FLAG = 0;
	unsigned char count = 0;
	unsigned char curr, next;

	*archive = (unsigned char *)malloc(sizeof(unsigned char) * 2 * filesize);

	for (i = 0, j = 0; i < filesize; i++, j += 2)
	{
		count = 1;

		// set current and next values
		curr = data[i];
		next = data[i + 1];

		k = i;
		diffrun = 0;
		while (data[k] != data[k + 1])
		{
			k++;
			diffrun++;
		}

		// if run of single values is larger than window, turn off compression
		if (diffrun >= WINDOW) A_FLAG = FALSE;
		else A_FLAG = TRUE;

		if (A_FLAG)
		{

			// compare current value to next for match, check for overflow
			while (count < 255 && curr == next)
			{
				i++;
				count++;

				curr = data[i];
				next = data[i + 1];
			}

			// once string of same values ends, write to archive
			(*archive)[j] = count;
			(*archive)[j + 1] = data[i];
			debug("%d%c",(*archive)[j],(*archive)[j+1]);
		}
		else
		{
			(*archive)[j] = MARKER;
			(*archive)[j + 1] = 'x';
			for (k = j + 2; k <= diffrun + j + 1; k++)
			{
				if (data[i] == 0)
				{
					(*archive)[k] = MARKER;
					(*archive)[k + 1] = 1;
					(*archive)[k + 2] = data[i];
					j += 2;
					k += 2;
					i++;
					A_FLAG = FALSE;
					break;
				}
				else
				{
					(*archive)[k] = data[i];
					i++;
				}
			}
			j = k;
			A_FLAG = FALSE;
			(*archive)[j] = MARKER;
			if ( j % 2 != 0)
			{
				(*archive)[j+1] = 'x';
				j++;
			}
			j--;
			i--;
		}
	}
	*archivesize = j * sizeof(unsigned char);
	debug("\nArchive Size: %ld\n",*archivesize);
}

void rle_markers_x(unsigned char** data, unsigned char* archive, long int *filesize, long int archivesize)
{
	int i, j;
	unsigned char count = 0;
	unsigned char val;


	// determine filesize of decompressed values
	*filesize = 0;
	for (i = 0; i < archivesize; i += 2)
	{
		if (archive[i] != 0) *filesize += archive[i];
		else
		{
			i += 2;
			while (archive[i] != 0)
			{
				(*filesize)++;
				i++;
			}
			if (i % 2 == 0) i--;
		}
	}

	*data = (unsigned char *)malloc((*filesize) * sizeof(unsigned char));

	debug("Archive size: %ld\n",archivesize);
	debug("File size: %ld\n",*filesize);

	// decompress file
	for (i = 0,j = 0; i < archivesize - 1; i += 2)
	{
		count = archive[i];
		val = archive[i+1];
		if (count == 0)
		{
			i += 2;
			while (archive[i] != 0)
			{
				(*data)[j] = archive[i];
				i++;
				j++;
			}
			if (i % 2 == 0) i--;
			continue;
		}
		while (count > 0)
		{
			(*data)[j] = val;
			j++;
			count--;
		}
	}

	return;
}

void rle_a(unsigned char* data, unsigned char **archive, long int filesize, long int *archivesize)
{
	int i = 0, j = 0;
	unsigned char count = 0;
	unsigned char curr, next;

	*archive = (unsigned char *)malloc(sizeof(unsigned char) * filesize);

	for (i = 0, j = 0; i < filesize; i++, j += 2)
	{
		count = 1;

		// set current and next values
		curr = data[i];
		next = data[i + 1];

		// compare current value to next for match, check for overflow
		while (count < 255 && curr == next)
		{
			i++;
			count++;

			curr = data[i];
			next = data[i + 1];
		}

		// once string of same values ends, write to archive
		(*archive)[j] = count;
		(*archive)[j + 1] = data[i];
		debug("%d%c",(*archive)[j],(*archive)[j+1]);
	}
	*archivesize = j * sizeof(unsigned char);
	debug("\nArchive Size: %ld\n",*archivesize);
}

void rle_x(unsigned char** data, unsigned char* archive, long int *filesize, long int archivesize)
{
	int i, j;
	unsigned char count = 0;
	unsigned char val;


	// determine filesize of decompressed values
	*filesize = 0;
	for (i = 0; i < archivesize; i++)
	{
		if ((i % 2) == 0) *filesize += archive[i];
	}

	*data = (unsigned char *)malloc((*filesize) * sizeof(unsigned char));

	debug("Archive size: %ld\n",archivesize);
	debug("File size: %ld\n",*filesize);

	for (i = 0,j = 0; i < archivesize - 1; i += 2)
	{
		count = archive[i];
		val = archive[i+1];
		while (count > 0)
		{
			(*data)[j] = val;
			j++;
			count--;
		}
	}

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

#ifdef MARKERS
	rle_markers_x(&data, archive, &filesize, archivesize);
#else
	rle_markers_x(&data, archive, &filesize, archivesize);
#endif

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

#ifdef MARKERS
	rle_markers_a(data, &archive, filesize, &archivesize);
#else
	rle_a(data, &archive, filesize, &archivesize);
#endif

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
