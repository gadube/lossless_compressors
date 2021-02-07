#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char** argv)
{
	char outfile[80];
	FILE* fp;
	int c, n, i;

	// check input args and give output file a name
	if (argc > 2)
	{
		printf("Usage: ./maketest filename\n");
		exit(0);
	}
	else if (argc == 1)
	{
		strcpy(outfile,"input.bin");
	}
	else
	{
		strcpy(outfile, argv[1]);
	}

	fp = fopen(outfile,"wb");

	c = 'A';
	n = 6;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}

	fclose(fp);
	return 0;
}