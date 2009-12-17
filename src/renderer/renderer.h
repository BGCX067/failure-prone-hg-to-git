
#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <GL/gl.h>
#include <GL/glu.h>

#define MAX_VERTEX_ATTRS 16
#define MAX_TEXTURES 256
#define MAX_VERTEX_FORMAT 256

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

enum AttributeType{
	ATTR_VERTEX = 0,
	ATTR_NORMAL = 2,
	ATTR_COLOR = 3,
	ATTR_SECONDARY_COLOR = 4,
	ATTR_FOG_COORD = 5,

	ATTR_TANGENT = 6,
	ATTR_BINORMAL = 7,

	ATTR_TEXCOORD0 = 8,
	ATTR_TEXCOORD1 = 9,
	ATTR_TEXCOORD2 = 10,
	ATTR_TEXCOORD3 = 11,
	ATTR_TEXCOORD4 = 12,
	ATTR_TEXCOORD5 = 13,
	ATTR_TEXCOORD6 = 14,
	ATTR_TEXCOORD7 = 15

};

typedef struct _texture{
	unsigned int id;
	int flags;
	int target;
}texture;

typedef struct _vertexAttribute{
	unsigned int count;
	unsigned int size;
	unsigned int offset;
	unsigned int components;
	int type;
}vertexAttribute;

typedef struct _vertexFormat{
	vertexAttribute** attributes;
	short int numAttrs;
}vertexFormat;

typedef struct _renderer{
	float fovy, znear, zfar;

	int prevTexture;
	texture* textures[MAX_TEXTURES];

	int prevVBO;
	int prevFormat;
	vertexFormat* vertexFormats[MAX_VERTEX_FORMAT];

}renderer;

renderer* initializeRenderer(int w, int h, float znear, float zfar, float fovy);

int render(void* data);

unsigned int initializeTexture(char* filename, int target, int imageFormat, int  internalFormat, int type, int flags);
void bindTexture(int slot, int id);
void setTextureFilter(int target, int filter);

unsigned int initializeVBO(unsigned int size, const void* data);
void killVBO(unsigned int id);
void drawVBO(unsigned int triCount, unsigned int vertexId, unsigned int indicesID,  int fmt);

unsigned int addVertexFormat(vertexAttribute** attrs, unsigned int num);



#endif
