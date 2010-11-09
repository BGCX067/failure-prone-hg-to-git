#ifndef _GUI_H_
#define _GUI_H_

#include <time.h> //pro time_t



typedef struct _GUI{

	int widgetShader;
	int textureViewShader;

	unsigned int  setupList;
	unsigned int  restoreList;

	int w, h;

	int hotitem;
	int activeitem;

	int focusx, focusy;
	int twophase;

}GUI;

GUI* initializeGUI(int w, int h);
void beginGUI();
void endGUI();

int doButton(int id, rect* r, char* text);


#endif

