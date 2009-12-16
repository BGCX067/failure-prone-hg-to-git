
#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <GL/gl.h>
#include <GL/glu.h>

typedef struct _renderer{
	float fovy, znear, zfar;
}renderer;

renderer* initializeRenderer(int w, int h, float znear, float zfar, float fovy);

int render(void* data);

#endif
