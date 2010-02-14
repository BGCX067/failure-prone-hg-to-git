
#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <GL/gl.h>
#include <GL/glu.h>
#include "../glapp.h"
#include "../util/fplist.h"

#define MAX_VERTEX_ATTRS 16
#define MAX_TEXTURES 256
#define MAX_VERTEX_FORMAT 256
#define MAX_SHADERS 256
#define MAX_FRAMEBUFFERS 10

enum ConstantType {
	CONSTANT_FLOAT,
	CONSTANT_VEC2,
	CONSTANT_VEC3,
	CONSTANT_VEC4,
	CONSTANT_INT,
	CONSTANT_IVEC2,
	CONSTANT_IVEC3,
	CONSTANT_IVEC4,
	CONSTANT_BOOL,
	CONSTANT_BVEC2,
	CONSTANT_BVEC3,
	CONSTANT_BVEC4,
	CONSTANT_MAT2,
	CONSTANT_MAT3,
	CONSTANT_MAT4,

	CONSTANT_TYPE_COUNT
};

enum Semantic{
	TIME,
	EYEPOS,
	LIGHTPOS
};

typedef struct _uniform{

	int location;
	char* name;
	unsigned char* data;
	int size;
	int type;
	int semantic;
	short int dirty;

}uniform;

typedef struct _sampler{
	char* name;
	unsigned int index;
	unsigned int location;
}sampler;

typedef struct _shader{
	uniform** uniforms;
	int numUniforms;
	sampler** samplers;
	int numSamplers;
}shader;

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

	LINEAR = 1 << 3,
	BILINEAR = 1 << 4,
	NEAREST = 1 << 5,

	ANISOTROPY_1 = 1 << 6,
	ANISOTROPY_2 = 1 << 7,
	ANISOTROPY_4 = 1 << 8,
	ANISOTROPY_8 = 1 << 9,
	ANISOTROPY_16= 1 << 10,

	MIPMAP  = 1<< 17
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

//void* createVertexFormat();
void destroyVertexFormat(void *ptr);

typedef struct _framebuffer{
	int width, height;
	int id;
} framebuffer;

typedef struct _renderer{

	int viewPortWidth, viewPortHeight;

	float fovy, znear, zfar;

	int prevTexture;
	texture* textures[MAX_TEXTURES];

	int prevVBO;
	int prevFormat;
	//vertexFormat* vertexFormats[MAX_VERTEX_FORMAT];
    fplist *vertexFormats;

	int prevShader;
	shader* shaders[MAX_SHADERS];
	void *uniformFuncs[CONSTANT_TYPE_COUNT];

	int prevFramebuffer;
	framebuffer* framebuffers[MAX_FRAMEBUFFERS];

}renderer;

//Primitivas mais high level do renderer FIXME: colocar em outro file?

typedef struct _point{
	int x, y;
}point;

typedef struct _rect{
	int x, y, w, h;
}rect;



renderer* initializeRenderer(int w, int h, float znear, float zfar, float fovy);

int render(event *e);

//texturas
unsigned int initializeTextureFromMemory(void* data, int x, int  y, int  target, int imageFormat, int internalFormat, int type, int flags);
unsigned int initializeTexture(char* filename, int target, int imageFormat, int internalFormat, int type, int flags);
void bindTexture(int slot, int id);

//framebuffers
void bindMainFramebuffer();
void bindFramebuffer(int id);
unsigned int initializeFramebuffer(void* data, int width, int height, int format, int internalFormat, int type, int flags );

//VBOS
unsigned int initializeVBO(unsigned int size, const void* data);
void killVBO(unsigned int id);
void drawVBO(unsigned int triCount, unsigned int vertexId, unsigned int indicesID,  int fmt);

unsigned int addVertexFormat(vertexAttribute** attrs, unsigned int num);

//shaders
unsigned int initializeShader(const char* vertexSource, const char* fragmentSource);
void bindShader(unsigned int program);
int printShaderCompilerLog(unsigned int shader);
int printShaderLinkerLog(unsigned int program);
void setShaderConstant1i(int shaderid, const char *name, const int constant);
void setShaderConstant1f(int shaderid, const char *name, const float constant);
void setShaderConstant2f(int shaderid, const char *name, const float constant[]);
void setShaderConstant3f(int shaderid, const char *name, const float constant[]);
void setShaderConstant4f(int shaderid, const char *name, const float constant[]);
/*void setShaderConstant4x4f(const char *name, const mat4 &constant);
void setShaderConstantArray1f(const char *name, const float *constant, const uint count);
void setShaderConstantArray2f(const char *name, const vec2  *constant, const uint count);
void setShaderConstantArray3f(const char *name, const vec3  *constant, const uint count);
void setShaderConstantArray4f(const char *name, const vec4  *constant, const uint count);
void setShaderConstantArray4x4f(const char *name, const mat4 *constant, const uint count);*/
void setShaderConstantRaw(int shaderid, const char *name, const void *data, const int size);

#endif
