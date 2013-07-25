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
    MOUSE_BUTTON_RELEASE = 1 << 5
};

/* enum para as keys do evento */
enum {
	KEY_ESC = 256,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_ENTER,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_LCONTROL,
	KEY_RCONTROL,
	KEY_LALT,
	KEY_RALT,
	KEY_SPACE,
	KEY_INSERT,
	KEY_DEL,
	KEY_PAGEUP,
	KEY_PAGEDOWN,
	KEY_HOME,
	KEY_END,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_0,
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
	KEY_z,
    KEY_minus = 45,     
    KEY_period = 46,        
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

void SetVideoMode(int w, int h, int fullscreen);
void CloseVideo();

void SetWindowTitle(char* title);
void SetMouse(int x, int y);
void WarpMouse( int yesno);

double GetTime();

void MainLoop();
void SwapBuffers();

unsigned int GetScreenW();
unsigned int GetScreenH();

//int Update( event* e, float* dt );
//int Render(event *e, float* dt);
#endif
