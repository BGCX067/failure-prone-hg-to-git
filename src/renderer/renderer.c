#include "GL3/glew.h"
#include "renderer.h"
#include "math/matrix.h"
#include "glapp.h"
#include "util/image.h"
#include "util/textfile.h"
#include <string.h>
#include <stdlib.h>
#include "util/utlist.h"
#ifdef __APPLE__
	#include <OpenGL/gl.h>
	#include <OpenGL/glext.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include "GL3/glext.h"
#endif

typedef struct ShaderInfo {
    char *vertname, *fragname;
    Shader *shdr;
    int refs;
    struct ShaderInfo *prev, *next;
} ShaderInfo;
ShaderInfo *shaders = NULL;

//TODO verificar se funções que acessam TextureInfo podem ser escritas de modo melhor
//(uma das funções recebe o filename e retorna a etxture (isLoaded), outra recebe texid
//e retorna o textureInfo, talvez pudesse ter só 1 função que recebe o filename e retorna
//o TextureInfo)
typedef struct TextureInfo {
	char* filename;
	Texture* tex;
	int refs;
    struct TextureInfo *prev, *next;
} TextureInfo;

TextureInfo *textures = NULL;

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

static int getConstantType(GLenum type){
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

int constantTypeLen[CONSTANT_TYPE_COUNT] = {
	1,
	2,
	3,
	4,
	1,
	2,
	3,
	4,
	1,
	2,
	3,
	4,
	4,
	9,
	16,
};

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

Renderer* r;
mat4 view;
mat4 projection;
mat4 model;

mat4 mvp;
mat4 modelview;
mat3 normalmatrix;

vec3 eyepos;

void SetView(mat4 m){
	memcpy(view, m, sizeof(float)*16);
	NormalMatrix(normalmatrix, view);
}

void SetModel(mat4 m){
	memcpy(model, m, sizeof(float)*16);
}

void SetProjection(mat4 m){
	memcpy(projection, m, sizeof(float)*16);
}

void GetView(mat4 m){
	memcpy(m, view, sizeof(float)*16);
}

void GetModel(mat4 m){
	memcpy(m, model, sizeof(float)*16);
}

void GetProjection(mat4 m){
	memcpy( m, projection, sizeof(float)*16);
}

void SetEyepos(vec3 pos) {
    memcpy(eyepos, pos, sizeof(vec3));
}

Renderer* InitializeRenderer(int w, int h, float znear, float zfar, float fovy){
	r = (Renderer*) malloc(sizeof(Renderer));
	Identity(model);
	Identity(view);
	Identity(projection);
	Identity(mvp);
//    r->framebuffers = fparray_init(NULL, free, sizeof(framebuffer));
	r->fovy = fovy;
	r->zfar = zfar;
	r->znear = znear;
	//r->prevVBO = 0;
	//r->prevVAO = -1;
	r->viewPortWidth = w;
	r->viewPortHeight = h;
	//r->prevFramebuffer = -1;

    glewExperimental=1;
    GLenum err = glewInit();
    if (GLEW_OK != err) 
        /* Problem: glewInit failed, something is seriously wrong. */
        printf("Error: %s\n", glewGetErrorString(err));
    printf("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));


//	float ratio = (float) w / (float) h;
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, w, h);

	//glClearColor( 32.0/255.0, 32.0/255.0, 32.0/255.0, 1.0 );
	glClearColor( 0.5, 0.5, 0.5, 1.0 );
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE); 
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

   
	printf("Renderer inicializado.\n");
	PrintGPUMemoryInfo();

	return r;
}

