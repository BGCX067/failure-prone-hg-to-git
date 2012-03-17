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
