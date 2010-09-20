#include "../util/GLee.h"
#include "renderer.h"
#include "camera.h"
#include "scene.h"
#include "math/matrix.h"
#include "../glapp.h"
#include "../util/image.h"
#include "../util/malloc.h"
#include "../util/textfile.h"
#include "mesh.h"
#include "../util/shadergen.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include "../util/procedural/terrain.h"
#include "glime.h"

typedef void (APIENTRYP PFNGLGENSAMPLERSPROC) (GLsizei count, GLuint *samplers);
typedef void (APIENTRYP PFNGLDELETESAMPLERSPROC) (GLsizei count, const GLuint *samplers);
typedef GLboolean (APIENTRYP PFNGLISSAMPLERPROC) (GLuint sampler);
typedef void (APIENTRYP PFNGLBINDSAMPLERPROC) (GLenum unit, GLuint sampler);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIPROC) (GLuint sampler, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIVPROC) (GLuint sampler, GLenum pname, const GLint *param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERFPROC) (GLuint sampler, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERFVPROC) (GLuint sampler, GLenum pname, const GLfloat *param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIIVPROC) (GLuint sampler, GLenum pname, const GLint *param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIUIVPROC) (GLuint sampler, GLenum pname, const GLuint *param);
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIVPROC) (GLuint sampler, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIIVPROC) (GLuint sampler, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERFVPROC) (GLuint sampler, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIFVPROC) (GLuint sampler, GLenum pname, GLfloat *params);
														

