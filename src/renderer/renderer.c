#include "GL3/glew.h"
#include "GL3/glext.h"
#include "renderer.h"
#include "math/matrix.h"
#include "../glapp.h"
#include "../util/image.h"
#include "../util/textfile.h"
#include <GL/glu.h>
#include <string.h>
#include <stdlib.h>

typedef struct TextureInfo_ {

	char* filename;
	Texture* tex;
	int refs;

} TextureInfo;

fplist* textures;

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

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

renderer* r;
mat4 view;
mat4 projection;
mat4 model;

mat4 mvp;
mat4 modelview;
mat3 normalmatrix;

void setView(mat4 m){
	memcpy(view, m, sizeof(float)*16);
	fpNormalMatrix(normalmatrix, view);
}

void setModel(mat4 m){
	memcpy(model, m, sizeof(float)*16);
}

void setProjection(mat4 m){
	memcpy(projection, m, sizeof(float)*16);
}

void getView(mat4 m){
	memcpy(m, view, sizeof(float)*16);
}

void getModel(mat4 m){
	memcpy(m, model, sizeof(float)*16);
}

void getProjection(mat4 m){
	memcpy( m, projection, sizeof(float)*16);
}

renderer* initializeRenderer(int w, int h, float znear, float zfar, float fovy){
	r = (renderer*) malloc(sizeof(renderer));
	fpIdentity(model);
	fpIdentity(view);
	fpIdentity(projection);
	fpIdentity(mvp);
	textures = fplist_init(free);
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

	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

   
	printf("Renderer inicializado.\n");
	printGPUMemoryInfo();

	return r;
}

