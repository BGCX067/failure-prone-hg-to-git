#include "cfgfile.h"
#include "textfile.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void DelTokens(char** buf, char* str){

	int  sz = 0;
	unsigned int i, k;

	for (i = 0; i < strlen(str); i++){
		if ( !(str[i] == '\t' || str[i] == '\n' /*|| str[i] == ' '*/) ){
			sz++;
		}
	}

//	sz++;
	*buf = malloc(sz);

	k= 0;
	for (i = 0; i < strlen(str); i++){
		if (!(str[i] == '\t' || str[i] == '\n' /*|| str[i] == ' '*/)){
			(*buf)[k] = str[i];
			k++;
		}
	}

	(*buf)[sz] = '\0';

}


Cfg* ReadConfigFile(char* filename){

	Cfg* fd = (Cfg*) malloc(sizeof(Cfg));
	fd->filename = malloc(sizeof(char)*strlen(filename));
	fd->nelems = 0;
	strcpy(fd->filename, filename);

	FILE* fp;
	char* line = NULL;
	size_t read = 1;
	size_t len = 0;
	int i; 
	unsigned int k = 0;
	int isbracket = 0;

	fp = fopen(filename, "r");
	if (!fp)
		return NULL;

	int inside = 0;
	while ( (int)read >= 0 ) {
		isbracket = 0;
		read = GetLine(&line, &len, fp);
		//printf("read %d line %s ", (int)read, line);
		for (i = 0; i < (int) read; i++){
			if (line[i] == '}'){ //TODO checar se tem espacos
				inside = 0;
				isbracket = 1;
			}
			if (line[i] == '{'){
				inside = 1;
				isbracket = 1;
			}
		}
		if (!inside && !isbracket){ //se nao tiver dentro entao deve ser Key
			char* key = strtok(line, " ");
			key = strtok(NULL, " ");
			if (key == NULL || strlen(key) == 0)
				continue;
			fd->nelems++;
			fd->elems = realloc(fd->elems, (fd->nelems)*sizeof(CfgElem));
			CfgElem* elem = &(fd->elems[fd->nelems-1]);
			elem->TextLines = 0;
		//	printf("key %s %d \n", key, fd->nelems);
			char* nkey = NULL;
			DelTokens(&nkey, key);
		//	printf("nkey: %s \n", nkey);
			elem->key = malloc(sizeof(char)*strlen(nkey));
			strcpy(elem->key, nkey);
		//	printf("copy: %s \n", elem->key);
			free(nkey);
		}
		if (inside && !isbracket){ //se tiver dentro copia a info
				CfgElem* elem = &(fd->elems[fd->nelems-1]);
				elem->TextLines++;
				int nlines = elem->TextLines;
				elem->propnames = realloc(elem->propnames, sizeof(char*)*(nlines));
				elem->propvalues= realloc(elem->propvalues, sizeof(char*)*(nlines));
				char* newline = NULL; //precisa de 2 pq o strtok zoa o newline
				char* newline2 = NULL;
				DelTokens(&newline, line); //tira /n e /t
				DelTokens(&newline2, line);
				char* propname = strtok(newline, " ");
//				printf("prop value %s \n", propvalue);
				elem->propnames[nlines-1] = malloc(sizeof(char)*strlen(propname));
				strcpy(elem->propnames[nlines-1], propname );
				//copia na marra
				int offset = strlen(propname);
				//for para tirar eventuais espacos ou tabs
				for (k = 0; k < strlen(newline2); k++)
					if (newline2[k+offset] == ' ' || newline2[k+offset] == '\t')
						offset++;
					else
						break;

				elem->propvalues[nlines-1] = malloc(sizeof(char)*(strlen(newline2)-offset)+1);
			//	printf( "newline2: %s offset: %d strlen(newline2) %d fim %d \n",newline2,offset, strlen(newline2), strlen(newline2)-offset );
				for (k = 0; k < strlen(newline2) - offset; k++){
			//		printf("k: %d indice: %d %c \n", k, k+offset, newline2[k+offset]);
					elem->propvalues[elem->TextLines-1][k] = newline2[k+offset];
				}
			//	printf("terminando com null no indice: %d \n", k);
				elem->propvalues[elem->TextLines-1][k] = '\0';

				free(newline);
				free(newline2);
		}
	}
 	
	free(line);
	return fd;

}

void FreeConfigFile(Cfg* f){
	if (f){
		unsigned int i, j;
		if (f->filename)
			free(f->filename);

		for (i = 0; i < f->nelems; i++){
			if (f->elems[i].key)
				free(f->elems[i].key);

			if (f->elems[i].propnames){
				for ( j =0; j < f->elems[i].TextLines; j++)
					if (f->elems[i].propnames[j])
						free(f->elems[i].propnames[j]);
				free(f->elems[i].propnames);
			}

			if (f->elems[i].propvalues){
				for ( j =0; j < f->elems[i].TextLines; j++)
					if (f->elems[i].propvalues[j])
						free(f->elems[i].propvalues[j]);
				free(f->elems[i].propvalues);
			}
		}
	}
}

CfgElem* FindConfigElem(Cfg* c, char* elem){
	unsigned int i = 0;
	for (i = 0; i < c->nelems; i++){
		if (strcmp(elem, c->elems[i].key) == 0)
			return &(c->elems[i]);
	}
	return NULL;
}

int GetConfigInt(CfgElem* elem, char* prop){
	unsigned int i = 0;
	for (i = 0; i < elem->TextLines; i++){
		if (strcmp(prop, elem->propnames[i]) == 0){
			int ret;
			sscanf(elem->propvalues[i], " %d", &ret);
			return ret;
		}
	}

	return 0;
}

float GetConfigReal(CfgElem* elem, char* prop){
	unsigned int i = 0;
	for (i = 0; i < elem->TextLines; i++){
		if (strcmp(prop, elem->propnames[i]) == 0){
			float ret;
			sscanf(elem->propvalues[i], " %f", &ret);
			return ret;
		}
	}

	return 0;

}

char* GetConfigString(CfgElem* elem, char* prop){
	unsigned int i = 0;
	for (i = 0; i < elem->TextLines; i++){
		if (strcmp(prop, elem->propnames[i]) == 0){
			return elem->propvalues[i];
		}
	}

	return NULL;
}

char** GetConfigStringArray(CfgElem* elem, char* prop, int* size){
	char** ret = NULL;
	unsigned int i = 0;
	*size = 0;
	for (i = 0; i < elem->TextLines; i++){
		if (strcmp(prop, elem->propnames[i]) == 0){
			(*size)++;
			ret = realloc(ret, sizeof(char*)*(*size));
			ret[*size-1] = elem->propvalues[i];
			//printf("value: %s \n", elem->propvalues[i]);
		}
	}

	return ret;
}