PFNGLGENSAMPLERSPROC glGenSamplers = NULL;
PFNGLDELETESAMPLERSPROC glDeleteSamplers = NULL;
PFNGLISSAMPLERPROC glIsSampler = NULL;
PFNGLBINDSAMPLERPROC glBindSampler = NULL;
PFNGLSAMPLERPARAMETERIPROC  glSamplerParameteri = NULL;
PFNGLSAMPLERPARAMETERFPROC  glSamplerParameterf = NULL;
PFNGLSAMPLERPARAMETERIVPROC glSamplerParameteriv = NULL;
PFNGLSAMPLERPARAMETERFVPROC glSamplerParameterfv = NULL;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
typedef GLvoid (APIENTRY *UNIFORM_FUNC)(GLint location, GLsizei count, const void *value);
typedef GLvoid (APIENTRY *UNIFORM_MAT_FUNC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void *uniformFuncs[CONSTANT_TYPE_COUNT];

int getConstantType(GLenum type){
	switch (type){
		case GL_FLOAT:          return CONSTANT_FLOAT;
		case GL_FLOAT_VEC2: return CONSTANT_VEC2;
		case GL_FLOAT_VEC3: return CONSTANT_VEC3;
		case GL_FLOAT_VEC4: return CONSTANT_VEC4;
		case GL_INT:            return CONSTANT_INT;
		case GL_INT_VEC2:   return CONSTANT_IVEC2;
		case GL_INT_VEC3:   return CONSTANT_IVEC3;
		case GL_INT_VEC4:   return CONSTANT_IVEC4;
		case GL_BOOL:       return CONSTANT_BOOL;
		case GL_BOOL_VEC2:  return CONSTANT_BVEC2;
		case GL_BOOL_VEC3:  return CONSTANT_BVEC3;
		case GL_BOOL_VEC4:  return CONSTANT_BVEC4;
		case GL_FLOAT_MAT2: return CONSTANT_MAT2;
		case GL_FLOAT_MAT3: return CONSTANT_MAT3;
		case GL_FLOAT_MAT4: return CONSTANT_MAT4;
	}

	return  -1;
}

int constantTypeSizes[CONSTANT_TYPE_COUNT] = {
	sizeof(float),
	sizeof(float) *2,
	sizeof(float) *3,
	sizeof(float) * 4,
	sizeof(int),
	sizeof(int) * 2,
	sizeof(int) * 3,
	sizeof(int) * 4,
	sizeof(int),
	sizeof(int) * 2,
	sizeof(int) * 3,
	sizeof(int) * 4,
	sizeof(float) *4,
	sizeof(float) *9,
	sizeof(float) * 16,
};

renderer* r;
camera c;
batch* cube;
batch* quad;

mat4 projection;
mat4 modelview;
mat4 mvp;

int samplerstate;
unsigned int testShader; 
unsigned int tex;

float elapsedTime;

void beginRender(event *e) { 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	cameraHandleEvent(&c, e);
	setupViewMatrix(&c);

    //FIXME perspective divide não está sendo feito em canto nenhum
    //FIXME multiplicação mais rápida na gpu? tentar montar MVP na gpu passando modelview e projection apenas
    //FIXME tentar usar 2 uniforms do tipo mat4 dá erro
    //FIXME para correção de perspectiva (substituir aqueles GL_PERSPECTIVE_CORRECTION_HINT)
	fpMultMatrix(c.mvp, c.projection, c.modelview);

	fpMultMatrix(mvp, projection, modelview);
	
}

int render(float ifps, event *e, scene *s){
	beginRender(e);
	elapsedTime += ifps;
	bindShader(testShader);
	bindSamplerState(0,  samplerstate);
	bindTexture(0, tex);
	//draw(cube);
	draw(quad);
    	glFinish();
	glFlush();
}

renderer* initializeRenderer(int w, int h, float znear, float zfar, float fovy){
	r = (renderer*) dlmalloc(sizeof(renderer));
   	r->textures = fparray_init(NULL, NULL, sizeof(texture));
    	r->shaders = fparray_init(NULL, NULL, sizeof(shader));
    	r->framebuffers = fparray_init(NULL, dlfree, sizeof(framebuffer));
	r->samplerStates = fparray_init(NULL, dlfree, sizeof(samplerState));
	r->fovy = fovy;
	r->zfar = zfar;
	r->znear = znear;
	r->prevTexture = -1;
	r->prevVBO = -1;
	r->prevVAO = 0;
	r->prevSamplerState = -1;
	r->prevShader = 0;
	r->viewPortWidth = w;
	r->viewPortHeight = h;
	r->prevFramebuffer = -1;

	//FIXME  gambi feia, o glee so inicializa as fncoes quando sao chamadas
	//mas o ponteiro pra funcao uniformFuncs nao chama elas, logo nao inicializa
	//logo nao fucionam
	float foo[4];
	float foo2[3];
	glUniform4fv(99, 0, foo);
	glUniform3fv(99, 0, foo2);
	uniformFuncs[CONSTANT_FLOAT] = (void *) glUniform1fv;
	uniformFuncs[CONSTANT_VEC2]  = (void *) glUniform2fv;
	uniformFuncs[CONSTANT_VEC3]  = (void *) glUniform3fv;
	uniformFuncs[CONSTANT_VEC4]  = (void *) glUniform4fv;
	uniformFuncs[CONSTANT_INT]   = (void *) glUniform1iv;
	uniformFuncs[CONSTANT_IVEC2] = (void *) glUniform2iv;
	uniformFuncs[CONSTANT_IVEC3] = (void *) glUniform3iv;
	uniformFuncs[CONSTANT_IVEC4] = (void *) glUniform4iv;
	uniformFuncs[CONSTANT_BOOL]  = (void *) glUniform1iv;
	uniformFuncs[CONSTANT_BVEC2] = (void *) glUniform2iv;
	uniformFuncs[CONSTANT_BVEC3] = (void *) glUniform3iv;
	uniformFuncs[CONSTANT_BVEC4] = (void *) glUniform4iv;
	uniformFuncs[CONSTANT_MAT2]  = (void *) glUniformMatrix2fv;
	uniformFuncs[CONSTANT_MAT3]  = (void *) glUniformMatrix3fv;
	uniformFuncs[CONSTANT_MAT4]  = (void *) glUniformMatrix4fv;



	glGenSamplers = (PFNGLGENSAMPLERSPROC)glXGetProcAddress("glGenSamplers");
        glDeleteSamplers = (PFNGLDELETESAMPLERSPROC)glXGetProcAddress("glDeleteSamplers");
	glIsSampler = (PFNGLISSAMPLERPROC)glXGetProcAddress("glIsSampler");
	glBindSampler = (PFNGLBINDSAMPLERPROC)glXGetProcAddress("glBindSampler");
	glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC)glXGetProcAddress("glSamplerParameteri");
	glSamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC)glXGetProcAddress("glSamplerParameteriv");
	glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC)glXGetProcAddress("glSamplerParameterf");
	glSamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC)glXGetProcAddress("glSamplerParameterfv");
	//glSamplerParameterIiv = (PFNGLSAMPLERPARAMETERIIVPROC)glXGetProcAddress("glSamplerParameterIiv");
	//glSamplerParameterIuiv = (PFNGLSAMPLERPARAMETERIUIVPROC)glXGetProcAddress("glSamplerParameterIuiv");


	float ratio = (float) w / (float) h;
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, w, h);

	glClearColor( 0.7, 0.7, 0.7, 1.0 );
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);

	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableClientState(GL_VERTEX_ARRAY);

 	initCamera(&c);
	c.viewDir[0] = 0.506;
	c.viewDir[1] = -0.624;
	c.viewDir[2] = 0.595;
	c.pos[0] = 46.211;
	c.pos[1] = 171.948;
	c.pos[2] = 33.311;
	c.up[0] = 0.404;
	c.up[1] = 0.781;
	c.up[2] = 0.475;
	fpperspective(c.projection, fovy, ratio, znear, zfar);
	fpOrtho(projection, 0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);	
	fpIdentity(modelview);
    
	tex = initializeTexture("data/textures/tex3.jpg", TEXTURE_2D, RGB, RGB_DXT1, UNSIGNED_BYTE,  (CLAMP_TO_EDGE));
    
    	testShader = initializeShader( readTextFile("data/shaders/minimal.vert"), readTextFile("data/shaders/deform.frag") );
    
    	samplerstate = initializeSamplerState(CLAMP, LINEAR, LINEAR, 0);
	//cube = makeCube(100);
	quad = malloc(sizeof(batch));
	initializeBatch(quad);
	begin(quad, GL_TRIANGLE_STRIP, 4, 1);
		texCoord2f(quad, 0, 0.0, 0.0);
		normal3f(quad, 0.0, 0.0, 0.0);
		vertex3f(quad, 0.0, 0.0, 0.0);

		texCoord2f(quad, 0, 1.0, 0.0);
		normal3f(quad, 1.0, 0.0, 0.0);
		vertex3f(quad, 800.0, 0.0, 0.0);

		texCoord2f(quad, 0, 0.0, 1.0);
		normal3f(quad, 0.0, 1.0, 0.0);
		vertex3f(quad, 0.0, 600.0, 0.0);

		texCoord2f(quad, 0, 1.0, 1.0);
		normal3f(quad, 1.0, 1.0, 0.0);
		vertex3f(quad, 800.0, 600.0, 0.0);
	end(quad);


	printf("Renderer inicializado.\n");

	return r;
}