void Begin2d(){
	glDisable(GL_STENCIL_TEST);
	glStencilMask( 0 );
	DisableDepth();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void End2d(){
	EnableDepth();
	glDisable(GL_BLEND);
}

void EnableDepth(){
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

void DisableDepth(){
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
}

////
//TEXTURAS E SAMPLERS
//////

//funcao private do renderer, verifica se uma textura ja foi carregada antes:
static Texture* texIsLoaded(char* filename){
    TextureInfo *it;
    DL_FOREACH(textures, it) {
        if(strcmp(filename, it->filename) == 0) {
            it->refs++;
            return it->tex;
        }
    }
    return NULL;
}

//adiciona uma texinfo nova
static void AddTexInfo(Texture* t, char* filename){
    TextureInfo* newinfo = malloc(sizeof(TextureInfo));
    newinfo->filename = malloc(sizeof(char)*(strlen(filename)+1));
    strcpy(newinfo->filename, filename);
    newinfo->tex = t;
    newinfo->refs = 1;
    DL_APPEND(textures, newinfo);
}

//gettexinfo
static TextureInfo* getTexInfo(int id){
    TextureInfo *it;
    DL_FOREACH(textures, it) {
        if(it->tex->texid == id)
            return it;
    }
    return NULL;
}

SamplerState* InitializeSamplerState(int wrapmode, int minfilter, int magfilter, int anisotropy){
	unsigned int samplerID;
	glGenSamplers(1, &samplerID);
	glSamplerParameteri(samplerID, GL_TEXTURE_MIN_FILTER, minfilter);
	glSamplerParameteri(samplerID, GL_TEXTURE_MAG_FILTER, magfilter);
	glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_S, wrapmode);
	glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_T, wrapmode);
    glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_R, wrapmode);
    float someVec[4];
    //FIXME teste pro shadowmap
    someVec[0] = 0.0;
    //someVec[0] = 1.0;
    someVec[1] = 0.0;
    someVec[2] = 0.0;
    someVec[3] = 0.0;
    //TODO passar por parametro esses outros valores ou deixar na struct renderer
	glSamplerParameterfv(samplerID, GL_TEXTURE_BORDER_COLOR, someVec);
	glSamplerParameterf(samplerID, GL_TEXTURE_MIN_LOD, -1000.f);
	glSamplerParameterf(samplerID, GL_TEXTURE_MAX_LOD, 1000.f);
	glSamplerParameterf(samplerID, GL_TEXTURE_LOD_BIAS, 0.0f);
	
    //FIXME só teste pro shadowmap
    //glSamplerParameteri(samplerID, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	//glSamplerParameteri(samplerID, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glSamplerParameteri(samplerID, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glSamplerParameteri(samplerID, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	//glSamplerParameteri(samplerID, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
	
	SamplerState* state = (SamplerState*) malloc(sizeof(SamplerState));
	state->id = samplerID;
	state->minfilter = minfilter;
	state->magfilter = magfilter;
	state->wrapmode = wrapmode;
	state->anisotropy = anisotropy;

	return state;
}

//TODO sampler state é uma coisa que nao muda muito e se beneficiaria de testar se o prev mudou
void BindSamplerState(SamplerState* s, unsigned int unit){
	if (s!= NULL){
		glBindSampler(unit, s->id);
	}
}

void DestroyTexture(Texture* t){
	if(t != NULL){
		TextureInfo* tinfo = getTexInfo(t->id);
		if (tinfo){
			tinfo->refs--;
			if (tinfo->refs <= 0){
				glDeleteTextures(1,  tinfo->tex->texid);
                DL_DELETE(textures, tinfo);
			}
		} else { //se  nao tem tinfo eh textura gerada procedural, entao deleta 
			glDeleteTextures(1, &t->texid);
		}
	}
}

Texture* InitializeTexture(char* filename, int target, int imageFormat, int internalFormat, int type){

    Texture* isloaded = texIsLoaded(filename);
    if (isloaded) return isloaded;

    Texture *t = malloc(sizeof(Texture));
    t->state = InitializeSamplerState(CLAMP, GL_LINEAR, GL_LINEAR, 0);
    glGenTextures(1, &t->texid);
    glBindTexture(target, t->texid);

    t->target = target;

    if(filename != NULL) {
        int x, y, n;
        unsigned char* data = NULL;
        if ((t->target == TEXTURE_2D) || (t->target == TEXTURE_RECTANGLE) ){
            data = stbi_load(filename, &x, &y, &n, 0);
            if (!data){
                printf("Error loading: %s texture. \n", filename );
                return NULL;
            }
            glTexImage2D(t->target, 0, internalFormat, x, y, 0, imageFormat, type, data);
            stbi_image_free(data);
        } else if (target == TEXTURE_CUBEMAP) {
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
                    return NULL;
                }
                glTexImage2D(facetargets[i], 0, internalFormat, x, y, 0, imageFormat, type, data);
                if (data){
                    stbi_image_free(data);
                    data = NULL;
                }
            }

        }
    }

    t->internalFormat = internalFormat;
    AddTexInfo(t, filename);

    return t;
}

