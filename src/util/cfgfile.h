# ifndef _CFG_H_
#define _CFG_H_

typedef struct elem {
	unsigned int TextLines;
	char* key;
	char** propvalues;
	char** propnames;
}CfgElem;

typedef struct cfg{

	char* filename;
	unsigned int nelems;
	CfgElem* elems;

} Cfg;

Cfg* ReadConfigFile(char* filename);
void FreeConfigFile(Cfg* f);
CfgElem* FindConfigElem(Cfg* c, char* elem);
int GetConfigInt(CfgElem* elem, char* prop);
float GetConfigReal(CfgElem* elem, char* prop);
char* GetConfigString(CfgElem* elem, char* prop);
char** GetConfigStringArray(CfgElem* elem, char* prop, int* size);



#endif
