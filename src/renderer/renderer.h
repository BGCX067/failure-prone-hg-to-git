#ifndef _RENDERER_H_
#define _RENDERER_H_

#ifdef __APPLE__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif
#include "../math/matrix.h"
#include "vertexattribute.h"

enum TextureTarget{
	TEXTURE_1D = GL_TEXTURE_1D,
	TEXTURE_2D = GL_TEXTURE_2D,
	TEXTURE_3D = GL_TEXTURE_3D,
	TEXTURE_CUBEMAP = GL_TEXTURE_CUBE_MAP,
	TEXTURE_RECTANGLE// = GL_TEXTURE_RECTANGLE_ARB

};

enum TextureFormat{
	RGB = GL_RGB,
	RGBA = GL_RGBA
};

enum TextureInternalFormat{
	RGB8 = GL_RGB8,
	RGBA8 = GL_RGBA8,
	RGB_DXT1, //= GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
	RGBA_DXT1, //= GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
	RGBA_DXT3, //= GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
	RGBA_DXT5 //= GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
};


enum TextureSampler{
	CLAMP  = GL_CLAMP,
	CLAMP_TO_EDGE  = GL_CLAMP_TO_EDGE,
    CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
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
	FLOAT = GL_FLOAT,
    INT = GL_INT
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
    INVMODELVIEW,
	NORMALMATRIX,
	LIGHTPOS
};

typedef struct _uniform{

	int location;
	char* name;
	//unsigned char* data;
    void *data;
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

}Renderer;

//Primitivas mais high level do renderer FIXME: colocar em outro file?

typedef struct _point{
	int x, y;
}Point;

typedef struct _rect{
	int x, y, w, h;
}Rect;

Renderer* InitializeRenderer(int w, int h, float znear, float zfar, float fovy);

//int render(float ifps, event *e, Scene* s);
void Begin2d();
void End2d();

void DisableDepth();
void EnableDepth();

VertexAttribute** InitializeVertexFormat();
void SetVertexAttribute(VertexAttribute** attr, int type, unsigned int count, unsigned int size, unsigned int offset, unsigned int comp, unsigned int vboid);

/////////////////////////
// MATRICES
//////////////////
void SetModel(mat4 m );
void SetView(mat4 m);
void SetProjection(mat4 m);

void GetModel(mat4 m);
void GetView(mat4 m);
void GetProjection(mat4 m);

void SetEyepos(vec3 pos);
//////////
//TEXTURE SAMPLERS
/////////
//TODO destroy sampler
SamplerState* InitializeSamplerState(int wrapmode, int minfilter, int magfilter, int anisotropy);
void BindSamplerState(SamplerState* s, unsigned int slot);

//////////////////////
//TEXTURES OBJECTS
/////////////////////
// TODO destroy texture
Texture* Initialize2DTexture(char *filename);
Texture* InitializeTexture(char* filename, int target, int imageFormat, int internalFormat, int type);
Texture* InitializeTextureFromMemory(void* data, int x, int y, int target, int imageFormat, int internalFormat, int type);
Texture* Initialize3DTexture(void* data, int xdim, int ydim, int zdim,int imageFormat, int internalFormat, int type);
void BindTexture(Texture* t, unsigned int slot);
void DestroyTexture(Texture* t);

//framebuffers
Framebuffer* InitializeFramebufferDepth(int width, int height);
Framebuffer* InitializeFramebufferColor(int width, int height);
void BindFramebuffer(Framebuffer *fb);
/*void bindMainFramebuffer();
void bindFramebuffer(int id);
unsigned int initializeFramebuffer(void* data, int width, int height, int format, int internalFormat, int type );*/


/////////////////////
//VBOS
/////////////////////
unsigned int InitializeVBO(unsigned int size, int mode, const void* data);
void* MapVBO(unsigned int id,  int mode);
void UnmapVBO(unsigned int id);
void DestroyVBO(unsigned int id);

/////////////////////
//VAO
////////////////////
void DrawArraysVAO(unsigned int vaoID, int type, int numVerts);
void DrawIndexedVAO(unsigned int  vaoID,  unsigned int triCount, int geometryType);
unsigned int InitEmptyVAO();
void ConfigureVAO(unsigned int vaoid, VertexAttribute** attrs);
void ConfigureIndexedVAO(unsigned int vaoid, unsigned int indicesid, VertexAttribute** attrs);
void DestroyVAO(unsigned int vaoid);


//shaders
Shader* InitializeShader(const char *geometrysource, const char* vertexsource, const char* fragmentsource);
void BindShader(Shader* program);
int PrintShaderCompilerLog(unsigned int shader);
int PrintShaderLinkerLog(unsigned int program);
void SetShaderConstant1i(Shader* s, const char *name, const int constant);
void SetShaderConstant1f(Shader* s, const char *name, const float constant);
void SetShaderConstant2f(Shader* s, const char *name, const float constant[]);
void SetShaderConstant3f(Shader* s, const char *name, const float constant[]);
void SetShaderConstant4f(Shader* s, const char *name, const float constant[]);
void SetShaderConstant3x3f(Shader* s, const char *name, const float constant[]);
void SetShaderConstant4x4f(Shader* s, const char *name, const float constant[]);
/*void setShaderConstantArray1f(const char *name, const float *constant, const uint count);
void setShaderConstantArray2f(const char *name, const vec2  *constant, const uint count);
void setShaderConstantArray3f(const char *name, const vec3  *constant, const uint count);
void setShaderConstantArray4f(const char *name, const vec4  *constant, const uint count);
void setShaderConstantArray4x4f(const char *name, const mat4 *constant, const uint count);*/
void SetShaderConstantRaw(Shader* s, const char *name, const void *data, const int size);

/////////
// Util
/////////
void PrintGPUMemoryInfo();
void Screenshot(char* filename);

#endif