//TODO fazer 1d, 3d e cubemap
Texture* InitializeTextureFromMemory(void* data, int x, int y, int target, int imageFormat, int internalFormat, int type){
    Texture *t = malloc(sizeof(Texture));
    t->state = InitializeSamplerState(CLAMP, GL_LINEAR, GL_LINEAR, 0);
    glGenTextures(1, &t->texid);
    glBindTexture(target, t->texid);

    t->target = target;
    t->internalFormat = internalFormat;
    if ((target == TEXTURE_2D) || (target == TEXTURE_RECTANGLE) ){
		glTexImage2D(target, 0, internalFormat, x, y, 0, imageFormat, type, data);
    } else if (target == TEXTURE_1D) {
        glTexImage1D(target, 0, internalFormat, x, 0, imageFormat, type, data);
    }
    return t;
}

Texture* Initialize2DTexture(char *filename) {
    Texture* isloaded = texIsLoaded(filename);
    if (isloaded){
	 return isloaded;
    }

    int x, y, n;
    unsigned char* data = stbi_load(filename, &x, &y, &n, 0);
    if (!data){
        printf("Error loading: %s texture. \n", filename );
        return NULL;
    }

    Texture* t;
    int internalFormat;
    int format;
    if (n == 3){
	internalFormat = RGB8;
	format = RGB;
    }else if (n == 4){
	    internalFormat = RGBA8;
	    format = RGBA;
    }else{
	    free(data);
	    return NULL;
    }


    t = InitializeTextureFromMemory(data, x, y, TEXTURE_2D, format, internalFormat, UNSIGNED_BYTE);

    free(data);
    if (t != NULL)
    	AddTexInfo(t, filename);
    return t;
}

Texture* Initialize3DTexture(void *data, int xdim, int ydim, int zdim, int imageFormat, int internalFormat, int type) {
    Texture *t = malloc(sizeof(Texture));
    t->state = InitializeSamplerState(GL_CLAMP, GL_LINEAR, GL_LINEAR, 0);
    glGenTextures(1, &t->texid);
    glBindTexture(GL_TEXTURE_3D, t->texid);

    t->target = GL_TEXTURE_3D;
    t->internalFormat = internalFormat;

    glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, xdim, ydim, zdim, 0, imageFormat, type, data);

    return t;
}

void BindTexture(Texture* t, unsigned int slot){
	if (t != NULL){
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(t->target, t->texid );
	}
}

////////
// SHADERS
//////////

static ShaderInfo* shdrIsLoaded(const char* vertfile, const char* fragfile) {
    ShaderInfo *si;
    DL_FOREACH(shaders, si) {
        if((strcmp(vertfile, si->vertname) == 0) &&
           (strcmp(fragfile, si->fragname) == 0)) {
            printf("achou! si->vertname: %s, si->fragname: %s\n", si->vertname, si->fragname);
            return si;
        }
    }
    return NULL;
}

static void shdrAddInfo(Shader *shdr, const char *vertname, const char *fragname) {
    ShaderInfo *si = malloc(sizeof(ShaderInfo));
    //si->vertname = strdup(vertname);
    si->vertname = malloc(sizeof(char)*(strlen(vertname) + 1));
    strcpy(si->vertname, vertname);
    si->fragname = malloc(sizeof(char)*(strlen(fragname) + 1));
    strcpy(si->fragname, fragname);
    //si->fragname = strdup(fragname);

    si->shdr = shdr;
    si->refs = 1;
    DL_APPEND(shaders, si);
}

Shader* InitializeShaderFile(const char* vertfile, const char* fragfile) {
    //verifica se o shader foi loaded ;
    static int count = 1;
    printf("<InitializeShaderFile> count: %d\n", count++);
    ShaderInfo *si = shdrIsLoaded(vertfile, fragfile);
    Shader *shdr;
    if(!si) { //cria um ShaderInfo
        printf("<InitializeShaderFile> !si\n");
        shdr = InitializeShader(NULL, ReadTextFile(vertfile), ReadTextFile(fragfile));
        shdrAddInfo(shdr, vertfile, fragfile);
    } else { //não precisa criar, apenas copia o progid e inicializa uniforms equivalentes
        printf("<InitializeShaderFile> si == true\n");
        si->refs++;
        shdr = malloc(sizeof(Shader));
        shdr->progid = si->shdr->progid;
        shdr->numUniforms = si->shdr->numUniforms;
        shdr->numSamplers = si->shdr->numSamplers;
        shdr->uniforms = malloc(sizeof(Uniform*)*shdr->numUniforms);
        shdr->samplers = malloc(sizeof(Sampler*)*shdr->numSamplers);
        for(int i = 0; i < shdr->numUniforms; i++) {
            Uniform *uni = malloc(sizeof(Uniform));
            uni->location = si->shdr->uniforms[i]->location;
            uni->name = malloc(sizeof(char)*(strlen(si->shdr->uniforms[i]->name)+ 1));
            strcpy(uni->name, si->shdr->uniforms[i]->name);
            //uni->name = strdup(shdr->uniforms[i]->name);
            printf("<InitializeShader> i: %d, name: %s\n", i, si->shdr->uniforms[i]->name);
            uni->size = si->shdr->uniforms[i]->size;
            uni->type = si->shdr->uniforms[i]->type;
            uni->semantic = si->shdr->uniforms[i]->semantic;
            uni->dirty = 0;
            int constantsize = constantTypeSizes[uni->type] * uni->size;
            uni->data = malloc(sizeof(unsigned char) * constantsize);
            memset(uni->data, 0, constantsize);
            shdr->uniforms[i] = uni;
        }
        //TODO copiar samplers
        
    }
    return shdr;
}

