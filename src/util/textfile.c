#include "textfile.h"
#include <stdio.h>

char* readTextFile(char* filename){

	FILE *f;
	f = fopen(filename,"r");
	if (!f){
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	long int fileLength = ftell(f);
	rewind(f);

	char* text = (char*) malloc(sizeof(char)*(fileLength+1));
	fread(text, fileLength, 1, f);
	text[fileLength] = '\0';

	fclose(f);
	return text;
}
