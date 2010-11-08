#ifndef _GLAPP_H_
#define _GLAPP_H_

#include "renderer/scene.h"

typedef struct _glapp{
	int width;
	int height;
	int depth;
}glapp;


/* enum para os tipos de evento */
enum {
    NO_EVENT = 1 << 0,
    KEYBOARD_EVENT = 1 << 1,
    MOUSE_MOTION_EVENT = 1 << 2,
    MOUSE_BUTTON_EVENT = 1 << 3
};

/* enum para as keys do evento */
enum {
	KEY_ESC = 256,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_W,
	KEY_A,
	KEY_S,
	KEY_D
};

/* enum para button do evento */
enum{
	BUTTON_LEFT = 1 << 0,
	BUTTON_MIDDLE = 1 << 1,
	BUTTON_RIGHT = 1 << 2,
	BUTTON_UP = 1 << 3,
	BUTTON_DOWN = 1 << 4
};

typedef struct _event {
    int type;
    int keys[512];
    int buttonLeft;
    int buttonRight;
    int button;
    int x, y;
}event;

glapp* setVideoMode(int w, int h, int fullscreen);
void closeVideo();

void setWindowTitle(char* title);
void setMouse(int x, int y);

int  getTime();

void mainloop(glapp* app, int(idle)(float, event*, scene* ), int(render)(float, event*, scene*), scene* s);

#endif
