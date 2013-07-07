#ifndef _GUI_H_
#define _GUI_H_

#include "../glapp.h"
#include "renderer.h"

typedef struct _GUI{

	Shader* widgetShader;
	Shader* textureViewShader;
	Shader* colorShader;
	Shader* skinnedShader;
	Shader* fontshader;

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

	float widgetColor[4][4];
	float borderColor[4][4];
	
	int drawingMenu;
	float menuoffsetx,menuoffsety;

	float fontColor[4];
	int fontsize; 
}GUI;

//necessary GUI system functions 
int initializeGUI(int w, int h);
void beginGUI(event *e);
void endGUI();

//widgets
void doLabel(float posx, float posy, char* text);
int  doButton(int id, rect* r, char* text);
int  doToggleButton(int id, rect* r, char* text, int* state);
void doHorizontalSlider(int id, rect* r, float* value);
void doVerticalSlider(int id, rect* r, float* value);
int  doRadioButton(int id, rect* r, char* text, int* state);
int  doCheckButton(int id, rect* r, char* text, int* state);
void doImage(int id, float x, float y, float sizex, float sizey, Texture* t);
int  doComboBox(int id, rect* r, int numOptions, char* options[], int* selected, int * state);

int beginMenu(int id, int x, int y, int w, int h, float* xoffset, float* yoffset, char* text, Texture* t);
void endMenu(int id, int x, int y, int w, int h, float* xoffset, float* yoffset);

int doTooltip(int id, float x, float y, float w, float h);
int doLineEdit(int id,  rect* r, char* text, int maxTextLength );


//aux functions
void setFontColor(float r, float g, float b, float a);
int setFontSize(int size);

/// PLOT
typedef struct _matrix{

	int sizex;
	int sizey;
	float* data;

	float** color;

}MatrixData;

int doMatrix(MatrixData* a, float x, float y, float size);
void doLine(int x1, int y1, int x2, int y2);
void plot1d(float* serie, int num, int x, int y, int w, int h, int r, int g, int b);

#endif