void begin2d(){

	glPushAttrib( GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glDisable(GL_STENCIL_TEST);
	glStencilMask( 0 );
	disableDepth();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

	glMatrixMode( GL_MODELVIEW );
        glPushMatrix();
        glLoadIdentity();

	//coloca no modo ortho
	//glMatrixMode( GL_PROJECTION );
    	//glPushMatrix();
        //glLoadIdentity();
	//TODO info da GUI na struct
	gluOrtho2D( 0, r->viewPortWidth, 0, r->viewPortHeight);
}

void end2d(){
	glPopAttrib();
	enableDepth();
	glMatrixMode( GL_PROJECTION);
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW);
	glPopMatrix();
}

void enableDepth(){
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

void disableDepth(){
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
}


int initializeSamplerState(int wrapmode, int minfilter, int magfilter, int anisotropy){

	//se ja tem um state igual  retorna o id dele
	for(int i = 0; i < r->samplerStates->size; i++){
		samplerState* state = fparray_getdata(i, r->samplerStates);
		if ( (state->minfilter == minfilter) && (state->wrapmode == wrapmode) && (state->wrapmode == wrapmode) && (state->anisotropy == anisotropy)){
			return state->id;
		}
	}

	//se nao cria um
	unsigned int samplerID;
	glGenSamplers(1, &samplerID);
	glSamplerParameteri(samplerID, GL_TEXTURE_MIN_FILTER, minfilter);
	glSamplerParameteri(samplerID, GL_TEXTURE_MAG_FILTER, magfilter);
	glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_S, wrapmode);
	glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_T, wrapmode);
	glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_R, wrapmode);
    	float someVec[4];
    	someVec[0] = 0.0;
    	someVec[1] = 0.0;
    	someVec[2] = 0.0;
    	someVec[3] = 0.0;
	//TODO passar por parametro esses outros valores ou deixar na struct renderer
	glSamplerParameterfv(samplerID, GL_TEXTURE_BORDER_COLOR, someVec);
	glSamplerParameterf(samplerID, GL_TEXTURE_MIN_LOD, -1000.f);
	glSamplerParameterf(samplerID, GL_TEXTURE_MAX_LOD, 1000.f);
	glSamplerParameterf(samplerID, GL_TEXTURE_LOD_BIAS, 0.0f);
	glSamplerParameteri(samplerID, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glSamplerParameteri(samplerID, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	//glSamplerParameteri(samplerID, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
	
	samplerState* state = (samplerState*) dlmalloc(sizeof(samplerState));
	state->id = samplerID;
	state->minfilter = minfilter;
	state->magfilter = magfilter;
	state->wrapmode = wrapmode;
	state->anisotropy = anisotropy;

	fparray_inspos(state, state->id, r->samplerStates);

	return state->id;
}

void bindSamplerState(unsigned int unit, unsigned int id){
	samplerState* samplerid = fparray_getdata(id, r->samplerStates);
	samplerState* prevSampler = fparray_getdata(r->prevSamplerState, r->samplerStates);

	if (r->prevSamplerState == -1){
		r->prevSamplerState = id;
		glBindSampler(unit, samplerid->id);
	}else{
		if (samplerid->id != prevSampler->id){
			glBindSampler(unit, samplerid->id);
			r->prevSamplerState = id;
		}

	}
}

unsigned int initializeTexture(char* filename, int target, int imageFormat, int internalFormat, int type, int flags){
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(target, textureID);

    if ( filename != NULL ){
        //TODO fazer 1d e 3d
        int x, y, n;
        unsigned char* data = NULL;
        if ((target == TEXTURE_2D) || (target == TEXTURE_RECTANGLE) ){
            data = stbi_load(filename, &x, &y, &n, 0);
            if (!data){
                printf("Error loading: %s texture. \n", filename );
                return 0;
            }
            glTexImage2D(target, 0, internalFormat, x, y, 0, imageFormat, type, data);
        }else if ( target == TEXTURE_CUBEMAP ){
            char buff[1024];
            GLuint facetargets[] = {
                GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
            };
            char *facenames[] = {"posx", "negx", "posy", "negy", "posz", "negz" };
            for (int i = 0; i < 6; i++){
                sprintf(buff, filename, facenames[i]);
                buff[strlen(filename)+5] = '\0';
                data = stbi_load(buff, &x, &y, &n, 0);
                if (!data){
                    printf("Error loading: %s texture. \n", buff);
                    return 0;
                }
                glTexImage2D(facetargets[i], 0, internalFormat, x, y, 0, imageFormat, type, data);
                if (data){
                    stbi_image_free(data);
                    data = NULL;
                }
            }

        }
        stbi_image_free(data);
    }

    texture* tex = (texture*) dlmalloc(sizeof(texture));
    tex->id = textureID;
    tex->target = target;

    fparray_inspos(tex, textureID, r->textures);

    if (r->prevTexture >= 0)
        bindTexture(0, r->prevTexture);

    return textureID;
}

unsigned int initializeTextureFromMemory(void* data, int x, int y, int target, int imageFormat, int internalFormat, int type, int flags){


	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(target, textureID);

	if ((target == TEXTURE_2D) || (target == TEXTURE_RECTANGLE) ){
		glTexImage2D(target, 0, internalFormat, x, y, 0, imageFormat, type, data);
	}

	texture* tex = (texture*) dlmalloc(sizeof(texture));
	tex->id = textureID;
	tex->target = target;

    	fparray_inspos(tex, textureID, r->textures);

	if (r->prevTexture >= 0)
		bindTexture(0, r->prevTexture);

	return textureID;


}

void bindTexture(int slot, int id){

	texture* tex = fparray_getdata(id, r->textures);
	texture* prevTex = fparray_getdata(r->prevTexture, r->textures);

	if (r->prevTexture == -1){
		r->prevTexture = id;
		glEnable(tex->target );
		glActiveTexture(GL_TEXTURE0 + slot);
		//TODO nao precisa mais do enable quando usa shaders
		glBindTexture(tex->target, tex->id );

	}else{
		if (tex->target != prevTex->target){
			glDisable(prevTex->target);
			glEnable(tex->target);
		}
		if (tex->id != prevTex->id)
			glBindTexture(tex->target, tex->id);

		glActiveTexture(GL_TEXTURE0 + slot);
		r->prevTexture = id;
	}

}

void killVBO(unsigned int id){
	glDeleteBuffers(1, &id);
}

vertexAttribute** initializeVertexFormat(){
	vertexAttribute** attr = malloc(sizeof(vertexAttribute*)*MAX_VERTEX_ATTRS);
	for (int i = 0; i < MAX_VERTEX_ATTRS; i++)
		attr[i] = NULL;
	return attr;
}

void configureVAO(unsigned int vaoID,   vertexAttribute** attrs){
	glBindVertexArray(vaoID);
	for(unsigned int i = 0; i < MAX_VERTEX_ATTRS; i++){
		if (attrs[i]){
			glEnableVertexAttribArray(i);
			printf("configure vao id:  %d vboid %d \n", i, attrs[i]->vboID); 
			glBindBuffer(GL_ARRAY_BUFFER, attrs[i]->vboID);
			glVertexAttribPointer(i, attrs[i]->components, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(attrs[i]->offset));
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(r->prevVAO);
}

unsigned int createVAO(){
	unsigned int vaoID;
	glGenVertexArrays(1, &vaoID);
	return vaoID;
}

void drawArraysVAO(unsigned int vaoID, int type, int numVerts){

	if (vaoID != r->prevVAO){
		r->prevVAO = vaoID;
		glBindVertexArray(vaoID);
	}
	glBindVertexArray(vaoID);
	glDrawArrays(type, 0, numVerts);
}

unsigned int initializeIndexedVAO( unsigned int  indicesID, vertexAttribute** attrs,  unsigned int num){
	unsigned int vaoID;
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);
	
	for(unsigned int i = 0; i < MAX_VERTEX_ATTRS; i++){
		if (attrs[i]){
			glEnableVertexAttribArray(i);
			glBindBuffer(GL_ARRAY_BUFFER, attrs[i]->vboID);
			glVertexAttribPointer(i, attrs[i]->components, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(attrs[i]->offset));
		}
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,  indicesID);

	glBindVertexArray(r->prevVAO);
	return vaoID;
}

unsigned int drawIndexedVAO(unsigned int vaoID, unsigned int triCount, int geometryType){

	if (vaoID != r->prevVAO){
		r->prevVAO = vaoID;
		glBindVertexArray(vaoID);
	}
	glDrawElements(geometryType, triCount, GL_UNSIGNED_INT, NULL);
}

unsigned int initializeVBO(unsigned int size, int mode, const void* data){
	unsigned int vboID;
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, size, data, mode);
	glBindBuffer(GL_ARRAY_BUFFER, r->prevVBO); //volta
	printf("initialize vbo: %d\n", vboID);
	return vboID;
}

void* mapVBO(unsigned int vboID,int mode){
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	float* ret = (float*) glMapBuffer(GL_ARRAY_BUFFER, mode);
	glBindBuffer(GL_ARRAY_BUFFER, r->prevVBO);
	return ret;
}

void unmapVBO(unsigned int id){
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, r->prevVBO);
}

