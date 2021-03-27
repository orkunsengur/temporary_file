
/*------------------------------------------------------------------------------------
Bir dosyadaki # ile baþlayan satýrlarý silen program örneði.
Programýn iki satýr argümaný vardýr. Program ismi ve '#' ile baþlayan satýrlardan 
arýndýrýlacak dosyanýn isminden (yol ifadesi) oluþur.
tmpnam fonksiyonu sadece dosya ismi verir, dosyayý biz açarýz, program bittiðinde dosya 
kapansa da silinmez. Gerekirse biz sileriz.
--------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* argv[0] programýn ismi. argv[1] ise dosyanýn yol ifadesi olsun*/
/*argc komut satýrý argüman sayýsý*/


#define MAX_LINE         8192
#define MAX_PATH         4096


int issharp(const char *);


int main(int argc, char *argv[])
{
	FILE *f, *ftemp;
	char line[MAX_LINE];
	char path[MAX_PATH];


	if (argc != 2)
	{
		fprintf(stderr, "wrong number of arguman");
		exit(EXIT_FAILURE);
	}


	if ((tmpnam(path)) == NULL)
	{
		fprintf(stderr, "cannot get temporary file..!");
		exit(EXIT_FAILURE);
	}


	if ((f = fopen(argv[1], "r")) == NULL)
	{
		fprintf(stderr, "File cannot be opened..!");
		goto FAILURE;
	}

	if ((ftemp = fopen(path, "w")) == NULL)
	{
		fprintf(stderr, "Temporary file cannot be opened..!");
		goto FAILURE;
	}

	while (fgets(line, MAX_LINE, f) != NULL)
	{
		if (!issharp(line))
		{
			if (fwrite(line, sizeof(char), strlen(line), ftemp) < 0)
			{
				fprintf(stderr, "Error ocuurs while writing temporary file!..");
				goto FAILURE;
			}
		}
	}

	if (ferror(f))
	{
		fprintf(stderr, "ýO error occurs!..0");
		goto FAILURE;
	}

	fclose(f);
	fclose(ftemp);



	if (remove(argv[1]) != 0)          // Eski dosya artýk silinebilir..
	{
		fprintf(stderr, "Cannot remove original file!..");
		goto FAILURE;
	}

	if (rename(path, argv[1]) != 0)
	{
		fprintf(stderr, "Cannot rename the temporary file!..");
		exit(EXIT_FAILURE);
	}

	printf("success...");


	exit(EXIT_SUCCESS);

FAILURE:

	if (remove(path) != 0)          // Hata durumunda temporary file silinir
	{
		fprintf(stderr, "Cannot remove the temporary file!..\n");
	}


	exit(EXIT_FAILURE);

}



int issharp(const char *line)
{
	while (isspace(*line))
	{
		line++;
	}

	return *line == '#';
}