Shader* InitializeShader(const char* geometrysource, const char* vertexsource, const char* fragmentsource){
	if (!vertexsource && !fragmentsource)
		return 0;

	int shaderprogram = glCreateProgram();
	if (!shaderprogram)
		return 0;

	int vertexshader = 0;
	if (vertexsource){
		vertexshader = glCreateShader(GL_VERTEX_SHADER);

		if (vertexshader == 0)
			return 0;

		glShaderSource(vertexshader, 1, &vertexsource, 0);
		glCompileShader(vertexshader);

		if (PrintShaderCompilerLog(vertexshader)){
			glDeleteShader(vertexshader);
			return 0;
		}

		glAttachShader(shaderprogram, vertexshader);
	}

	int fragmentshader = 0;
	if (fragmentsource){
		fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);

		if (fragmentshader == 0)
			return 0;

		glShaderSource(fragmentshader, 1, &fragmentsource, 0);
		glCompileShader(fragmentshader);

		if (PrintShaderCompilerLog(fragmentshader)){
			glDeleteShader(fragmentshader);
			return 0;
		}

		glAttachShader(shaderprogram, fragmentshader);
	}

	int geometryshader = 0;
	if (geometrysource){
		geometryshader = glCreateShader(GL_GEOMETRY_SHADER);

		if (geometryshader == 0)
			return 0;

		glShaderSource(geometryshader, 1, &geometrysource, 0);
		glCompileShader(geometryshader);

		if (PrintShaderCompilerLog(geometryshader)){
			glDeleteShader(geometryshader);
			return 0;
		}

		glAttachShader(shaderprogram, geometryshader);
	}


	glLinkProgram(shaderprogram);
	if (PrintShaderLinkerLog(shaderprogram)){
		glDeleteShader(vertexshader);
		glDeleteShader(fragmentshader);
		glDeleteProgram(shaderprogram);
		if (geometryshader)
			glDeleteProgram(geometryshader);
		return 0;
	}

	glUseProgram(shaderprogram);

	int uniformcount, maxlength;
	glGetProgramiv(shaderprogram, GL_ACTIVE_UNIFORMS, &uniformcount);
	glGetProgramiv(shaderprogram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxlength);

	Shader* newshader =  (Shader*) malloc( sizeof(Shader));
    newshader->progid = shaderprogram;
	newshader->numUniforms = newshader->numSamplers = 0;
	//conta quantas uniforms sao samplers ou variaveis uniforms
	char* name = (char*) malloc (sizeof(char)*maxlength);
	for (int  i = 0; i < uniformcount; i++){
		GLenum type;
		GLint length, size;
		glGetActiveUniform(shaderprogram, i, maxlength, &length, &size, &type, name);
		if (type >= GL_SAMPLER_1D  &&  type <=  GL_SAMPLER_2D_RECT_SHADOW_ARB)
			newshader->numSamplers++;
		else
			if (strncmp(name, "gl_", 3) != 0)
				newshader->numUniforms++;
	}
	newshader->uniforms = (Uniform**)malloc(sizeof(Uniform*)*newshader->numUniforms);
	newshader->samplers = (Sampler**)malloc(sizeof(Sampler*)*newshader->numSamplers);
	int samplers = 0;
	int numuniforms = 0;
	int numsamplers = 0;
	for(int i = 0; i < uniformcount; i++){
		GLenum type;
		GLint length, size;
		glGetActiveUniform(shaderprogram, i, maxlength, &length, &size, &type, name);
		if (type >= GL_SAMPLER_1D && type <= GL_SAMPLER_2D_RECT_SHADOW_ARB){
			GLint location = glGetUniformLocation(shaderprogram, name);
			glUniform1i(location, samplers); //informa o shader a texunit
			Sampler* sam = (Sampler*) malloc( sizeof(Sampler));
			sam->name = (char*) malloc( sizeof( char)*(length + 1));
			sam->index = samplers;
			sam->location = location;
			strcpy(sam->name, name);
			newshader->samplers[numsamplers] = sam;
			samplers++;
			numsamplers++;
		}else{
			if (strncmp(name, "gl_", 3) != 0){
				Uniform* uni = (Uniform*) malloc(sizeof(Uniform));
				uni->name = (char*) malloc( sizeof(char)*(length + 1));
				uni->location = glGetUniformLocation(shaderprogram, name);
				uni->type = getConstantType(type);
				uni->size = size;
				strcpy(uni->name, name);
                printf("uni->name: %s\n", uni->name);
				int constantsize = constantTypeSizes[uni->type] * uni->size;
				uni->data = malloc(sizeof(unsigned char) * constantsize);
				memset(uni->data, 0, constantsize);
				uni->dirty = 0;
				if (strcmp(uni->name, "eyepos") == 0)
					uni->semantic = EYEPOS;
				if (strcmp(uni->name, "time") == 0)
					uni->semantic = TIME;
				if (strcmp(uni->name, "model") == 0)
					uni->semantic = MODEL;
                if (strcmp(uni->name, "view") == 0)
					uni->semantic = VIEW;
				if ( strcmp(uni->name, "projection") == 0 )
					uni->semantic = PROJECTION;
				if (strcmp(uni->name,"mvp") == 0)
					uni->semantic = MVP;
				if (strcmp(uni->name,"modelview") == 0)
					uni->semantic = MODELVIEW;
                if (strcmp(uni->name,"invModelView") == 0)
					uni->semantic = INVMODELVIEW;
				if (strcmp(uni->name,"normalmatrix") == 0)
					uni->semantic = NORMALMATRIX;
				newshader->uniforms[numuniforms] = uni;
				numuniforms++;
			}
		}
	}

	free(name);
	return newshader;
}