unsigned int initializeShader(const char* vertexSource, const char* fragmentSource){


	if (!vertexSource && !fragmentSource)
		return 0;

	int shaderProgram = glCreateProgram();
	if (!shaderProgram)
		return 0;

	int vertexShader;
	if (vertexSource){
		vertexShader = glCreateShader(GL_VERTEX_SHADER);

		if (vertexShader == 0)
			return 0;

		glShaderSource(vertexShader, 1, &vertexSource, 0);
		glCompileShader(vertexShader);

		if (printShaderCompilerLog(vertexShader)){
			glDeleteShader(vertexShader);
			return 0;
		}

		glAttachShader(shaderProgram, vertexShader);
	}

	int fragmentShader;
	if (fragmentSource){
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		if (fragmentShader == 0)
			return 0;

		glShaderSource(fragmentShader, 1, &fragmentSource, 0);
		glCompileShader(fragmentShader);

		if (printShaderCompilerLog(fragmentShader)){
			glDeleteShader(fragmentShader);
			return 0;
		}

		glAttachShader(shaderProgram, fragmentShader);
	}

	glLinkProgram(shaderProgram);
	if (printShaderLinkerLog(shaderProgram)){
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glDeleteProgram(shaderProgram);
		return 0;
	}

	glUseProgram(shaderProgram);

	int uniformCount, maxLength;
	int attributeCount, maxAttrLength;
	glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &uniformCount);
	glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);
	glGetProgramiv(shaderProgram, GL_ACTIVE_ATTRIBUTES, &attributeCount);
	glGetProgramiv(shaderProgram, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttrLength);

	shader* newShader =  (shader*) dlmalloc( sizeof(shader));
	newShader->numUniforms = newShader->numSamplers = 0;
	//conta quantas uniforms sao samplers ou variaveis uniforms
	char* name = (char*) dlmalloc (sizeof(char)*maxLength);
	char* attrName = (char*) dlmalloc(sizeof(char)*maxAttrLength);
	for (int  i = 0; i < uniformCount; i++){
		GLenum type;
		GLint length, size;
		glGetActiveUniform(shaderProgram, i, maxLength, &length, &size, &type, name);
		if (type >= GL_SAMPLER_1D  &&  type <=  GL_SAMPLER_2D_RECT_SHADOW_ARB)
			newShader->numSamplers++;
		else
			if (strncmp(name, "gl_", 3) != 0)
				newShader->numUniforms++;
	}
	newShader->uniforms = (uniform**)dlmalloc(sizeof(uniform*)*newShader->numUniforms);
	newShader->samplers = (sampler**)dlmalloc(sizeof(sampler*)*newShader->numSamplers);
	int samplers = 0;
	for (int i = 0; i < attributeCount; i++){
		GLenum type;
		GLint length, size;
		glGetActiveAttrib(shaderProgram, i, maxAttrLength, &length, &size, &type, attrName );
		if ( strcmp(name, "Tangent") == 0)
			glBindAttribLocation(shaderProgram, ATTR_TANGENT, "Tangent");
		if ( strcmp(name, "Binormal") == 0 )
			glBindAttribLocation(shaderProgram, ATTR_BINORMAL, "Binormal");
	}
	int numUniforms = 0;
	int numSamplers = 0;
	for(int i = 0; i < uniformCount; i++){
		GLenum type;
		GLint length, size;
		glGetActiveUniform(shaderProgram, i, maxLength, &length, &size, &type, name);
		if (type >= GL_SAMPLER_1D && type <= GL_SAMPLER_2D_RECT_SHADOW_ARB){
            printf("\t\tshader sampler name: %s\n", name);
			GLint location = glGetUniformLocation(shaderProgram, name);
			printf("location e unit: %d %d  \n", location, samplers );
			glUniform1i(location, samplers); //informa o shader a texunit
			sampler* sam = (sampler*) dlmalloc( sizeof(sampler));
			sam->name = (char*) dlmalloc( sizeof( char)*(length + 1));
			sam->index = samplers;
			sam->location = location;
			strcpy(sam->name, name);
			newShader->samplers[numSamplers] = sam;
			samplers++;
			numSamplers++;
		}else{
			if (strncmp(name, "gl_", 3) != 0){
				uniform* uni = (uniform*) dlmalloc(sizeof(uniform));
				uni->name = (char*) dlmalloc( sizeof(char)*(length + 1));
				uni->location = glGetUniformLocation(shaderProgram, name);
				uni->type = getConstantType(type);
				uni->size = size;
				strcpy(uni->name, name);
				int constantSize = constantTypeSizes[uni->type] * uni->size;
				uni->data =  dlmalloc(sizeof(unsigned char) * constantSize);
				memset(uni->data, 0, constantSize);
				uni->dirty = 0;
				if (strcmp(uni->name, "lightPosition") == 0)
					uni->semantic = LIGHTPOS;
				if (strcmp(uni->name, "eyePosition") == 0)
					uni->semantic = EYEPOS;
				if (strcmp(uni->name, "time") == 0)
					uni->semantic = TIME;
				if (strcmp(uni->name, "mvp") == 0)
					uni->semantic = MVP;
				newShader->uniforms[numUniforms] = uni;
				numUniforms++;
			}
		}
	}

	dlfree(name);
    fparray_inspos(newShader, shaderProgram, r->shaders);
	glUseProgram(r->prevShader);
	
	return shaderProgram;
}

