#ifndef _GUI_H_
#define _GUI_H_

#include "util/fontstash.h"
#include "../glapp.h"

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

	int kbditem;
	int keyentered;
	int keymod;
	int lastwidget;

	struct font_stash* stash;
}GUI;

GUI* initializeGUI(int w, int h);
void beginGUI(event *e);
void endGUI();

int doButton(int id, rect* r, char* text);


#endif

