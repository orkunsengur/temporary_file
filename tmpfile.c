/*------------------------------------------------------------------------------------
tmpfile isimli standart C fonksiyonu olmayan bir dosya ismi ile bir dosyayý "w+b" modunda yaratýr ve dosyaya iliþkin
dosya pointer'ý ile geri döner. Böyle dosyalar kapatýldýðýnda diskteki dosya da otomatik olarak silinecektir.
--------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>



int main()
{
	FILE *f;
	uint8_t i;
	uint8_t val;

	if ((f = tmpfile()) == NULL)
	{
		fprintf(stderr, "temp file cannot created!..\n");
		exit(EXIT_FAILURE);    //exit iþlemi tüm geçici dosyalarý siler
	}


	for (i = 0; i < 100; i++)
	{
		if (fwrite(&i, sizeof(uint8_t), 1, f) != 1)
		{
			fprintf(stderr, "data is not written to the file\n");
			exit(EXIT_FAILURE);
		}
	}

	fseek(f, 0, SEEK_SET);    // Set file pointer position to the begining of the file

	while (fread(&val, sizeof(uint8_t), 1, f) == 1)
	{
		printf("%d\n", val);
	}

	if (ferror(f))
	{
		fprintf(stderr, "cannot read file..");
		exit(EXIT_FAILURE);
	}

	fclose(f);


	return 0;
}

