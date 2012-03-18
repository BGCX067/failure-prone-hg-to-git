#ifndef _GUI_H_
#define _GUI_H_

#include "util/fontstash.h"
#include "../glapp.h"
#include "renderer.h"

typedef struct _GUI{

	Shader* widgetShader;
	Shader* textureViewShader;
	Shader* colorShader;

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

	struct sth_stash* stash;

	float widgetColor[4][4];
	float borderColor[4][4];
	float fontColor[4][4];
	
	int drawingMenu;
	float menuoffsetx,menuoffsety;
}GUI;

int initializeGUI(int w, int h);
void beginGUI(event *e);
void endGUI();

void doLabel(rect* textRect, char* text);
int doButton(int id, rect* r, char* text);
void doHorizontalSlider(int id, rect* r, float* value);
void doVerticalSlider(int id, rect* r, float* value);
int doRadioButton(int id, rect* r, char* text, int* state);
int doCheckButton(int id, rect* r, char* text, int* state);

void doLine(int x1, int y1, int x2, int y2);

int beginMenu(int id, int x, int y, int w, int h, float* xoffset, float* yoffset, char* text);
void endMenu(int id, int x, int y, int w, int h, float* xoffset, float* yoffset);

void plot1d(float* serie, int num, int x, int y, int w, int h, int r, int g, int b);


#endif

