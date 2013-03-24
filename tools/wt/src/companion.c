#include "companion.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

NameList* names[50];

Character* generateCharacter(int type, char* nation){

	Character* ch = malloc(sizeof(Character));
	for(int i = 0; i < 12; i++)
		ch->skills[i] = 0;

	ch->age = rand() % 40;
	ch->loyalty = 0;
	
	ch->st = rand() % 5;
	ch->iq = rand() % 5;
	ch->ch = rand() % 5;
	ch->dx = rand() % 5;

	switch (type){

		case GUARD:
			ch->skills[WEAPONSKILL] = 2 + (rand()%3);
			break;
		case HUNTER:
			ch->skills[SURVIVAL] = 2 + (rand()%3);
			break;
		case HANDMAIDEN:
			ch->skills[COOKING] = 2 + (rand()%3);
			break;
		case STABLEHAND:
			ch->skills[ANIMALHANDLING] = 2 + (rand()%3);
			break;

		case SPY:
			ch->skills[THIEVERY] = 2 + (rand()%3);
			break;
		case SCRIBER:
			ch->skills[SCRIBE] = 2 + (rand()%3);
			break;
		case WOODSMAN:
			ch->skills[WOODWORKING] = 2 + (rand()%3);
			break;
		case BLACKSMITH:
			ch->skills[BLACKSMITH] = 2 + (rand()%3);
			break;
	}

	ch->name = getRandomName(nation);
	return ch;
}

int loadNames(){

	for (int i = 0; i < 50; i++)
		names[i] = NULL;

	FILE* f;
	f = fopen("data/names.cfg", "r");

	if (!f)
		return -1;

	char buffer[100];
	int ncount = 0;

	int inside = 0;
	while(!feof(f)){
		fgets(buffer, 100, f);
		if ( (buffer[0] != ' ') && (strlen(buffer) >= 3) && (inside == 0) ){
			inside = 1;
			names[ncount] = malloc(sizeof(NameList));
			names[ncount]->n = 0;
			names[ncount]->name = NULL;
			int tsize = strlen(buffer);
			names[ncount]->type = malloc(sizeof(char)*tsize);
		//	printf("%s %d \n", buffer, tsize);
			int m = 0;
			for (int l = 0; l < tsize; l++){
				if (buffer[l] != ' ' && buffer[l] != '\t' && buffer[l] != '\n'){
					names[ncount]->type[m] = buffer[l];
					m++;
				}
			}
			names[ncount]->type[m] = '\0';
//			strcpy(names[ncount]->type, buffer);
	//		printf("type %s \n", names[ncount]->type);
			continue;
		}
		if (inside && buffer[0] == '}' ){
			inside = 0;
			ncount++;
			continue;
		}
		if (inside && buffer[0] != '{'){ //deve ler
			int strsz = 0;
			for(unsigned int i = 0; i < strlen(buffer); i++){ //conta o tamanho da palavra pro malloc
				if (buffer[i] != ' ' && buffer[i] != '\t' && buffer[i] != '\n')
					strsz++;
			}
			names[ncount]->n++;
			NameNode* ne = malloc(sizeof(NameNode));
			ne->name = malloc((sizeof(char)*strsz+1));
			ne->next = NULL;
			for(unsigned int i = 0, j = 0;  i < strlen(buffer); i++){ //copia a string
				if (buffer[i] != ' ' && buffer[i] != '\t' && buffer[i] != '\n'){
					ne->name[j] = buffer[i];
					j++;
				}
			}
			ne->name[strsz] = '\0';
			if (names[ncount]->n == 1){
				names[ncount]->name = ne;
			}else{ //acha um local pra alocar
				int k = 0;
				NameNode* ptr = names[ncount]->name;
				while (ptr->next != NULL){
					//names[k]->name->next = ne;
					ptr = (ptr->next);
					k++;
				}
				ptr->next = ne;
			}
			//printf("name %s  %d\n", buffer, strsz);
		}
	}

	fclose(f);
	printf("%s \n", getRandomName("swedishm"));
	return 1;
}

char* getRandomName(char* type){

	int i = 0;
	while (names[i] != NULL){
		//int cmp = strcmp(type, names[i]->type);
		//printf("%s %s %d %d %d \n", type, names[i]->type, cmp, strlen(type), strlen(names[i]->type));
		if (strcmp(type, names[i]->type) == 0){
			int r = rand() % (names[i]->n);
			int k = 1;
			NameNode* chosen = names[i]->name;
			while( k < r){
				chosen = chosen->next;
			//	printf(" %d \n", k);
				k++;
			}
			//printf("chosen %s \n", chosen->name);
			return chosen->name;
		}
		i++;
	}
	return NULL;
}
