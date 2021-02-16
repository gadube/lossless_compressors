#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char** argv)
{
	char outfile[80];
	FILE* fp;
	int n, i;
	unsigned char c;

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
	c = 'B';
	n = 1;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'A';
	n = 1;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'B';
	n = 1;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'A';
	n = 2;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'B';
	n = 1;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'A';
	n = 3;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'B';
	n = 1;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'A';
	n = 2;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'B';
	n = 1;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'A';
	n = 2;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'B';
	n = 1;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'A';
	n = 2;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'B';
	n = 1;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'A';
	n = 2;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'B';
	n = 1;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'A';
	n = 2;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'B';
	n = 1;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'A';
	n = 2;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'B';
	n = 1;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'A';
	n = 2;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'B';
	n = 1;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'A';
	n = 2;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'B';
	n = 1;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'A';
	n = 2;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'B';
	n = 1;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'A';
	n = 2;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'B';
	n = 1;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'A';
	n = 2;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'B';
	n = 1;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'A';
	n = 2;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'B';
	n = 1;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = 'A';
	n = 2;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
/*
	c = 'A';
	n = 256;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	c = '\n';
	n = 2;
	for (i = 0; i < n; i++)
	{
		fwrite(&c,sizeof(char),1,fp);
	}
	char w[20] = "This is a text file.";
	n = 5;
	for (i = 0; i < n; i++)
	{
		fwrite(&w,sizeof(char),20,fp);
	}
	*/

	fclose(fp);
	return 0;
}
