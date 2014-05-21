
#include <stdio.h>
#include <stdlib.h>

typedef struct CvVecFile
{
	FILE*  input;
	int    count;
	int    vecsize;
	int    last;
	short* vector;
} CvVecFile;

int main(int argc, char **argv)
{
	FILE *f = NULL;
	size_t vec_count = 0;
	size_t file_count = 0;
	CvVecFile vec;
	memset(&vec, 0, sizeof(vec));

	if (argc < 2)
	{
		fprintf(stderr, "Usage:\n");
		fprintf(stderr, " %s <vec file>\n", argv[0]);
		return -1;
	}

	if (!(vec.input = fopen(argv[1], "rb")))
	{
		fprintf(stderr, "Failed to open file %s\n", argv[1]);
		return -1;
	}

	fread(&vec.count, sizeof(vec.count), 1, vec.input);
	fread(&vec.vecsize, sizeof(vec.vecsize), 1, vec.input);

	printf("Contains %d vectors of size %d\n", vec.count, vec.vecsize);

	fclose(vec.input);

	return 0;
}
