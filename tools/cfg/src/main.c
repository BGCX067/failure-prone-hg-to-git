#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../../src/util/cfgfile.h"

int main(){

	Cfg* c = ReadConfigFile("recipes.cfg");	

	int i = 0;
	int j = 0;
	for( i = 0; i < c->nelems; i++){
		printf("%s \n", c->elems[i].key);
		for (j = 0; j < c->elems[i].TextLines; j++){
			printf("%s \n", c->elems[i].propnames[j]);
			printf("%s \n", c->elems[i].propvalues[j]);
		}
	}

	CfgElem* e = FindConfigElem(c, "applepie");
	if (!e)
		printf("Elem not found \n");
	else{
		printf( "GetConfigInt(elem, \"Cost\"): %d \n", GetConfigInt(e, "Cost") );
		printf( "GetConfigInt(elem, \"Calories\"): %d \n", GetConfigInt(e, "Calories"));
		printf( "GetConfigReal(elem, \"Deliciousness\"): %f \n", GetConfigReal(e, "Deliciousness"));

		char* madelike = GetConfigString(e, "Madelike");
		printf( "GetConfigString(elem, \"Madelike\"): %s \n", madelike);
		int size =0;
		int i = 0;
		char** ingredients = GetConfigStringArray(e, "Ingredient", &size);
		for ( i = 0; i <size; i++)
			printf("%s\n", ingredients[i]);
	}

	FreeConfigFile(c);
	return 0;
}
