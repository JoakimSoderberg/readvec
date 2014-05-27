
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct CvVecFile
{
	char *filename;
	FILE*  input;
	int    count;
	int    vecsize;
	int    last;
	short* vector;
} CvVecFile;

int main(int argc, char **argv)
{
	int ret = 0;
	size_t file_count = 0;
	CvVecFile *vecs = NULL;
	CvVecFile *vec = NULL;
	CvVecFile mergevec;
	short width;
	short height;
	int i;
	char *merge = NULL;
	memset(&mergevec, 0, sizeof(mergevec));

	if (argc < 2)
	{
		fprintf(stderr, "Usage:\n");
		fprintf(stderr, " %s [--merge <vec target>] <vec files>\n", argv[0]);
		return -1;
	}

	for (i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "--merge"))
		{
			if ((i + 1) >= argc)
			{
				fprintf(stderr, "Need to specify a destination merge file\n");
				return -1;
			}

			merge = argv[i];
		}
		else
		{
			file_count++;
		}
	}

	if (!(vecs = (CvVecFile *)calloc(file_count, sizeof(CvVecFile))))
	{
		fprintf(stderr, "%s\n", "Out of memory!");
		return -1;
	}

	for (i = 1; i < argc; i++)
	{
		if (!strncmp(argv[i], "--", 2))
		{
			printf("bla\n");
			continue;
		}

		vec = &vecs[i];
		vec->filename = argv[i];

		if (!(vec->input = fopen(argv[1], "rb")))
		{
			fprintf(stderr, "Failed to open file %s\n", argv[1]);
			ret = -1;
			goto fail;
		}

		fread(&vec->count, sizeof(vec->count), 1, vec->input);
		fread(&vec->vecsize, sizeof(vec->vecsize), 1, vec->input);
		// TODO: Read 2 more shorts (What are they)

		printf("%s: Contains %d vectors of size %d\n",
			vec->filename, vec->count, vec->vecsize);

		mergevec.count += vec->count;
	}

fail:
	if (vecs)
	{
		for (i = 0; i < argc; i++)
		{
			if (vecs[i].input)
			{
				fclose(vecs[i].input);
			}
		}

		free(vecs);
	}

	return ret;
}