//bug
void BindShader(Shader* shdr){
	if (shdr == NULL)
		return;    

    glUseProgram(shdr->progid);
    for(unsigned int i = 0; i < shdr->numUniforms; i++ ){
        if (shdr->uniforms[i]->dirty ){
            shdr->uniforms[i]->dirty = 0;
            if (shdr->uniforms[i]->type == CONSTANT_MAT4){
                glUniformMatrix4fv(shdr->uniforms[i]->location, shdr->uniforms[i]->size, GL_FALSE, (float *) shdr->uniforms[i]->data);
            } else if (shdr->uniforms[i]->type == CONSTANT_MAT3){
                glUniformMatrix3fv(shdr->uniforms[i]->location, shdr->uniforms[i]->size, GL_FALSE, (float *) shdr->uniforms[i]->data);
            } else if (shdr->uniforms[i]->type == CONSTANT_VEC2){
                glUniform2fv(shdr->uniforms[i]->location, shdr->uniforms[i]->size, (float*)shdr->uniforms[i]->data);
            } else if (shdr->uniforms[i]->type == CONSTANT_VEC3){
                glUniform3fv(shdr->uniforms[i]->location, shdr->uniforms[i]->size, (float*)shdr->uniforms[i]->data);
            } else if (shdr->uniforms[i]->type == CONSTANT_VEC4){
                glUniform4fv(shdr->uniforms[i]->location, shdr->uniforms[i]->size, (float*)shdr->uniforms[i]->data);
            } else if (shdr->uniforms[i]->type == CONSTANT_FLOAT){
                glUniform1f(shdr->uniforms[i]->location, *((float*)shdr->uniforms[i]->data));
            }
        }
        if(shdr->uniforms[i]->semantic == MVP){
            Multm(modelview, view, model);
            Multm(mvp, projection, modelview);
            glUniformMatrix4fv(shdr->uniforms[i]->location, shdr->uniforms[i]->size, GL_FALSE, (float *) mvp);
        } else if(shdr->uniforms[i]->semantic == MODELVIEW){
            Multm(modelview, view, model);
            glUniformMatrix4fv(shdr->uniforms[i]->location, shdr->uniforms[i]->size, GL_FALSE, (float *) modelview);
        } else if(shdr->uniforms[i]->semantic == NORMALMATRIX){
            Multm(modelview, view, model);
            NormalMatrix(normalmatrix, modelview);
            glUniformMatrix3fv(shdr->uniforms[i]->location, shdr->uniforms[i]->size, GL_FALSE, (float *) normalmatrix);
        } else if(shdr->uniforms[i]->semantic == VIEW){
            glUniformMatrix4fv(shdr->uniforms[i]->location, shdr->uniforms[i]->size, GL_FALSE, (float *) view);
        } else if(shdr->uniforms[i]->semantic == MODEL) { 
            glUniformMatrix4fv(shdr->uniforms[i]->location, shdr->uniforms[i]->size, GL_FALSE, (float *) model);
        } else if(shdr->uniforms[i]->semantic == INVMODELVIEW){
            Multm(modelview, view, model);
            mat4 invModelView;
            Inverse(invModelView, modelview);
            glUniformMatrix4fv(shdr->uniforms[i]->location, shdr->uniforms[i]->size, GL_FALSE, (float *) invModelView); 
        }else if(shdr->uniforms[i]->semantic == EYEPOS){
            glUniform3fv(shdr->uniforms[i]->location, shdr->uniforms[i]->size, (float *) eyepos);
        }
    }
}