void bindShader(unsigned int program){

	if (program == 0){
		glUseProgram(0);
		r->prevShader = 0;
		return;
	}

	if (program != r->prevShader){
		r->prevShader = program;
		glUseProgram(program);
	}

	shader *shdr = fparray_getdata(program, r->shaders);
	for(unsigned int i = 0; i < shdr->numUniforms; i++ ){
		if (shdr->uniforms[i]->dirty ){
			shdr->uniforms[i]->dirty = 0;
			if (shdr->uniforms[i]->type >= CONSTANT_MAT2){
				((UNIFORM_MAT_FUNC) uniformFuncs[shdr->uniforms[i]->type])(shdr->uniforms[i]->location, shdr->uniforms[i]->size, GL_FALSE, (float *) shdr->uniforms[i]->data);
			} else {
				((UNIFORM_FUNC) uniformFuncs[shdr->uniforms[i]->type])(shdr->uniforms[i]->location, shdr->uniforms[i]->size, shdr->uniforms[i]->data);
				//((UNIFORM_FUNC) uniformFuncs[2])(0, 1, color);
				//glUniform4fv(shaders[program]->uniforms[i]->location, shaders[program]->uniforms[i]->size, (GLfloat*) shaders[program]->uniforms[i]->data);
			}
		}else if ( shdr->uniforms[i]->semantic == EYEPOS){
			setShaderConstant3f(program, "eyePosition",  c.pos);
		}else if (shdr->uniforms[i]->semantic == TIME){
		//	setShaderConstant1f(program, "Time", ifps);
			  setShaderConstant1f(program, "time", elapsedTime);
		}else if  (shdr->uniforms[i]->semantic == MVP){
			setShaderConstant4x4f(program, "mvp", mvp);
		
		//else if (r->shaders[program]->uniforms[i]->semantic == LIGHTPOS){
		//	float lightp[3] = {10.0, 10.0, 10.0 };
		//	setShaderConstant3f(program, "LightPosition",  lightp);
		}
	}
}

