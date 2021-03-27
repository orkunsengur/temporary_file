#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_SIZE				10000
#define PARTIAL_COUNT			100
#define PARTIAL_SIZE			(TOTAL_SIZE / PARTIAL_COUNT)
#define RANDOM_FILE_PATH		"random.dat"
#define RESULT_FILE_PATH		"result.dat"


struct merge_info {
	FILE *f;
	int curval;
};

FILE *create_random_file(void);
void bsort(int *pi, size_t size);
int getmin_index(const struct merge_info *mi, int size);
int verify_result(FILE *f, int size);


int main(void)
{
	FILE *fr, *fd;
	struct merge_info m[PARTIAL_COUNT];
	int i, size, min_index;
	int *buf;

	if ((fr = create_random_file()) == NULL)
	{
		fprintf(stderr,"Cannot create random file!..\n");
		exit(EXIT_FAILURE);
	}

	if ((fd = fopen(RESULT_FILE_PATH, "w+b")) == NULL)
	{
		fprintf(stderr, "cannot create result file!..\n");
		fclose(fr);
		remove(RANDOM_FILE_PATH);
		exit(EXIT_FAILURE);
	}

	if ((buf = (int *)malloc(sizeof(int) * PARTIAL_SIZE)) == NULL)
	{
		fprintf(stderr, "cannot allocate memory!..\n");
		goto FAILURE;
	}

	rewind(fr);

	for (i = 0; i < PARTIAL_COUNT; i++)
	{
		if (fread(buf, sizeof(int), PARTIAL_SIZE, fr) != PARTIAL_SIZE)
		{
			fprintf(stderr, "cannot read from random file!..\n");
			free(buf);
			goto FAILURE; // close and delete both random and result file 
		}
		bsort(buf, PARTIAL_SIZE);
		if ((m[i].f = tmpfile()) == NULL)
		{
			fprintf(stderr, "cannot create temproray file!..\n");
			free(buf);
			goto FAILURE;
		}

		if (fwrite(buf, sizeof(int), PARTIAL_SIZE, m[i].f) != PARTIAL_SIZE)
		{
			fprintf(stderr, "cannot write to temproray file!..\n"); // tüm temporary dosyalar otomatik kapanýr
			free(buf);
			goto FAILURE;
		}
		rewind(m[i].f);   //temporary file pointerý baþa getirilmelidir
	}
	fclose(fr);
	free(buf);

	for (i = 0; i < PARTIAL_COUNT; i++)
	{
		if (fread(&m[i].curval, sizeof(int), 1, m[i].f) != 1)
		{
			fprintf(stderr, "cannot read from temporary file!..\n");
			goto FAILURE;
		}
	}
	size = PARTIAL_COUNT;
	
	while(size>0)
	{
		min_index = getmin_index(m, size);

		if (fwrite(&m[min_index].curval, sizeof(int), 1, fd) != 1)
		{
			fprintf(stderr, "cannot write to destination file!..\n");
			goto FAILURE;
		}

		if (fread(&m[min_index].curval, sizeof(int), 1, m[min_index].f) != 1)
		{
			if (ferror(m[min_index].f))
			{
				fprintf(stderr, "cannot read from temporary file to curval!..\n");
				goto FAILURE;
			}
			m[min_index] = m[size - 1];
			size--;
		}
	}

	rewind(fd);

	printf("%s\n", verify_result(fd, TOTAL_SIZE) ? "Verify Ok...\n" : "Verify Failed!..\n");

	
	fclose(fd);

	return 0;

FAILURE:
	fclose(fr);
	fclose(fd);
	remove(RANDOM_FILE_PATH);
	remove(RESULT_FILE_PATH);
	exit(EXIT_FAILURE);

	

}


int getmin_index(const struct merge_info *mi, int size)
{
	int i, min_index = 0;

	
	for (i = 1; i < size; i++)
	{
		if (mi[min_index].curval > mi[i].curval)
		{
			min_index = i;
		}
	}

	return min_index;
}




FILE *create_random_file(void)
{
	FILE *f;
	int i, val;

	srand(time(NULL));

	if ((f = fopen(RANDOM_FILE_PATH, "w+b")) == NULL)
	{
		return NULL;
	}

	for (i = 0; i < TOTAL_SIZE; i++)
	{
		val = rand();
		if (fwrite(&val, sizeof(int), 1, f) != 1)
		{
			fclose(f);
			remove(RANDOM_FILE_PATH);
			return NULL;
		}
	}

	return f;
}






void bsort(int *pi, size_t size)
{
	size_t i, k;
	int temp;

	for (i = 0; i < size - 1; i++)
	{
		for (k = 0; k < size - 1 - i; k++)
		{
			if (pi[k] > pi[k + 1])
			{
				temp = pi[k];
				pi[k] = pi[k+1];
				pi[k + 1] = temp;
			}
		}
	}

}


int verify_result(FILE *f, int size)
{
	int val, prev;
	int i;

	if (fread(&prev, sizeof(int), 1, f) != 1)
	{
		return 0;
	}

	for (i = 1; i < size; i++)
	{
		if (fread(&val, sizeof(int), 1, f) != 1)
		{
			return 0;
		}
		if (val < prev)
		{
			return 0;
		}
		prev = val;
	}

	return ftell(f) == size * sizeof(int);
}



