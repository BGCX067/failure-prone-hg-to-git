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
int InitializeGUI(int w, int h);
void BeginGUI(event *e);
void EndGUI();

//widgets
void DoLabel(float posx, float posy, char* text);
int  DoButton(int id, Rect* r, char* text);
int  DoToggleButton(int id, Rect* r, char* text, int* state);
void DoHorizontalSlider(int id, Rect* r, float* value);
void DoVerticalSlider(int id, Rect* r, float* value);
int  DoRadioButton(int id, Rect* r, char* text, int* state);
int  DoCheckButton(int id, Rect* r, char* text, int* state);
void DoImage(int id, float x, float y, float sizex, float sizey, Texture* t);
int  DoComboBox(int id, Rect* r, int numOptions, char* options[], int* selected, int * state);

int BeginMenu(int id, int x, int y, int w, int h, float* xoffset, float* yoffset, char* text, Texture* t);
void EndMenu(int id, int x, int y, int w, int h, float* xoffset, float* yoffset);

int DoTooltip(int id, float x, float y, float w, float h);
int DoLineEdit(int id,  Rect* r, char* text, int maxTextLength );


//aux functions
void SetFontColor(float r, float g, float b, float a);
int SetFontSize(int size);

/// PLOT
typedef struct _matrix{
	int sizex;
	int sizey;
	float* data;

	float** color;

}MatrixData;

int DoMatrix(MatrixData* a, float x, float y, float size);
void DoLine(int x1, int y1, int x2, int y2);
void Plot1d(float* serie, int num, int x, int y, int w, int h, int r, int g, int b);

#endif

