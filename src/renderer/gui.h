#ifndef _GUI_H_
#define _GUI_H_

#include <time.h> //pro time_t

enum ButtonFlags{
	BUTTON_OFF = 0x0,
	BUTTON_ON = 0x1,
	BUTTON_BEGIN = 0x2,
	BUTTON_END = 0x4

};

typedef struct _buttonState{
	int state;
	time_t time;
	point cursor;
};

typedef struct _GUI{

	int widgetShader;
	int textureViewShader;

	unsigned int  setupList;
	unsigned int  restoreList;

	int w, h;

}GUI;

GUI* initializeGUI(int w, int h);
void beginGUI();
void endGUI();

int doButton(rect* r, char* text, int* state, int style);


void initfont(void);
void stbtt_print(float x, float y, char *text);

#endif