int PrintShaderCompilerLog(unsigned int shader){
	int compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	int infoLen = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
	if(infoLen > 1) {
		char* infoLog = (char*) malloc(sizeof(char) * infoLen);
		glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
		printf("Error compiling shader:\n%s\n", infoLog);
		free(infoLog);
		return 1;
	}
	return 0;
}

int PrintShaderLinkerLog(unsigned int program){
	int linked;
	glGetShaderiv(program, GL_LINK_STATUS, &linked);
	int infoLen = 0;
	glGetShaderiv(program, GL_INFO_LOG_LENGTH, &infoLen);
	if(infoLen > 1) {
		char* infoLog = (char*) malloc(sizeof(char) * infoLen);
		glGetProgramInfoLog(program, infoLen, NULL, infoLog);
		printf("Error compiling shader:\n%s\n", infoLog);
		free(infoLog);
		return 1;
	}
	return 0;
}

static int resolveUniType(int type) {
    switch(type) {
        case CONSTANT_FLOAT:
        case CONSTANT_VEC2:
        case CONSTANT_VEC3:
        case CONSTANT_VEC4:
        case CONSTANT_MAT2:
        case CONSTANT_MAT3:
        case CONSTANT_MAT4:
            return FLOAT;
            break;
        case CONSTANT_INT:
        case CONSTANT_IVEC2:
        case CONSTANT_IVEC3:
        case CONSTANT_IVEC4:
        case CONSTANT_BOOL:
        case CONSTANT_BVEC2:
        case CONSTANT_BVEC3:
        case CONSTANT_BVEC4:
            return INT;
            break;
        default:
            return -1;
    }
}

