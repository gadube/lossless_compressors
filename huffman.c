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
	unsigned char internal;
	struct node* left;
	struct node* right;
} node_t;

typedef struct tree
{
	int size;
	node_t *root;
} tree_t;

int find_frequency(unsigned char* data, long int filesize, unsigned int** freq);
void print_frequency(unsigned int *frequency_list);
void print_frequency_nodes(node_t **frequency_list, int unique);

void parse_input(int argc, char** argv, char* filename, char* compress, char* decompress);
void read_file(char* filename, unsigned char** data, long int *filesize);
void read_huffman_file(char* filename, unsigned int** data, long int *filesize);
void write_file(char* filename, unsigned char* data, long int filesize);
void write_huffman_file(char* filename, unsigned int* data, long int filesize);
void huffman_a(unsigned char* data, unsigned int **archive, long int filesize, long int *archivesize);
void huffman_x(unsigned char** data, unsigned int* archive, long int *filesize, long int archivesize);
void print_compress_stats(char* infile, char* outfile, long int filesize, long int archivesize);
void print_decompress_stats(char* infile, char* outfile, long int filesize, long int archivesize);
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
	unsigned int* archive;
	unsigned char* data;
	long int filesize, archivesize;
	int len;
	char outfile[80];

	read_huffman_file(file, &archive, &archivesize);

	// compress with RLE
	printf("Decompressing file... %s\n", file);

	huffman_x(&data, archive, &filesize, archivesize);

	// append file with *.a file extension
	strcpy(outfile,file);
	if (strstr(file, ".a") != NULL)
	{
		len = strlen(outfile);
		//strcpy(&outfile[len - 2],&outfile[2 + len - 2]); //get rid of archive extension
		outfile[len - 1] = 'u';
	}

	write_file(outfile, data, filesize);

	free(archive);
	free(data);

	print_decompress_stats(file, outfile, filesize, archivesize);

	return;
}

void compress(char* file)
{
	unsigned char* data;
	unsigned int* archive;
	long int filesize, archivesize;
	char outfile[80];

	read_file(file, &data, &filesize);

	// compress with RLE
	printf("Compressing file... %s\n", file);

	huffman_a(data, &archive, filesize, &archivesize);

	// append file with *.a file extension
	strcpy(outfile,file);
	strcat(outfile,".a");

	write_huffman_file(outfile, archive, archivesize);

	free(data);
	free(archive);

	print_compress_stats(file, outfile, filesize, archivesize);

	return;
}
void print_decompress_stats(char* infile, char* outfile, long int filesize, long int archivesize)
{
	printf("\nOriginal file: \t\t%s\n", infile);
	printf("Decompressed file: \t%s\n", outfile);
	printf("Original size: \t\t%ld\n", filesize);
	printf("Compressed size: \t%ld\n", archivesize);
	printf("Compression ratio: \t%0.4f\n", (float)filesize/(archivesize));

	return;
}

void print_compress_stats(char* infile, char* outfile, long int filesize, long int archivesize)
{
	printf("\nOriginal file: \t\t%s\n", infile);
	printf("Compressed file: \t%s\n", outfile);
	printf("Original size: \t\t%ld\n", filesize);
	printf("Compressed size: \t%ld\n", archivesize * 4);
	printf("Compression ratio: \t%0.4f\n", (float)filesize/(archivesize * 4));

	return;
}

void read_huffman_file(char* filename, unsigned int** data, long int *filesize)
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
	*data = (unsigned int *)malloc((*filesize) * sizeof(unsigned int));
	fread(*data, sizeof(unsigned int), *filesize, fp);
	fclose(fp);

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