void begin2d(){
	
	glDisable(GL_STENCIL_TEST);
	glStencilMask( 0 );
	disableDepth();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void end2d(){
	enableDepth();
	glDisable(GL_BLEND);
}

void enableDepth(){
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

void disableDepth(){
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
}

////
//TEXTURAS E SAMPLERS
//////

//funcao private do renderer, verifica se uma textura ja foi carregada antes:
Texture* texIsLoaded(char* filename){
    for (int i = 0; i < textures->size; i++){
	TextureInfo *tinfo = fplist_getdata(i, textures);
	if ( (strcmp(filename, tinfo->filename ) == 0) ){
		tinfo->refs++;
		return tinfo->tex;
	}
    }
    return NULL;
}

//adiciona uma texinfo nova
void addTexInfo(Texture* t, char* filename){
    TextureInfo* newinfo = malloc(sizeof(TextureInfo));
    newinfo->filename = malloc(sizeof(char)*(strlen(filename)+1));
    strcpy(newinfo->filename, filename);
    newinfo->tex = t;
    newinfo->refs = 1;
    fplist_insback(newinfo, textures);

}

//gettexinfo
TextureInfo* getTexInfo(int id){
    for (int i = 0; i < textures->size; i++){
	TextureInfo *tinfo = fplist_getdata(i, textures);
	if ( tinfo->tex->id  == id ){
		return tinfo;
	}
    }
    return -1;
}

SamplerState* initializeSamplerState(int wrapmode, int minfilter, int magfilter, int anisotropy){

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
	
	SamplerState* state = (SamplerState*) malloc(sizeof(SamplerState));
	state->id = samplerID;
	state->minfilter = minfilter;
	state->magfilter = magfilter;
	state->wrapmode = wrapmode;
	state->anisotropy = anisotropy;

	return state;
}

//TODO sampler state é uma coisa que nao muda muito e se beneficiaria de testar se o prev mudou
void bindSamplerState(SamplerState* s, unsigned int unit){
	if (s!= NULL){
		glBindSampler(unit, s->id);
	}
}

void destroyTexture(Texture* t){
	if (t != NULL){
		TextureInfo* tinfo = getTexInfo(t->id);
		if (tinfo){
			tinfo->refs--;
			if (tinfo->refs <= 0){
				glDeleteTextures(1,  tinfo->tex->texid);
				for ( int i = 0; i < textures->size; i++){ //TODO nao tem funcao legal pra remover de uma fplist
					TextureInfo* toRemove = fplist_getdata(i, textures);
					if (toRemove->tex->id == t->id)
						fplist_rm(i, textures);
				} 	
			}
		} else { //se  nao tem tinfo eh textura gerada procedural, entao deleta 
			glDeleteTextures(1, &t->texid);
		}
	}
}

Texture* initializeTexture(char* filename, int target, int imageFormat, int internalFormat, int type){

    Texture* isloaded = texIsLoaded(filename);
    if (isloaded) return isloaded;

    Texture *t = malloc(sizeof(Texture));
    t->state = initializeSamplerState(CLAMP, GL_LINEAR, GL_LINEAR, 0);
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
    addTexInfo(t, filename);

    return t;
}

//TODO fazer 1d, 3d e cubemap
Texture* initializeTextureFromMemory(void* data, int x, int y, int target, int imageFormat, int internalFormat, int type){
    Texture *t = malloc(sizeof(Texture));
    t->state = initializeSamplerState(CLAMP, GL_LINEAR, GL_LINEAR, 0);
    glGenTextures(1, &t->texid);
    glBindTexture(target, t->texid);

    t->target = target;
    t->internalFormat = internalFormat;
    if ((target == TEXTURE_2D) || (target == TEXTURE_RECTANGLE) ){
		glTexImage2D(target, 0, internalFormat, x, y, 0, imageFormat, type, data);
    }
    return t;
}

Texture* initialize2DTexture(char *filename) {

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


    t = initializeTextureFromMemory(data, x, y, TEXTURE_2D, format, internalFormat, UNSIGNED_BYTE);

    free(data);
    if (t != NULL)
    	addTexInfo(t, filename);
    return t;
}

void bindTexture(Texture* t, unsigned int slot){
	if (t != NULL){
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(t->target, t->texid );
	}
}

////////
// SHADERS
//////////
Shader* initializeShader(const char* vertexSource, const char* fragmentSource){
	if (!vertexSource && !fragmentSource)
		return 0;

	int shaderProgram = glCreateProgram();
	if (!shaderProgram)
		return 0;

	int vertexShader = 0;
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

	int fragmentShader = 0;
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

	Shader* newShader =  (Shader*) malloc( sizeof(Shader));
    newShader->progid = shaderProgram;
	newShader->numUniforms = newShader->numSamplers = 0;
	//conta quantas uniforms sao samplers ou variaveis uniforms
	char* name = (char*) malloc (sizeof(char)*maxLength);
	//char* attrName = (char*) malloc(sizeof(char)*maxAttrLength);
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
	newShader->uniforms = (Uniform**)malloc(sizeof(Uniform*)*newShader->numUniforms);
	newShader->samplers = (Sampler**)malloc(sizeof(Sampler*)*newShader->numSamplers);
	int samplers = 0;
	/*for (int i = 0; i < attributeCount; i++){
		GLenum type;
		GLint length, size;
		glGetActiveAttrib(shaderProgram, i, maxAttrLength, &length, &size, &type, attrName );
		if ( strcmp(name, "Tangent") == 0)
			glBindAttribLocation(shaderProgram, ATTR_TANGENT, "Tangent");
		if ( strcmp(name, "Binormal") == 0 )
			glBindAttribLocation(shaderProgram, ATTR_BINORMAL, "Binormal");
	}*/
	int numUniforms = 0;
	int numSamplers = 0;
	for(int i = 0; i < uniformCount; i++){
		GLenum type;
		GLint length, size;
		glGetActiveUniform(shaderProgram, i, maxLength, &length, &size, &type, name);
		if (type >= GL_SAMPLER_1D && type <= GL_SAMPLER_2D_RECT_SHADOW_ARB){
			GLint location = glGetUniformLocation(shaderProgram, name);
			glUniform1i(location, samplers); //informa o shader a texunit
			Sampler* sam = (Sampler*) malloc( sizeof(Sampler));
			sam->name = (char*) malloc( sizeof( char)*(length + 1));
			sam->index = samplers;
			sam->location = location;
			strcpy(sam->name, name);
			newShader->samplers[numSamplers] = sam;
			samplers++;
			numSamplers++;
		}else{
			if (strncmp(name, "gl_", 3) != 0){
				Uniform* uni = (Uniform*) malloc(sizeof(Uniform));
				uni->name = (char*) malloc( sizeof(char)*(length + 1));
				uni->location = glGetUniformLocation(shaderProgram, name);
				uni->type = getConstantType(type);
				uni->size = size;
				strcpy(uni->name, name);
				int constantSize = constantTypeSizes[uni->type] * uni->size;
				uni->data =  malloc(sizeof(unsigned char) * constantSize);
				memset(uni->data, 0, constantSize);
				uni->dirty = 0;
				if (strcmp(uni->name, "lightPosition") == 0)
					uni->semantic = LIGHTPOS;
				if (strcmp(uni->name, "eyePosition") == 0)
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
				if (strcmp(uni->name,"normalmatrix") == 0)
					uni->semantic = NORMALMATRIX;
				newShader->uniforms[numUniforms] = uni;
				numUniforms++;
			}
		}
	}

	free(name);
	return newShader;
}

void bindShader(Shader* shdr){
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
                glUniform1f(shdr->uniforms[i]->location, *(shdr->uniforms[i]->data));
           }
        }
        if  (shdr->uniforms[i]->semantic == MVP){
		fpMultMatrix(modelview, model, view);
		fpMultMatrix(mvp, projection, modelview);
		glUniformMatrix4fv(shdr->uniforms[i]->location, shdr->uniforms[i]->size, GL_FALSE, (float *) mvp);
        } else if  (shdr->uniforms[i]->semantic == MODELVIEW){
		fpMultMatrix(modelview, model, view);
		glUniformMatrix4fv(shdr->uniforms[i]->location, shdr->uniforms[i]->size, GL_FALSE, (float *) modelview);
	} else if  (shdr->uniforms[i]->semantic == NORMALMATRIX){
		glUniformMatrix3fv(shdr->uniforms[i]->location, shdr->uniforms[i]->size, GL_FALSE, (float *) normalmatrix);
	}

   }
}