void SetShaderConstant(Shader* shdr, const char* name, const void* data) {
    for(unsigned int i = 0; i < shdr->numUniforms; i++ ){
        Uniform *uni = shdr->uniforms[i];
        if (strcmp(name, uni->name) == 0 ){
            int t = resolveUniType(uni->type);
            for(int j = 0; j < constantTypeLen[uni->type] ; j++) {
                switch(t) {
                    case FLOAT: {
                        float *a = uni->data;
                        const float *b = data;
                        if(FCmp(a[j], b[j])) {
                            uni->dirty = 1;
                            a[j] = b[j];
                        }
                        break;
                    }
                    case INT: {
                        int *a = uni->data;
                        const int *b = data;
                        if(a[j] != b[j]) {
                            uni->dirty = 1;
                            a[j] = b[j];
                        }
                      break;
                    }
                    case UNSIGNED_BYTE: {
                        unsigned char *a = uni->data;
                        const unsigned char *b = data;
                        if(a[j] != b[j]) {
                            uni->dirty = 1;
                            a[j] = b[j];
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    }
}

VertexAttribute** InitializeVertexFormat(){
	VertexAttribute** attr = malloc(sizeof(VertexAttribute*)*MAX_VERTEX_ATTRS);
	for (int i = 0; i < MAX_VERTEX_ATTRS; i++)
		attr[i] = NULL;
	return attr;
}

void SetVertexAttribute(VertexAttribute** attr, int type, unsigned int count, unsigned int size, unsigned int offset, unsigned int comp, unsigned int vboid){

	attr[type] = malloc(sizeof(VertexAttribute));
	attr[type]->count = count;
	attr[type]->size = size;
	attr[type]->type = type;
	attr[type]->offset =  offset;
	attr[type]->components = comp;
	attr[type]->vboID = vboid;


}  

///////////////////////////////
//      VAO Related
///////////////////////////////
unsigned int InitEmptyVAO(){
	unsigned int vaoID;
	glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);
	return vaoID;
}

void ConfigureVAO(unsigned int vaoID, VertexAttribute** attrs){
	glBindVertexArray(vaoID);
	for(unsigned int i = 0; i < MAX_VERTEX_ATTRS; i++){
		if (attrs[i]){
			glEnableVertexAttribArray(i);
			glBindBuffer(GL_ARRAY_BUFFER, attrs[i]->vboID);
			glVertexAttribPointer(i, attrs[i]->components, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(attrs[i]->offset));
		}
	}
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ConfigureIndexedVAO(unsigned int vaoID, unsigned int indicesid, VertexAttribute** attrs){
	glBindVertexArray(vaoID);
	for(unsigned int i = 0; i < MAX_VERTEX_ATTRS; i++){
		if (attrs[i]){
			glEnableVertexAttribArray(i);
			glBindBuffer(GL_ARRAY_BUFFER, attrs[i]->vboID);
			glVertexAttribPointer(i, attrs[i]->components, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(attrs[i]->offset));
		}
	}
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,  indicesid);
}

void DestroyVAO(unsigned int vaoID) {
    glDeleteVertexArrays(1, &vaoID);
}

void DrawArraysVAO(unsigned int vaoID, int type, int numVerts){
	glBindVertexArray(vaoID);
	glDrawArrays(type, 0, numVerts);
}

void DrawIndexedVAO(unsigned int vaoID, unsigned int triCount, int geometryType){
    glBindVertexArray(vaoID);
    glDrawElements(geometryType, triCount, GL_UNSIGNED_INT, NULL);
}

///////////////////////////////
//      VBO Related
///////////////////////////////
unsigned int InitializeVBO(unsigned int size, int mode, const void* data){
	unsigned int vboID;
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, size, data, mode);
	return vboID;
}

void* MapVBO(unsigned int vboID,int mode){
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	float* ret = (float*) glMapBuffer(GL_ARRAY_BUFFER, mode);
	//glBindBuffer(GL_ARRAY_BUFFER, r->prevVBO);
	return ret;
}

void UnmapVBO(unsigned int id){
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	//glBindBuffer(GL_ARRAY_BUFFER, r->prevVBO);
}

void DestroyVBO(unsigned int id){
	glDeleteBuffers(1, &id);
}


//////////////////
// FRAMEBUFFERS
////////////////

static int checkFramebufferStatus( int silent)
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

Framebuffer* InitializeFramebufferDepth(int width, int height){
    Framebuffer *fb = malloc(sizeof(Framebuffer));
    fb->width = width;
    fb->height = height;
    glGenFramebuffers(1, &fb->id);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->id);
    
    fb->tex = malloc(sizeof(Texture));
    fb->tex->state = InitializeSamplerState(CLAMP_TO_BORDER, GL_LINEAR, GL_LINEAR, 0);
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &fb->tex->texid);
    glBindTexture(TEXTURE_2D, fb->tex->texid);

    fb->tex->target = TEXTURE_2D;
    glTexImage2D(TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fb->tex->texid, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
    checkFramebufferStatus(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return fb;
}

Framebuffer* InitializeFramebufferColor(int width, int height){
    Framebuffer *fb = malloc(sizeof(Framebuffer));
    fb->width = width;
    fb->height = height;
    glGenFramebuffers(1, &fb->id);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->id);
    
    fb->tex = malloc(sizeof(Texture));
    fb->tex->state = InitializeSamplerState(CLAMP, GL_LINEAR, GL_LINEAR, 0);
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &fb->tex->texid);
    glBindTexture(TEXTURE_2D, fb->tex->texid);
    fb->tex->target = TEXTURE_2D;
    glTexImage2D(TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->tex->texid, 0);

    //Cria o depth buffer
    glGenRenderbuffers(1, &fb->depthid);
    glBindRenderbuffer(GL_RENDERBUFFER, fb->depthid);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fb->depthid);

    GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0};

    glDrawBuffers(1, drawBufs);
    checkFramebufferStatus(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    return fb;
}

