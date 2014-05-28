
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct CvVecFile
{
	char *filename;
	FILE*  input;
	int    count;
	int    vecsize;
	int    last;
	short* vector;
} CvVecFile;

int write_vec_header(FILE *f, int count, int vecsize, short min, short max)
{
	if (fseek(f, 0, SEEK_SET))
	{
		fprintf(stderr, "Failed to seek in file\n");
		return -1;
	}

	fwrite(&count, sizeof(count), 1, f);
	fwrite(&vecsize, sizeof(vecsize), 1, f);
	fwrite(&min, sizeof(min), 1, f);
	fwrite(&max, sizeof(max), 1, f);

	//fseek(f, 0, SEEK_END);
	return 0;
}

int main(int argc, char **argv)
{
	int ret = 0;
	size_t file_count = 0;
	CvVecFile *vecs = NULL;
	CvVecFile *vec = NULL;
	CvVecFile mergevec;
	int vecsize = 0;
	short min;
	short max;
	int i;
	FILE *output = NULL;
	short *samples = NULL;
	int vec_index = 0;
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

			i++;
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

	// Read the headers of all files.
	for (i = 1; i < argc; i++)
	{
		if (!strncmp(argv[i], "--", 2))
		{
			i++;
			continue;
		}

		vec = &vecs[vec_index];
		vec_index++;
		vec->filename = argv[i];

		if (!(vec->input = fopen(vec->filename, "rb")))
		{
			fprintf(stderr, "Failed to open file %s\n", vec->filename);
			ret = -1;
			goto fail;
		}

		fread(&vec->count, sizeof(vec->count), 1, vec->input);
		fread(&vec->vecsize, sizeof(vec->vecsize), 1, vec->input);
		fread(&min, sizeof(min), 1, vec->input);
		fread(&max, sizeof(max), 1, vec->input);

		if (!vecsize)
		{
			vecsize = vec->vecsize;
		}
		else if (vecsize != vec->vecsize)
		{
			fprintf(stderr, "Error! Size %d for %s is not the same as the expected size %d. "
				"All .vec files must contain samples of the same size!\n",
				vec->vecsize, vec->filename, vecsize);
			ret = -1;
			goto fail;
		}

		printf("%s: Contains %d vectors of size %d. Min %hd, Max %hd\n",
			vec->filename, vec->count, vec->vecsize, min, max);

		mergevec.count += vec->count;
	}

	if (merge)
	{
		int sample_size = (vecsize * sizeof(short)) + sizeof(unsigned char);

		printf("Merging files...\n");

		if (!(output = fopen(merge, "w")))
		{
			fprintf(stderr, "Failed to open merge file %s for writing: %s\n",
				merge, strerror(errno));
			ret = -1;
			goto fail;
		}

		if (write_vec_header(output, mergevec.count, vecsize, 0, 0))
		{
			ret = -1;
			goto fail;
		}

		for (i = 0; i < file_count; i++)
		{
			vec = &vecs[i];

			if (!(samples = (short *)malloc(sample_size * vec->count)))
			{
				fprintf(stderr, "Out of memory!\n");
				ret = -1;
				goto fail;
			}

			fread(samples, sample_size, vec->count, vec->input);
			fwrite(samples, sample_size, vec->count, output);

			free(samples);
			samples = NULL;
		}

		printf("Successfully merged %d vectors into %s\n", mergevec.count, merge);
	}

fail:
	if (vecs)
	{
		for (i = 0; i < file_count; i++)
		{
			if (vecs[i].input)
			{
				fclose(vecs[i].input);
			}

			if (vecs[i].vector)
			{
				free(vecs[i].vector);
				vecs[i].vector = NULL;
			}
		}

		free(vecs);
	}

	if (output)
	{
		fclose(output);
	}

	if (samples)
	{
		free(samples);
	}

	return ret;
}
