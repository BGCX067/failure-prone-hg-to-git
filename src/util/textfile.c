#include "textfile.h"
#include <stdio.h>
#include <stdlib.h> //malloc

char* readTextFile(char* filename){

	FILE *f;
	long result;
	f = fopen(filename,"r");
	if (!f){
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	long int fileLength = ftell(f);
	rewind(f);

	char* text = (char*) malloc(sizeof(char)*(fileLength+1));
	result = fread(text, fileLength, 1, f);
	if (result < fileLength)
		printf("Result from fread less than file length: %s \n", filename);
	text[fileLength] = '\0';

	fclose(f);
	return text;
}

size_t GetDelim(char** linebuff, size_t* linebufsz, int delim, FILE* fd){

	static const int GROWBY = 80;

	int ch;
	int idx = 0;
	
	if (fd == NULL || linebuff == NULL || linebufsz == NULL)
		return -1;

	if (*linebuff == NULL || *linebufsz < 2){
		*linebuff = malloc(GROWBY);
		if (!*linebuff)
			return -2;
		*linebufsz += GROWBY;
	}

	while(1){
		ch = fgetc(fd);
		if (ch == EOF)
			break;

		while (idx > *linebufsz-2){ //2 pra contar com o delim e o 0 no final
			*linebuff = realloc(*linebuff, *linebufsz += GROWBY);
			if (!*linebuff)
				return -2;

		}

		(*linebuff)[idx++] = (char)ch;
		if ((char)ch == delim)
			break;

	}

	if (idx != 0)
		(*linebuff)[idx] = 0;
	if (ch == EOF)
		return -3;

	return idx;

}

size_t GetLine(char** linebuff, size_t *n, FILE *fd){
	return (GetDelim(linebuff, n, '\n', fd));
}