void write_huffman_file(char* filename, unsigned int* data, long int filesize)
{
	FILE* fp = NULL;

	fp = fopen(filename, "wb");
	if (fp == NULL)
	{
		printf("Unable to open file %s\n", filename);
		exit(0);
	}

	debug("Writing %ld bytes to %s\n", filesize * 4, filename);

	fwrite(data, sizeof(unsigned int), filesize, fp);
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

void print_frequency_nodes(node_t **frequency_list, int unique)
{
	debug("Char:\tFrequency:\tInternal:%c",'\n');
	for (int i = 0; i < unique; i++)
	{
		debug("%u\t%u\t%s\n", (frequency_list[i])->c, (frequency_list[i])->frequency, ((frequency_list[i])->internal)?"TRUE":"FALSE");
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

node_t *create_node(unsigned char ch, unsigned int freq, unsigned char internal)
{
	node_t *N = (node_t *)malloc(sizeof(node_t));
	N->c = ch;
	N->frequency = freq;
	N->internal = internal;
	N->left = NULL;
	N->right = NULL;
	debug("Creating Node with Char %u and Frequency %u\n", ch, freq);

	return N;
}

void combine(node_t **array, int left, int middle, int right)
{
	int leftsize = middle - left  + 1;
	int rightsize = right - middle;
	int i, j, k;

	node_t **Left = (node_t **)malloc(sizeof(node_t *) * leftsize);
	node_t **Right = (node_t **)malloc(sizeof(node_t *) * rightsize);

	for (i = 0; i < leftsize; i++)
	{
		Left[i] = array[left + i];
	}
	for (i = 0; i < rightsize; i++)
	{
		Right[i] = array[middle + 1 + i];
	}

	for (i = 0, j = 0, k = left; i < leftsize && j < rightsize; k++)
	{
		if (Left[i]->frequency <= Right[j]->frequency)
		{
			array[k] = Left[i];
			i++;
		}
		else
		{
			array[k] = Right[j];
			j++;
		}
	}

	for (; i < leftsize; i++, k++)
	{
		array[k] = Left[i];
	}

	for (; j < rightsize; j++, k++)
	{
		array[k] = Right[j];
	}

	// free temp arrays
	free(Left);
	free(Right);
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
	merge_sort(array, 0, num_nodes - 1);
	return;
}

node_t** create_list(unsigned int* freq, int num_nodes)
{
	int i, j = 0;
	node_t** array = (node_t **)malloc(sizeof(node_t *) * num_nodes);

	debug("Num Nodes: %d\n", num_nodes);
	for (i = 0; i < 256; i++)
	{
		if (freq[i] == 0) continue;
		array[j] = create_node(i, freq[i], FALSE);
		debug("Char: %u \t Frequency: %u \n",array[j]->c, array[j]->frequency);
		j++;
	}


	return array;
}

node_t* pop_node(node_t*** PQ, int* num_nodes)
{
	assert(num_nodes != 0 && "NUMBER OF NODES IN QUEUE CANNONT BE ZERO\n");

	node_t* N;

	N = (*PQ)[0];
	node_t **Q = (node_t **)malloc(sizeof(node_t *) * (*num_nodes - 1));

	for (int i = 1; i < *num_nodes; i++)
	{
		Q[i - 1] = (*PQ)[i];
	}

	free(*PQ);
	*PQ = Q;
	(*num_nodes)--;

	return N;
}

void push_node(node_t*** PQ, int *num_nodes, node_t* N)
{
	(*num_nodes)++;
	node_t **Q = (node_t **)malloc(sizeof(node_t *) * (*num_nodes));
	Q[0] = N;
	for (int i = 1; i < *num_nodes; i++)
	{
		Q[i] = (*PQ)[i-1];
	}
	free(*PQ);
	*PQ = Q;
	sort(*PQ, *num_nodes);

	return;
}

tree_t *init_tree(node_t* root, int nodes)
{
	tree_t *T = (tree_t *)malloc(sizeof(tree_t));
	T->root = root;
	T->size = nodes;

	return T;
}

void print2DUtil(node_t *root, int space)
{
    // Base case
    if (root == NULL)
        return;

    // Increase distance between levels
    space += 10;

    // Process right child first
    print2DUtil(root->right, space);

    // Print current node after space
    // count
    debug("%c", '\n');
    for (int i = 10; i < space; i++)
        debug("%c",' ');
    debug("%u|%u\n", root->c, root->frequency);

    // Process left child
    print2DUtil(root->left, space);
}

node_t** assemble_tree(node_t** PQ, int num_nodes)
{
	while (num_nodes > 1)
	{
		node_t* N1 = pop_node(&PQ, &num_nodes);
		node_t* N2 = pop_node(&PQ, &num_nodes);
		node_t* New = create_node(0, N1->frequency + N2->frequency,TRUE);
		New->left = N1;
		New->right = N2;
		push_node(&PQ, &num_nodes, New);
		print_frequency_nodes(PQ, num_nodes);
	}

	return PQ;
}

tree_t* build_huffman_tree(unsigned int* frequencies, long int filesize, int nnodes)
{

	// create priority queue of frequencies
	node_t **PQ = create_list(frequencies, nnodes);
	sort(PQ, nnodes);
	print_frequency_nodes(PQ, nnodes);

	// create huffman tree
	PQ = assemble_tree(PQ, nnodes);
	tree_t* T = init_tree(PQ[0], nnodes);

	free(PQ);
	return T;
}

int get_tree_depth(node_t* T)
{
	if (T == NULL) return 0;
	else
	{
		int depth_l = get_tree_depth(T->left);
		int depth_r = get_tree_depth(T->right);
		if (depth_l > depth_r) return depth_l + 1;
		else return depth_r + 1;
	}
}

typedef struct code
{
	unsigned char symbol;
	int len;
	unsigned char* bstring;
} code_t;

void get_code_util(node_t* N, unsigned char *array, unsigned char **bstring, int len, int *blength, unsigned char sym)
{
	if (N->left != NULL)
	{
		array[len] = 0;
		get_code_util(N->left, array, bstring, len + 1, blength, sym);
	}
	if (N->right != NULL)
	{
		array[len] = 1;
		get_code_util(N->right, array, bstring, len + 1, blength, sym); }

	if (N->internal == FALSE && sym == N->c)
	{
		*blength = len;
		for (int i = 0; i < len; i++)
		{
			(*bstring)[i] = array[i];
		}
		return;
	}
}

code_t get_code(node_t* N, unsigned char symbol, int depth)
{
	code_t code;
	code.symbol = symbol;
	code.len = 0;
	code.bstring = (unsigned char*)malloc(sizeof(unsigned char) * depth);
	unsigned char *tmparray = (unsigned char*)malloc(sizeof(unsigned char) * depth);

	get_code_util(N, tmparray, &(code.bstring), code.len, &(code.len), code.symbol);

	free(tmparray);
	return code;
}

code_t* get_codes(tree_t* T, unsigned int* freq, int n, int depth)
{
	int j = 0;
	code_t* C = (code_t *)malloc(sizeof(code_t) * n);
	for (int i = 0; i < 256; i++)
	{
		if (freq[i] == 0) continue;
		C[j] = get_code(T->root, i, depth);
		j++;
	}

	for (j = 0; j < n; j++)
	{
		debug("%u\t", C[j].symbol);
		for (int i = 0; i < C[j].len; i++) debug("%u ", C[j].bstring[i]);
		debug("%c", '\n');
	}
	return C;
}

void dumpbits(unsigned int **array, long int *size, unsigned int *bitstring, int *bits)
{
	unsigned int *tmp = (unsigned int *)calloc(sizeof(unsigned int), *size + 1);
	for (int i = 0 ; i < *size; i++) tmp[i] = (*array)[i];
	free(*array);
	*array = tmp;
	tmp = NULL;

	(*array)[*size] = *bitstring;
	(*size)++;
	*bitstring = 0;
	*bits = 0;
	return;
}

code_t find_code(code_t *codelist, int num_codes, unsigned char symbol)
{
	for (int i = 0 ; i < num_codes; i++)
	{
		if (symbol == codelist[i].symbol) return codelist[i];
	}
	printf("ERROR: SYMBOL %u NOT FOUND! OH NOOOOOOOOOO...\n", symbol);
	exit(0);
}

void huffman_a(unsigned char* data, unsigned int **archive, long int filesize, long int *archivesize)
{
	tree_t* huff_tree;
	unsigned int *frequencies = (unsigned int *)calloc(sizeof(unsigned int), 256);
	int num_nodes = 0;
	int max_sym_len;
	int bits = 0;
	code_t code;
	unsigned int bitstring = 0;
	num_nodes = find_frequency(data, filesize, &frequencies);

	//create huffman tree
	huff_tree = build_huffman_tree(frequencies, filesize, num_nodes);
	max_sym_len = get_tree_depth(huff_tree->root);
	print2DUtil(huff_tree->root, 0);
	debug("MAX DEPTH: %d\n",max_sym_len);

	//generate list of codes
	code_t *codelist = get_codes(huff_tree, frequencies, num_nodes, max_sym_len);

	//allocate space for archive
	*archive = (unsigned int*)malloc(sizeof(unsigned int) * (num_nodes * 2 + 1));
	*archivesize = 0;
	(*archive)[0] = filesize;
	(*archivesize)++;
	for (int i = 0; i < 256; i++)
	{
		if (frequencies[i] == 0) continue;
		(*archive)[*archivesize] = i;
		(*archivesize)++;
		(*archive)[*archivesize] = frequencies[i];
		(*archivesize)++;
	}

	for (int i = 0; i < filesize; i++)
	{
		code = find_code(codelist, num_nodes, data[i]);
		for (int j = 0; j < code.len; j++, bits++)
		{
			if (bits == 32) dumpbits(archive, archivesize, &bitstring, &bits);
			bitstring |= (code.bstring[j] << bits);
		}
	}
	dumpbits(archive, archivesize, &bitstring, &bits);

	for (int i = 0; i < num_nodes; i++)
	{
		free((codelist[i]).bstring);
	}
	free(frequencies);
	return;
}

unsigned int *create_freq_list(unsigned int *archive, int *nnodes, long int fsize)
{
	int totalchars = 0;
	int i = 1;
	unsigned int *frequencies = (unsigned int *)calloc(sizeof(unsigned int), 256);
	while (totalchars < fsize)
	{
		frequencies[archive[i]] = archive[i + 1];
		totalchars += archive[i + 1];
		i += 2;
		(*nnodes)++;
	}

	assert(totalchars == fsize && "TOTAL CHARS NEQ FILESIZE\n");
	return frequencies;
}

unsigned char get_bit(unsigned int val, unsigned int idx)
{
	unsigned char bit = (val & (1 << idx)) ? 1 : 0;
	return bit;
}

unsigned char match_code(unsigned int *archive, code_t *codes, int num_codes, int max_len, int *bits, int *archidx)
{
	unsigned int sym_len = 0;
	int i = 0, j = 0;
	unsigned char *code = (unsigned char *)calloc(sizeof(unsigned char), max_len);

	while (sym_len < max_len)
	{
		if (*bits >= 32)
		{
			(*archidx)++;
			*bits = 0;
		}
		code[sym_len] = get_bit(archive[*archidx],*bits);
		(*bits)++;
		sym_len++;
		for (i = 0; i < num_codes; i++)
		{
			if (sym_len == codes[i].len)
			{
				for (j = 0; j < sym_len; j++)
				{
					if (code[j] != codes[i].bstring[j]) break;
				}
				if (j == sym_len)
				{
					return codes[i].symbol;
				}
			}
		}
	}
	printf("UH OH, NOT FOUND... LOOKS LIKE YOUR DEMO IS FAILING.\n");
	exit(0);
}

void huffman_x(unsigned char** data, unsigned int* archive, long int *filesize, long int archivesize)
{
	*filesize = archive[0];
	int num_nodes = 0;
	int max_sym_len;

	// get frequencies from archive
	unsigned int *frequencies = create_freq_list(archive, &num_nodes,*filesize);

	//create huffman tree
	tree_t *huff_tree = build_huffman_tree(frequencies, *filesize, num_nodes);
	max_sym_len = get_tree_depth(huff_tree->root);
	print2DUtil(huff_tree->root, 0);
	debug("MAX DEPTH: %d\n",max_sym_len);

	// create list of codes from tree
	code_t *codelist = get_codes(huff_tree, frequencies, num_nodes, max_sym_len);

	// decompress
	*data = (unsigned char *)calloc(sizeof(unsigned char), (*filesize));
	int archidx = 2 * num_nodes + 1;
	int bits = 0;
	for (int i = 0; i < *filesize; i++)
	{
		(*data)[i] = match_code(archive, codelist, num_nodes, max_sym_len, &bits, &archidx);
	}

	return;
}