int printShaderCompilerLog(unsigned int shader){
	int compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	int infoLen = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
	if(infoLen > 1) {
		char* infoLog = (char*) dlmalloc(sizeof(char) * infoLen);
		glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
		printf("Error compiling shader:\n%s\n", infoLog);
		free(infoLog);
		return 1;
	}
	return 0;
}

int printShaderLinkerLog(unsigned int program){
	int linked;
	glGetShaderiv(program, GL_LINK_STATUS, &linked);
	int infoLen = 0;
	glGetShaderiv(program, GL_INFO_LOG_LENGTH, &infoLen);
	if(infoLen > 1) {
		char* infoLog = (char*) dlmalloc(sizeof(char) * infoLen);
		glGetProgramInfoLog(program, infoLen, NULL, infoLog);
		printf("Error compiling shader:\n%s\n", infoLog);
		free(infoLog);
		return 1;
	}
	return 0;
}

void setShaderConstant1i(int shaderid, const char *name, const int constant){
	setShaderConstantRaw(shaderid, name, &constant, sizeof(constant));
}

void setShaderConstant1f(int shaderid, const char *name, const float constant){
	setShaderConstantRaw(shaderid, name, &constant, sizeof(constant));
}

void setShaderConstant2f(int shaderid, const char* name, const float constant[]){
	setShaderConstantRaw(shaderid, name, constant, sizeof(constant)*2);
}

