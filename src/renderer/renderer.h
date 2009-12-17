
#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <GL/gl.h>
#include <GL/glu.h>

#define MAX_TEXTURES 256

enum TextureTarget{

	TEXTURE_1D = GL_TEXTURE_1D,
	TEXTURE_2D = GL_TEXTURE_2D,
	TEXTURE_3D = GL_TEXTURE_3D,
	TEXTURE_CUBEMAP = GL_TEXTURE_CUBE_MAP,
	TEXTURE_RECTANGLE = GL_TEXTURE_RECTANGLE_ARB
};

enum TextureFormat{
	RGB = GL_RGB,
	RGBA = GL_RGBA
};

enum TextureInternalFormat{
	RGB8 = GL_RGB8,
	RGBA8 = GL_RGBA8
};

enum TextureFlags{
	CLAMP  = 1 << 1,
	CLAMP_TO_EDGE  = 1 << 2,
	
	MIPMAP  = 1<< 17
};

enum TextureFilter{

	LINEAR = 1 << 2,
	BILINEAR = 1 << 3,
	NEAREST = 1 << 4,

	ANISOTROPY_1 = 1 << 5,
	ANISOTROPY_2 = 1 << 6,
	ANISOTROPY_4 = 1 << 7,
	ANISOTROPY_8 = 1 << 8,
	ANISOTROPY_16= 1 << 9
};

enum Type{
	UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
	FLOAT = GL_FLOAT
};

typedef struct _texture{
	unsigned int id;
	int flags;
	int target;
}texture;

typedef struct _renderer{
	float fovy, znear, zfar;

	int prevTexture;
	texture* textures[MAX_TEXTURES];

}renderer;

renderer* initializeRenderer(int w, int h, float znear, float zfar, float fovy);

int render(void* data);

unsigned int initializeTexture(char* filename, int target, int imageFormat, int  internalFormat, int type, int flags);
void bindTexture(int slot, int id);
void setTextureFilter(int target, int filter);

unsigned int initializeVBO(int size, const void* data);

#endif
