
#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <GL/gl.h>
#include <GL/glu.h>
#include "../glapp.h"
#include "../util/fplist.h"
#include "../util/fparray.h"
#include "camera.h"

#define MAX_VERTEX_ATTRS 16

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
	RGBA8 = GL_RGBA8,
	RGB_DXT1 = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
	RGBA_DXT1 = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
	RGBA_DXT3 = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
	RGBA_DXT5 = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
};


enum TextureSampler{
	CLAMP  = GL_CLAMP,
	CLAMP_TO_EDGE  = GL_CLAMP_TO_EDGE,

	LINEAR = GL_LINEAR,
	BILINEAR = GL_LINEAR_MIPMAP_LINEAR,
	NEAREST = GL_NEAREST
};

enum GeometryType{

	TRIANGLES = GL_TRIANGLES,
	POINTS = GL_POINTS
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

typedef struct _vertexAttribute{
	unsigned int count;
	unsigned int size;
	unsigned int offset;
	unsigned int components;
	int type;
	unsigned int vboID; //opcional
}vertexAttribute;

typedef struct _framebuffer{
	int width, height;
	int id;
	int texid;
} framebuffer;

typedef struct _renderer{

	int viewPortWidth, viewPortHeight;

	float fovy, znear, zfar;

	int prevVBO;

	int prevFramebuffer;
    	fparray* framebuffers; 
    
   
   	int prevVAO;

}renderer;

//Primitivas mais high level do renderer FIXME: colocar em outro file?

typedef struct _point{
	int x, y;
}point;

typedef struct _rect{
	int x, y, w, h;
}rect;

renderer* initializeRenderer(int w, int h, float znear, float zfar, float fovy, CameraType t);

int render(float ifps, event *e, Scene* s);
void begin2d();
void end2d();

void disableDepth();
void enableDepth();

vertexAttribute** initializeVertexFormat();

//textures
int initializeSamplerState(int wrapmode, int minfilter, int magfilter, int anisotropy);
unsigned int initializeTexture(char* filename, int target, int imageFormat, int internalFormat, int type);
void bindSamplerState(unsigned int unit, unsigned int id);
unsigned int initializeTextureFromMemory(void* data, int x, int y, int target, int imageFormat, int internalFormat, int type);
void bindTexture(int slot, int id);

//framebuffers
void bindMainFramebuffer();
void bindFramebuffer(int id);
unsigned int initializeFramebuffer(void* data, int width, int height, int format, int internalFormat, int type );

//VBOS
unsigned int initializeVBO(unsigned int size, int mode, const void* data);
void killVBO(unsigned int id);
void* mapVBO(unsigned int id,  int mode);
void unmapVBO(unsigned int id);

//VAO
//passando o vaoid, o formato dos vertices attrs e os vbos, essa funcao configura a vao
void initializeVAO(unsigned int vaoID, vertexAttribute** attrs); 
//cria uma vaoid vazia e retorna
unsigned int createVAO();
void drawArraysVAO(unsigned int vaoID, int type, int numVerts);

//parecida com a initializeVAO mas essa eh pra geometria indexada, entao passa os indicesID
unsigned int initializeIndexedVAO( unsigned int indicesID, vertexAttribute** attrs);
unsigned int  drawIndexedVAO(unsigned int  vaoID,  unsigned int triCount, int geometryType);

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
void setShaderConstant4x4f(int shaderid, const char *name, const float constant[]);
/*void setShaderConstantArray1f(const char *name, const float *constant, const uint count);
void setShaderConstantArray2f(const char *name, const vec2  *constant, const uint count);
void setShaderConstantArray3f(const char *name, const vec3  *constant, const uint count);
void setShaderConstantArray4f(const char *name, const vec4  *constant, const uint count);
void setShaderConstantArray4x4f(const char *name, const mat4 *constant, const uint count);*/
void setShaderConstantRaw(int shaderid, const char *name, const void *data, const int size);


//FIXME remover essa porcaria
Camera* getcamera();

#endif