void setShaderConstant3f(int shaderid, const char *name,  const float constant[]){
	setShaderConstantRaw(shaderid, name, constant, sizeof(constant)*3);
}

void setShaderConstant4f(int shaderid, const char *name, const float constant[]){
	setShaderConstantRaw(shaderid, name, constant, sizeof(constant)*4);
}

void setShaderConstant4x4f(int shaderid, const char *name, const float constant[]) {
	setShaderConstantRaw(shaderid, name, constant, sizeof(constant)*16);
}

void setShaderConstantRaw(int shaderid, const char* name, const void* data, int size){
    shader *shdr = fparray_getdata(shaderid, r->shaders);
	for(unsigned int i = 0; i < shdr->numUniforms; i++ ){
		if (strcmp(name, shdr->uniforms[i]->name ) == 0 ){
			if (memcmp(shdr->uniforms[i]->data, data, size)){
				memcpy(shdr->uniforms[i]->data, data, size);
				shdr->uniforms[i]->dirty = 1;
			}
		}
	}
}

int checkFramebufferStatus( int silent)
{
    GLenum status;
    status = (GLenum) glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch(status) {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            if (!silent) printf("Unsupported framebuffer format\n");
            return 0;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            if (!silent) printf("Framebuffer incomplete, missing attachment\n");
            return 0;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            if (!silent) printf("Framebuffer incomplete, duplicate attachment\n");
            return 0;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            if (!silent) printf("Framebuffer incomplete, attached images must have same dimensions\n");
            return 0;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            if (!silent) printf("Framebuffer incomplete, attached images must have same format\n");
            return 0;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            if (!silent) printf("Framebuffer incomplete, missing draw buffer\n");
            return 0;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            if (!silent) printf("Framebuffer incomplete, missing read buffer\n");
            return 0;
        default:
            printf("Framebuffer unknow error\n");
            return 0;
    }
    return 1;
}

unsigned int initializeFramebuffer(void* data, int width, int height, int format, int internalFormat, int type, int  flags){

	
	unsigned int texid = initializeTextureFromMemory(data, width,  height, TEXTURE_2D, format, internalFormat, type, flags);
	unsigned int id;
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texid, 0);

	printf("fbo id criado: %d \n", id);

	//r->framebuffers[id] = id;
    framebuffer *fb = dlmalloc(sizeof(framebuffer));
    fb->id = id;
    fb->width = width;
    fb->height = height;
    fb->texid = texid;

    fparray_inspos(fb, id, r->framebuffers);

	checkFramebufferStatus(0);

	bindMainFramebuffer();

	return id;

}

void bindFramebuffer(int id){
	if (r->prevFramebuffer != id){
		r->prevFramebuffer = id;
		glBindFramebuffer(GL_FRAMEBUFFER, id);
		printf("vai pegar o array\n");
        framebuffer* fb = fparray_getdata(id, r->framebuffers);
		printf("fb array done\n");
		glViewport(0, 0, fb->width,  fb->height); 
		printf("viewport configurado \n");
	}
}

void bindMainFramebuffer(){
	r->prevFramebuffer = 0;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, r->viewPortWidth, r->viewPortHeight);
}
