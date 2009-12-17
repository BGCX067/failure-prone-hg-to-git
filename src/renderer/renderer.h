
#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <GL/gl.h>
#include <GL/glu.h>

#define MAX_TEXTURES 256;

typedef struct _renderer{
	float fovy, znear, zfar;

	int prevTexture;

}renderer;

renderer* initializeRenderer(int w, int h, float znear, float zfar, float fovy);

int render(void* data);

unsigned int initializeTexture(char* filename, int target, int imageFormat, int  internalFormat, int type, int flags);
void bindTexture(int slot, int id);

unsigned int initializeVBO(int size, const void* data);

#endif