Framebuffer* InitializeFramebuffer2(int width, int height) {
    //Inicializa o FBO
    Framebuffer *fb = malloc(sizeof(Framebuffer));
    fb->width = width;
    fb->height = height;
    glGenFramebuffers(1, &fb->id);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->id);
    
    //fb->tex = initializeTextureFromMemory(0, width, height, TEXTURE_2D, RGBA, RGBA8, UNSIGNED_BYTE);

    fb->tex = malloc(sizeof(Texture));
    fb->tex->state = InitializeSamplerState(CLAMP, GL_LINEAR, GL_LINEAR, 0);
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &fb->tex->texid);
    glBindTexture(TEXTURE_2D, fb->tex->texid);

    fb->tex->target = TEXTURE_2D;
    glTexImage2D(TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->tex->texid, 0);

    //Cria o depth buffer
    glGenRenderbuffers(1, &fb->depthid);
    glBindRenderbuffer(GL_RENDERBUFFER, fb->depthid);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fb->depthid);

//    GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0};
//    glDrawBuffers(1, drawBufs);

//	glDrawBuffer(GL_NONE);
//	glReadBuffer(GL_NONE);

    checkFramebufferStatus(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    return fb;
}


void BindFramebuffer(Framebuffer *fb) {
    if(!fb) { //Volta pro framebuffer default
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, r->viewPortWidth, r->viewPortHeight);
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, fb->id);
        glViewport(0, 0, fb->width, fb->height);
    }
}

/*unsigned int initializeFramebuffer(void* data, int width, int height, int format, int internalFormat, int type){

	
	Texture *t = initializeTextureFromMemory(data, width,  height, TEXTURE_2D, format, internalFormat, type);
	unsigned int id;
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t->texid, 0);

	//printf("fbo id criado: %d \n", id);

	//r->framebuffers[id] = id;
    framebuffer *fb = malloc(sizeof(framebuffer));
    fb->id = id;
    fb->width = width;
    fb->height = height;
    fb->texid = t->texid;

//    fparray_inspos(fb, id, r->framebuffers);

	checkFramebufferStatus(0);

	bindMainFramebuffer();

	return id;

}*/

/*void bindFramebuffer(int id){
	if (r->prevFramebuffer != id){
		r->prevFramebuffer = id;
		glBindFramebuffer(GL_FRAMEBUFFER, id);
		//printf("vai pegar o array\n");
        framebuffer* fb = fparray_getdata(id, r->framebuffers);
		//printf("fb array done\n");
		glViewport(0, 0, fb->width,  fb->height); 
		//printf("viewport configurado \n");
	}
}*/

/*void bindMainFramebuffer(){
	r->prevFramebuffer = 0;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, r->viewPortWidth, r->viewPortHeight);
}*/

void PrintGPUMemoryInfo(){
	if (GLEW_NVX_gpu_memory_info){
		int m = 0;
		glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &m);
		printf("Dedicated Video Memory: %d Kb. \n", m );
		m = 0;
		glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &m);
		printf("Total Avaliable Memory: %d Kb. \n", m);
		m = 0;
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &m);
		printf("Current Avaliable Dedicated Video Memory: %d Kb. \n", m);
		m = 0;
		glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX, &m);
		printf("Count of Total Evictions Seen By System: %d. \n", m);
		m = 0;
		glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &m);
		printf("Size of Total Video Memory Evicted %d Kb. \n", m);
	}else if ( GLEW_ATI_meminfo){
		int vbo[4];
		glGetIntegerv(GL_VBO_FREE_MEMORY_ATI, vbo);
		int textures[4];
		glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, textures);
		int renderbuffer[4];
		glGetIntegerv(GL_RENDERBUFFER_FREE_MEMORY_ATI, renderbuffer);
		printf( "VBO: total memory free in the pool: %d Kb. Largest avaliable free block: %d Kb. Total auxiliary memory free %d Kb. Largest auxiliary free block: %d Kb. \n", vbo[0], vbo[1], vbo[2], vbo[3] );
		printf( "TEXTURE: total memory free in the pool: %d Kb. Largest avaliable free block: %d Kb. Total auxiliary memory free %d Kb. Largest auxiliary free block: %d Kb. \n", textures[0], textures[1], textures[2], textures[3] );
		printf( "RENDERBUFFER: total memory free in the pool: %d Kb. Largest avaliable free block: %d Kb. Total auxiliary memory free %d Kb. Largest auxiliary free block: %d Kb. \n", renderbuffer[0], renderbuffer[1], renderbuffer[2], renderbuffer[3] );
	}else 
		printf("GPU Memory Info Not Supported.");
}

void Screenshot(char* filename){
	unsigned char* pixels = malloc(sizeof( unsigned char)*r->viewPortWidth*r->viewPortHeight*3);
	glReadPixels(0, 0, r->viewPortWidth, r->viewPortHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	stbi_write_tga(filename, r->viewPortWidth, r->viewPortHeight, 3, pixels);
	free(pixels);
}

