#ifndef _GLAPP_H_
#define _GLAPP_H_

#include "renderer/scene.h"

typedef struct _glapp{
	int width;
	int height;
	int depth;

	int warpmouse;
}glapp;


/* enum para os tipos de evento */
enum {
    NO_EVENT = 1 << 0,
    KEYBOARD_EVENT = 1 << 1,
    MOUSE_MOTION_EVENT = 1 << 2,
    MOUSE_BUTTON_EVENT = 1 << 3,
    MOUSE_BUTTON_PRESS = 1 << 4,
    MOUSE_BUTTON_RELEASE = 1 << 4
};

/* enum para as keys do evento */
enum {
	KEY_ESC = 256,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_BACKSPACE,
	KEY_SPACE = 32,
	KEY_a = 97,
	KEY_b,
	KEY_c,
	KEY_d,
	KEY_e,
	KEY_f,
	KEY_g,
	KEY_h,
	KEY_i,
	KEY_j,
	KEY_k,
	KEY_l,
	KEY_m,
	KEY_n,
	KEY_o,
	KEY_p,
	KEY_q,
	KEY_r,
	KEY_s,
	KEY_t,
	KEY_u,
	KEY_v,
	KEY_w,
	KEY_x,
	KEY_y,
	KEY_z
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

void setVideoMode(int w, int h, int fullscreen);
void closeVideo();

void setWindowTitle(char* title);
void setMouse(int x, int y);

void warpmouse( int yesno);

double getTime();

void MainLoop();

void swapBuffers();


//int Update( event* e, float* dt );
//int Render(event *e, float* dt);
#endif
