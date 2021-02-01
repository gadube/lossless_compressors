#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

void parse_input(int argc, char** argv, char* filename, char* compress, char* decompress);

int main (int argc, char** argv)
{
	char comp, decomp;
	char file[80];
	int i;
	FILE* fp;
	parse_input(argc,argv,file,&comp,&decomp);
	printf("Compression = %s\n", (comp) ? "true" : "false");
	printf("Decompression = %s\n", (decomp) ? "true" : "false");

	return 0;
}

void parse_input(int argc, char** argv, char* filename, char* compress, char* decompress)
{
	int opt;
	while ((opt = getopt(argc, argv, ":ax")) != -1)
	{
		switch(opt)
		{
			case 'a':
				//printf("Performing RLE Compression on %s\n", &optarg);
				//strcpy(filename, optarg);
				*compress = 1;
				break;
			case 'x':
				//printf("Performing RLE Decompression on %s\n", &optarg);
				//strcpy(filename, optarg);
				*decompress = 1;
				break;
		}
	}

	assert(*compress != 1 || *decompress != 1);
	return;
}
