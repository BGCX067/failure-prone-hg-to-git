#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <GL/gl.h>
#include <GL/glu.h>
//#include "../glapp.h"
#include "../math/matrix.h"
//#include "../util/fparray.h"
#include "vertexattribute.h"

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

typedef struct _samplerState{
	unsigned int id;
	int wrapmode;
	int anisotropy;
	int minfilter;
	int magfilter;
	//minlod, maxlod, lodbias, comparemode, comparefunc 
} SamplerState;

typedef struct _texture{
	unsigned int texid;
	SamplerState* state;
	int internalFormat;
	int target;

    char *id;
} Texture;

enum GeometryType{
	TRIANGLES = GL_TRIANGLES,
	POINTS = GL_POINTS
};

enum Type{
	UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
	FLOAT = GL_FLOAT
};

typedef struct _framebuffer{
	int width, height;
	unsigned int id;
	//unsigned int texid;
    Texture *tex;
    unsigned int depthid;
} Framebuffer;

enum Semantic{
	TIME,
	EYEPOS,
	MVP,
	MODEL,
	PROJECTION,
	VIEW,
	MODELVIEW,
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

}Uniform;

typedef struct _sampler{
	char* name;
	unsigned int index;
	unsigned int location;
}Sampler;

typedef struct _shader{
	Uniform** uniforms;
	unsigned int numUniforms;
	Sampler** samplers;
	int numSamplers;

    unsigned int progid;
}Shader;


typedef struct _renderer{

	int viewPortWidth, viewPortHeight;

	float fovy, znear, zfar;

	//int prevVBO;

	//int prevFramebuffer;
    //fparray* framebuffers; 
    
   
   	//int prevVAO;

}renderer;

//Primitivas mais high level do renderer FIXME: colocar em outro file?

typedef struct _point{
	int x, y;
}point;

typedef struct _rect{
	int x, y, w, h;
}rect;

renderer* initializeRenderer(int w, int h, float znear, float zfar, float fovy);

//int render(float ifps, event *e, Scene* s);
void begin2d();
void end2d();

void disableDepth();
void enableDepth();

VertexAttribute** initializeVertexFormat();
void setVertexAttribute(VertexAttribute** attr, int type, unsigned int count, unsigned int size, unsigned int offset, unsigned int comp, unsigned int vboid);

/////////////////////////
// MATRICES
//////////////////
void setModel(mat4 m );
void setView(mat4 m);
void setProjection(mat4 m);

void getModel(mat4 m);
void getView(mat4 m);
void getProjection(mat4 m);

//////////
//TEXTURE SAMPLERS
/////////
//TODO destroy sampler
SamplerState* initializeSamplerState(int wrapmode, int minfilter, int magfilter, int anisotropy);
void bindSamplerState(SamplerState* s, unsigned int slot);

//////////////////////
//TEXTURES OBJECTS
/////////////////////
// TODO destroy texture
Texture* initialize2DTexture(char *filename);
Texture* initializeTexture(char* filename, int target, int imageFormat, int internalFormat, int type);
Texture* initializeTextureFromMemory(void* data, int x, int y, int target, int imageFormat, int internalFormat, int type);
void bindTexture(Texture* t, unsigned int slot);
void destroyTexture(Texture* t);

//framebuffers
Framebuffer* initializeFramebuffer(int width, int height);
void bindFramebuffer(Framebuffer *fb);
/*void bindMainFramebuffer();
void bindFramebuffer(int id);
unsigned int initializeFramebuffer(void* data, int width, int height, int format, int internalFormat, int type );*/


/////////////////////
//VBOS
/////////////////////
unsigned int initializeVBO(unsigned int size, int mode, const void* data);
void* mapVBO(unsigned int id,  int mode);
void unmapVBO(unsigned int id);
void destroyVBO(unsigned int id);

/////////////////////
//VAO
////////////////////
void drawArraysVAO(unsigned int vaoID, int type, int numVerts);
void drawIndexedVAO(unsigned int  vaoID,  unsigned int triCount, int geometryType);
unsigned int initEmptyVAO();
void configureVAO(unsigned int vaoid, VertexAttribute** attrs);
void configureIndexedVAO(unsigned int vaoid, unsigned int indicesid, VertexAttribute** attrs);
void destroyVAO(unsigned int vaoid);


//shaders
Shader* initializeShader(const char* vertexSource, const char* fragmentSource);
void bindShader(Shader* program);
int printShaderCompilerLog(unsigned int shader);
int printShaderLinkerLog(unsigned int program);
void setShaderConstant1i(Shader* s, const char *name, const int constant);
void setShaderConstant1f(Shader* s, const char *name, const float constant);
void setShaderConstant2f(Shader* s, const char *name, const float constant[]);
void setShaderConstant3f(Shader* s, const char *name, const float constant[]);
void setShaderConstant4f(Shader* s, const char *name, const float constant[]);
void setShaderConstant3x3f(Shader* s, const char *name, const float constant[]);
void setShaderConstant4x4f(Shader* s, const char *name, const float constant[]);
/*void setShaderConstantArray1f(const char *name, const float *constant, const uint count);
void setShaderConstantArray2f(const char *name, const vec2  *constant, const uint count);
void setShaderConstantArray3f(const char *name, const vec3  *constant, const uint count);
void setShaderConstantArray4f(const char *name, const vec4  *constant, const uint count);
void setShaderConstantArray4x4f(const char *name, const mat4 *constant, const uint count);*/
void setShaderConstantRaw(Shader* s, const char *name, const void *data, const int size);

/////////
// Util
/////////
void printGPUMemoryInfo();


#endif