int printShaderCompilerLog(unsigned int shader){
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

int printShaderLinkerLog(unsigned int program){
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

void setShaderConstant1i(Shader* s, const char *name, const int constant){
	setShaderConstantRaw(s, name, &constant, sizeof(int));
}

void setShaderConstant1f(Shader* s, const char *name, const float constant){
	setShaderConstantRaw(s, name, &constant, sizeof(float));
}

void setShaderConstant2f(Shader *s, const char* name, const float constant[]){
	setShaderConstantRaw(s, name, constant, sizeof(float)*2);
}

void setShaderConstant3f(Shader* s, const char *name,  const float constant[]){
	setShaderConstantRaw(s, name, constant, sizeof(float)*3);
}

void setShaderConstant4f(Shader* s, const char *name, const float constant[]){
	setShaderConstantRaw(s, name, constant, sizeof(float)*4);
}

void setShaderConstant3x3f(Shader* s, const char *name, const float constant[]) {
	setShaderConstantRaw(s, name, constant, sizeof(float)*9);
}

void setShaderConstant4x4f(Shader* s, const char *name, const float constant[]) {
	setShaderConstantRaw(s, name, constant, sizeof(float)*16);
}

void setShaderConstantRaw(Shader* shdr, const char* name, const void* data, int size){
	for(unsigned int i = 0; i < shdr->numUniforms; i++ ){
		if (strcmp(name, shdr->uniforms[i]->name ) == 0 ){
			if (memcmp(shdr->uniforms[i]->data, data, size)){
				memcpy(shdr->uniforms[i]->data, data, size);
				shdr->uniforms[i]->dirty = 1;
			}
		}
	}
}



VertexAttribute** initializeVertexFormat(){
	VertexAttribute** attr = malloc(sizeof(VertexAttribute*)*MAX_VERTEX_ATTRS);
	for (int i = 0; i < MAX_VERTEX_ATTRS; i++)
		attr[i] = NULL;
	return attr;
}

void setVertexAttribute(VertexAttribute** attr, int type, unsigned int count, unsigned int size, unsigned int offset, unsigned int comp, unsigned int vboid){

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
unsigned int initEmptyVAO(){
	unsigned int vaoID;
	glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);
	return vaoID;
}

void configureVAO(unsigned int vaoID, VertexAttribute** attrs){
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

void configureIndexedVAO(unsigned int vaoID, unsigned int indicesid, VertexAttribute** attrs){
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

void destroyVAO(unsigned int vaoID) {
    glDeleteVertexArrays(1, &vaoID);
}

void drawArraysVAO(unsigned int vaoID, int type, int numVerts){
	glBindVertexArray(vaoID);
	glDrawArrays(type, 0, numVerts);
}

void drawIndexedVAO(unsigned int vaoID, unsigned int triCount, int geometryType){
    glBindVertexArray(vaoID);
    glDrawElements(geometryType, triCount, GL_UNSIGNED_INT, NULL);
}

///////////////////////////////
//      VBO Related
///////////////////////////////
unsigned int initializeVBO(unsigned int size, int mode, const void* data){
	unsigned int vboID;
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, size, data, mode);
	return vboID;
}

void* mapVBO(unsigned int vboID,int mode){
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	float* ret = (float*) glMapBuffer(GL_ARRAY_BUFFER, mode);
	//glBindBuffer(GL_ARRAY_BUFFER, r->prevVBO);
	return ret;
}

void unmapVBO(unsigned int id){
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	//glBindBuffer(GL_ARRAY_BUFFER, r->prevVBO);
}

void destroyVBO(unsigned int id){
	glDeleteBuffers(1, &id);
}


//////////////////
// FRAMEBUFFERS
////////////////

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

Framebuffer* initializeFramebuffer(int width, int height){

    Framebuffer *fb = malloc(sizeof(Framebuffer));
    fb->width = width;
    fb->height = height;
    glGenFramebuffers(1, &fb->id);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->id);
    
    //fb->tex = initializeTextureFromMemory(0, width, height, TEXTURE_2D, RGBA, RGBA8, UNSIGNED_BYTE);

    fb->tex = malloc(sizeof(Texture));
    fb->tex->state = initializeSamplerState(CLAMP, GL_NEAREST, GL_NEAREST, 0);
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &fb->tex->texid);
    glBindTexture(TEXTURE_2D, fb->tex->texid);

    fb->tex->target = TEXTURE_2D;
    glTexImage2D(TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fb->tex->texid, 0);

    //Cria o depth buffer
//    glGenRenderbuffers(1, &fb->depthid);
//    glBindRenderbuffer(GL_RENDERBUFFER, fb->depthid);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fb->depthid);

    GLenum drawBufs[] = {GL_NONE};
    glDrawBuffers(1, drawBufs);

//	glDrawBuffer(GL_NONE);
//	glReadBuffer(GL_NONE);

    checkFramebufferStatus(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    return fb;


}

Framebuffer* initializeFramebuffer2(int width, int height) {
    //Inicializa o FBO
    Framebuffer *fb = malloc(sizeof(Framebuffer));
    fb->width = width;
    fb->height = height;
    glGenFramebuffers(1, &fb->id);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->id);
    
    //fb->tex = initializeTextureFromMemory(0, width, height, TEXTURE_2D, RGBA, RGBA8, UNSIGNED_BYTE);

    fb->tex = malloc(sizeof(Texture));
    fb->tex->state = initializeSamplerState(CLAMP, GL_LINEAR, GL_LINEAR, 0);
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


void bindFramebuffer(Framebuffer *fb) {
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

void printGPUMemoryInfo(){

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

void screenshot(char* filename){

	unsigned char* pixels = malloc(sizeof( unsigned char)*r->viewPortWidth*r->viewPortHeight*3);
	glReadPixels(0, 0, r->viewPortWidth, r->viewPortHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	stbi_write_tga(filename, r->viewPortWidth, r->viewPortHeight, 3, pixels);
	free(pixels);

}

