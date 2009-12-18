#ifndef _GLAPP_H_
#define _GLAPP_H_

typedef struct _glapp{
	int width;
	int height;
	int depth;
}glapp;

enum {
	KEY_ESC = 256,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
    MOUSE_MOVE,
    NO_EVENT
};

enum{
	BUTTON_LEFT = 1 << 0,
	BUTTON_MIDDLE = 1 << 1,
	BUTTON_RIGHT = 1 << 2,
	BUTTON_UP = 1 << 3,
	BUTTON_DOWN = 1 << 4
};

typedef struct _keyboard{
	int keys[512];
}keyboard;

typedef struct _mouse{
	int x;
	int y;
	int button;
}mouse;

typedef struct _event {
    int type;
    int x, y;
}event;

glapp* setVideoMode(int w, int h, int fullscreen);
void closeVideo();

void setWindowTitle(char* title);
void setMouse(int x, int y);

int  getTime();

void mainloop(glapp* app, keyboard* key, mouse* m,  int(idle)(void*), int(render)(event));

#endif
