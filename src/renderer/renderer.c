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
  

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
typedef GLvoid (APIENTRY *UNIFORM_FUNC)(GLint location, GLsizei count, const void *value);
typedef GLvoid (APIENTRY *UNIFORM_MAT_FUNC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
void *uniformFuncs[CONSTANT_TYPE_COUNT];

void destroyVertexFormat(void *ptr) {
    for(int i = 0; i < ((vertexFormat*)ptr)->numAttrs; i++)
        dlfree(((vertexFormat*)ptr)->attributes);
    dlfree(ptr);
}

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
//FIXME camera global
camera c;

unsigned int tex;
unsigned int cm;
unsigned int normalMap;
unsigned int phong;
scene *duck;

void beginRender(event *e) { 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    mat4 m;
    cameraHandleEvent(&c, e);
    setupViewMatrix(&c, m);
    //glMultMatrixf(m);
    //glTranslated(-c.pos[0], -c.pos[1], -c.pos[2]);
    gluLookAt(c.pos[0], c.pos[1], c.pos[2], c.viewDir[0] + c.pos[0],
              c.viewDir[1] + c.pos[1], c.viewDir[2] + c.pos[2],
              c.up[0], c.up[1], c.up[2]);
    setShaderConstant3f(phong, "eyePos", c.pos);
}

int render(float ifps, event *e, scene *s){
    beginRender(e);
	glTranslatef(0.0, 0.0, -5.0f);
    
    //glTranslatef(0.0, 0.0, -5.0f);
	/*GLUquadric* quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_FILL);
	gluQuadricTexture(quadric, GLU_TRUE);
    gluQuadricOrientation(quadric, GLU_INSIDE);
    gluQuadricNormals(quadric, GLU_SMOOTH);
    gluSphere(quadric,  0.5, 20, 20);*/
    /*
    glNormal3f(0.0, 0.0, 1.0);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(-10.0, -10.0, 0.0);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(10.0, -10.0, 0.0);
        glTexCoord2f(1.0, .0);
        glVertex3f(10.0, 10.0, 0.0);
        glTexCoord2f(0.0, 0.0);
        glVertex3f(-10.0, 10.0, 0.0);
    glEnd();
    glNormal3f(0.0, 1.0, 0.0);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(-10.0, -10.0, 20.0);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(10.0, -10.0, 20.0);
        glTexCoord2f(1.0, .0);
        glVertex3f(10.0, -10.0, 0.0);
        glTexCoord2f(0.0, 0.0);
        glVertex3f(-10.0, -10.0, 0.0);
    glEnd();*/
    fpnode *duckNode = duck->meshes->first;
    mesh *duckMesh = duck->meshes->first->data;
    //bindTexture(1, normalMap);
    bindTexture(1, tex);
    bindTexture(2, cm);
    bindShader(phong);
    triangles *duckTri = duckMesh->tris->first->data;
    drawVBO(duckTri->indicesCount, duckTri->vboId, duckTri->indicesId, duckTri->vertexFormatId );
/*   begin2d();
	bindTexture(0, tex);
	fpnode* iterator = s->nodes->first;
	while ( iterator){
//	gluSphere(quadric,  0.5, 20, 20);
		node* n = iterator->data;
		mesh *m = n->model;
		triangles* t = m->tris->first->data;
		glPushMatrix();
		glTranslatef(n->pos[0], n->pos[1], n->pos[2]);
		drawVBO(t->indicesCount, t->vboId, t->indicesId, t->vertexFormatId );
		glPopMatrix();
//	beginGUI();
//		doButton(NULL, NULL, NULL, 0);
//	endGUI();
		iterator = iterator->next;
	}
	end2d();*/
    glFinish();
    glFlush();
}

renderer* initializeRenderer(int w, int h, float znear, float zfar, float fovy){
	r = (renderer*) dlmalloc(sizeof(renderer));
	//FIXME se usar linkedlist ou map remover os memsets
	//memset(r->vertexFormats, 0, MAX_VERTEX_FORMAT*sizeof(vertexFormat*));
    r->vertexFormats = fparray_init(NULL, destroyVertexFormat, sizeof(vertexFormat));
	//memset(r->textures, 0, MAX_TEXTURES*sizeof(texture*));
    r->textures = fparray_init(NULL, NULL, sizeof(texture));
	//memset(r->shaders, 0, MAX_SHADERS*sizeof(shader*));
    r->shaders = fparray_init(NULL, NULL, sizeof(shader));
	//memset(r->framebuffers, 0, MAX_FRAMEBUFFERS*sizeof(framebuffer*));
    r->framebuffers = fparray_init(NULL, dlfree, sizeof(framebuffer));
	r->fovy = fovy;
	r->zfar = zfar;
	r->znear = znear;
	r->prevTexture = -1;
	r->prevVBO = -1;
	r->prevShader = 0;
	r->viewPortWidth = w;
	r->viewPortHeight = h;
	r->prevFramebuffer = -1;
	vertexAttribute** defaultAttr = dlmalloc(sizeof(vertexAttribute*)*MAX_VERTEX_ATTRS);
	for(int i  = 0; i < MAX_VERTEX_ATTRS; i++)
		defaultAttr[i] = NULL;
	r->prevFormat = addVertexFormat(defaultAttr, 16);

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


	float ratio = (float) w / (float) h;
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(fovy, ratio, znear, zfar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor( 0.7, 0.7, 0.7, 1.0 );
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableClientState(GL_VERTEX_ARRAY);

 	initCamera(&c);
    
    
    //tex = initializeTexture("data/textures/cthulhuship.png", TEXTURE_2D, RGBA, RGBA8, UNSIGNED_BYTE,  (MIPMAP));
    //normalMap = initializeTexture("data/textures/rockwallnormal.tga", TEXTURE_2D, RGB, RGB8, UNSIGNED_BYTE, (MIPMAP | CLAMP_TO_EDGE));
 	//tex = initializeTexture("data/textures/rockwall.tga", TEXTURE_2D, RGB, RGB8, UNSIGNED_BYTE,  (MIPMAP |CLAMP_TO_EDGE));
    tex = initializeTexture("data/textures/duckCM.tga", TEXTURE_2D, RGB, RGB8, UNSIGNED_BYTE,  (MIPMAP |CLAMP_TO_EDGE));
    cm = initializeTexture("data/textures/cm1_%s.jpg", TEXTURE_CUBEMAP, RGB, RGB8, UNSIGNED_BYTE,  (MIPMAP |CLAMP_TO_EDGE));
	//phong = initializeShader( readTextFile("data/shaders/normal_map.vert"), readTextFile("data/shaders/normal_map.frag") );
    //phong = initializeShader( readTextFile("data/shaders/phong.vert"), readTextFile("data/shaders/phong.frag") );
    material m;
    m.flags = PHONG | REFRACT | TEX | ATTENUATION;
    char *vertShader, *fragShader;
    shadergen(m, &vertShader, &fragShader);
    phong = initializeShader( vertShader, fragShader );
    
    float etaRatio = 0.412;
    setShaderConstant1f(phong, "etaRatio", etaRatio);
	float Ka[] = {0.4, 0.4, 0.4, 1.0};
    setShaderConstant4f(phong, "Ka", Ka);
    float Kd[] = {0.5, 0.5, 0.5, 1.0};
    setShaderConstant4f(phong, "Kd", Kd);
    float Ks[] = {0.9, 0.9, 0.9, 1.0};
    setShaderConstant4f(phong, "Ks", Ks);
    float Kc = 0.0;
    setShaderConstant1f(phong, "Kc", Kc);
    float Kl = 0.0;
    setShaderConstant1f(phong, "Kl", Kl);
    float Kq = 0.00001;
    setShaderConstant1f(phong, "Kq", Kq);

    float shininess = 16.0;
    setShaderConstant1f(phong, "shininess", shininess);
    
    float ambientLight[] = { 0.4, 0.4, 0.4, 1.0 };
	setShaderConstant4f(phong, "globalAmbient", ambientLight);
    float color[] = { 0.8, 0.8, 0.8, 1.0 };
	setShaderConstant4f(phong, "LightColor", color);
	float position[] = {0, 0, 500};
	setShaderConstant3f(phong, "LightPosition", position); 
	setShaderConstant3f(phong, "EyePosition", c.pos);
    
    bindShader(phong);
	duck = initializeDae("data/models/duck_triangulate_deindexer.dae");
	createVBO(duck->meshes->first->data);
	printf("initialize gui \n");
	//initializeGUI(800, 600);
	printf("gui done\n");

    shader* shdr = fparray_getdata(1, r->shaders);
    printf("\tNum samplers: %d\n", shdr->numSamplers);
    for(int i = 0; i < shdr->numSamplers; i++) {
        printf("\tsamplers[%d]->location: %d\n", i, shdr->samplers[i]->location);   
    }
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
	glMatrixMode( GL_PROJECTION );
    	glPushMatrix();
        glLoadIdentity();
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


//funcao auxiliar pra tirar o codigo repetido nas duas funcoes abaixo
////TODO melhorar isso =[
void applySamplerState(int target, int flags){

	if ( (flags & CLAMP) ){
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP);
	}
	if ((flags & CLAMP_TO_EDGE) ){
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	if ( (flags & NEAREST) ){
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	if ( (flags & BILINEAR) ){
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	if ( (flags & LINEAR) ){
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	if ( (flags & ANISOTROPY_1) )
		glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0);
	else if ((flags & ANISOTROPY_2) )
		glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2.0);
	else if ((flags & ANISOTROPY_4) )
		glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0);
	else if ((flags & ANISOTROPY_8) )
		glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.0);
	else if ((flags & ANISOTROPY_16) )
		glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0);	

	if (flags & MIPMAP)
		glTexParameteri(target, GL_GENERATE_MIPMAP, GL_TRUE);

}

unsigned int initializeTexture(char* filename, int target, int imageFormat, int internalFormat, int type, int flags){
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(target, textureID);

    texture* prevTex = fparray_getdata(r->prevTexture, r->textures);
    if (r->prevTexture >= 0){
        if ( (flags & CLAMP) && !(prevTex->flags  &CLAMP) ){
            glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP);
        }
        if ((flags & CLAMP_TO_EDGE) && !(prevTex->flags  &CLAMP_TO_EDGE) ){
            glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        }

        if ( (flags & NEAREST) &&  !(prevTex->flags & NEAREST) ){
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
        if ( (flags & BILINEAR) &&  !(prevTex->flags & BILINEAR)){
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }
        if ( (flags & LINEAR) &&  !(prevTex->flags & LINEAR) ){
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }

        if ( (flags & ANISOTROPY_1) && !(prevTex->flags & ANISOTROPY_1))
            glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0);
        else if ((flags & ANISOTROPY_2) && !(prevTex->flags & ANISOTROPY_2))
            glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2.0);
        else if ((flags & ANISOTROPY_4) && !(prevTex->flags & ANISOTROPY_4))
            glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0);
        else if ((flags & ANISOTROPY_8) && !(prevTex->flags & ANISOTROPY_8))
            glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.0);
        else if ((flags & ANISOTROPY_16) && !(prevTex->flags & ANISOTROPY_16))
            glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0);	

        if (flags & MIPMAP)
            glTexParameteri(target, GL_GENERATE_MIPMAP, GL_TRUE);
    }else
        applySamplerState(target, flags);

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
            printf("TEXTURE_CUBEMAP\n");
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
    tex->flags = flags;
    tex->id = textureID;
    tex->target = target;

    //r->textures[textureID] = tex;

    fparray_inspos(tex, textureID, r->textures);

    if (r->prevTexture >= 0)
        bindTexture(0, r->prevTexture);

    return textureID;
}

unsigned int initializeTextureFromMemory(void* data, int x, int y, int target, int imageFormat, int internalFormat, int type, int flags){


	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(target, textureID);

    texture* prevTex = fparray_getdata(r->prevTexture, r->textures);
	if (r->prevTexture >= 0){
		if ( (flags & CLAMP) && !(prevTex->flags  &CLAMP) ){
			glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP);
		}
		if ((flags & CLAMP_TO_EDGE) && !(prevTex->flags  &CLAMP_TO_EDGE) ){
			glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}

		if ( (flags & NEAREST) &&  !(prevTex->flags & NEAREST) ){
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
		if ( (flags & BILINEAR) &&  !(prevTex->flags & BILINEAR)){
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		if ( (flags & LINEAR) &&  !(prevTex->flags & LINEAR) ){
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}

		if ( (flags & ANISOTROPY_1) && !(prevTex->flags & ANISOTROPY_1))
			glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0);
		else if ((flags & ANISOTROPY_2) && !(prevTex->flags & ANISOTROPY_2))
			glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2.0);
		else if ((flags & ANISOTROPY_4) && !(prevTex->flags & ANISOTROPY_4))

			glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0);
		else if ((flags & ANISOTROPY_8) && !(prevTex->flags & ANISOTROPY_8))
			glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.0);
		else if ((flags & ANISOTROPY_16) && !(prevTex->flags & ANISOTROPY_16))
			glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0);	

		if (flags & MIPMAP)
			glTexParameteri(target, GL_GENERATE_MIPMAP, GL_TRUE);
	}else
		applySamplerState(target, flags);

	if ((target == TEXTURE_2D) || (target == TEXTURE_RECTANGLE) ){
		glTexImage2D(target, 0, internalFormat, x, y, 0, imageFormat, type, data);
	}

	texture* tex = (texture*) dlmalloc(sizeof(texture));
	tex->flags = flags;
	tex->id = textureID;
	tex->target = target;

	//r->textures[textureID] = tex;
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

		glActiveTexture(GL_TEXTURE0 + slot);

		if (tex->id != prevTex->id )
			glBindTexture(tex->target, tex->id );

		if ( (tex->flags & CLAMP) && !(prevTex->flags  &CLAMP) ){
			glTexParameteri(tex->target, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(tex->target, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(tex->target, GL_TEXTURE_WRAP_R, GL_CLAMP);
		}
		if ((tex->flags & CLAMP_TO_EDGE) && !(prevTex->flags  &CLAMP_TO_EDGE) ){
			glTexParameteri(tex->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(tex->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(tex->target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}

		if ( (tex->flags & NEAREST) &&  !(prevTex-> flags & NEAREST) ){
			glTexParameteri(tex->target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(tex->target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
		else if ( (tex->flags & BILINEAR) &&  !(prevTex->flags & BILINEAR)){
			glTexParameteri(tex->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(tex->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
		else if ( (tex->flags & LINEAR) &&  !(prevTex->flags & LINEAR) ){
			glTexParameteri(tex->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(tex->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}

		
		//TODO uma maneira mais simples de 
		if ( ((prevTex->flags &ANISOTROPY_2) || (prevTex->flags &ANISOTROPY_4) || (prevTex->flags &ANISOTROPY_8) || (prevTex->flags &ANISOTROPY_16) ) && ( !((tex->flags & ANISOTROPY_2) || (tex->flags & ANISOTROPY_4) || (tex->flags & ANISOTROPY_8) || (tex->flags & ANISOTROPY_16)  )  )){
			glTexParameterf(tex->target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 0.0);
			printf("anterior tinha anisotropy e atual nao tem, desativando \n");

		}else{
			if ( (tex->flags & ANISOTROPY_1) && !(prevTex->flags & ANISOTROPY_1))
				glTexParameterf(tex->target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0);
			else if ((tex->flags & ANISOTROPY_2) && !(prevTex->flags & ANISOTROPY_2))
				glTexParameterf(tex->target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2.0);
			else if ((tex->flags & ANISOTROPY_4) && !(prevTex->flags & ANISOTROPY_4))
				glTexParameterf(tex->target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0);
			else if ((tex->flags & ANISOTROPY_8) && !(prevTex->flags & ANISOTROPY_8))
				glTexParameterf(tex->target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.0);
			else if ((tex->flags & ANISOTROPY_16) && !(prevTex->flags & ANISOTROPY_16)) 
				glTexParameterf(tex->target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0);	
		}


		r->prevTexture = id;
	}

}

void killVBO(unsigned int id){
	glDeleteBuffers(1, &id);
}

unsigned int initializeVBO(unsigned int size, const void* data){
	unsigned int vboID;
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	//TODO se precisar de outro mode alem do STATIC_DRAW ?
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, r->prevVBO); //volta
	return vboID;
}

//TODO usar draw rande elements pros indices
void drawVBO(unsigned int triCount, unsigned int vertexID, unsigned int indicesID, int formatID){


	//vertexFormat* current = r->vertexFormats[formatID];
    vertexFormat* current = fparray_getdata(formatID, r->vertexFormats);
	//vertexFormat* prev = r->vertexFormats[r->prevFormat];
	vertexFormat* prev = fparray_getdata(r->prevFormat, r->vertexFormats);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);

	//primeiro ativa e desativa as vertex attrib arrays necessarias
	for ( unsigned int i = 0; i < MAX_VERTEX_ATTRS; i++ ){
		if ( current->attributes[i] ){
			if ( !prev->attributes[i] ){
				printf("enabling: %d \n",  i);
				glEnableVertexAttribArray(i);
			}
		}
		if ( !current->attributes[i] && prev->attributes[i]  ){
			printf("disabling: %d \n", i);
			glDisableVertexAttribArray(i);
		}
	}

	//seta o attrib pointer, so necessario caso nao tenha sido feito antes
	if ( r->prevVBO != vertexID ){
		r->prevVBO = vertexID;
		for ( unsigned int i = 0; i < MAX_VERTEX_ATTRS; i++ )
			if ( current->attributes[i]){
				printf("vertex attrib pointer: %d %d \n", i, current->attributes[i]->components);
				glVertexAttribPointer(i, current->attributes[i]->components, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(current->attributes[i]->offset));
				}
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesID);
	//TODO pode ser interessante usar GL_SHORT
	glDrawElements(GL_TRIANGLES, triCount, GL_UNSIGNED_INT, NULL);

	r->prevFormat = formatID;
}

unsigned int addVertexFormat(vertexAttribute** attrs,  unsigned int num){

	vertexFormat* format = (vertexFormat*) dlmalloc(sizeof(vertexFormat));
	format->attributes = (vertexAttribute*) dlmalloc(sizeof(vertexAttribute*[MAX_VERTEX_ATTRS]));
	format->numAttrs = num;
	format->attributes = attrs;
	/*unsigned int indice;
	//pega o primeiro valor do array de ponteiros que esta vazio
	for (unsigned int i = 0; i < MAX_VERTEX_FORMAT; i++){
		if (!r->vertexFormats[i]){
			indice = i;
			break;
		}
	}
	r->vertexFormats[indice] = format;
	return indice;*/
    return fparray_insback(format, r->vertexFormats);
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
				if (strcmp(uni->name, "LightPosition") == 0)
					uni->semantic = LIGHTPOS;
				if (strcmp(uni->name, "EyePosition") == 0)
					uni->semantic = EYEPOS;
				if (strcmp(uni->name, "Timer") == 0)
					uni->semantic = TIME;
				newShader->uniforms[numUniforms] = uni;
				numUniforms++;
			}
		}
	}

	dlfree(name);
	//r->shaders[shaderProgram] = newShader;
    fparray_inspos(newShader, shaderProgram, r->shaders);
	glUseProgram(r->prevShader);
	
	return shaderProgram;
}

void bindShader(unsigned int program){
	if (program != r->prevShader){
		r->prevShader = program;
		glUseProgram(program);
	}
    shader *shdr = fparray_getdata(program, r->shaders);
	for(unsigned int i = 0; i < shdr->numUniforms; i++ ){
		if (shdr->uniforms[i]->dirty ){
			shdr->uniforms[i]->dirty = 0;
			if (shdr->uniforms[i]->type >= CONSTANT_MAT2){
				((UNIFORM_MAT_FUNC) uniformFuncs[shdr->uniforms[i]->type])(shdr->uniforms[i]->location, shdr->uniforms[i]->size, GL_TRUE, (float *) shdr->uniforms[i]->data);
			} else {
				((UNIFORM_FUNC) uniformFuncs[shdr->uniforms[i]->type])(shdr->uniforms[i]->location, shdr->uniforms[i]->size, shdr->uniforms[i]->data);
				//((UNIFORM_FUNC) uniformFuncs[2])(0, 1, color);
				//glUniform4fv(shaders[program]->uniforms[i]->location, shaders[program]->uniforms[i]->size, (GLfloat*) shaders[program]->uniforms[i]->data);
			}
		}else if ( shdr->uniforms[i]->semantic == EYEPOS){
			setShaderConstant3f(program, "EyePosition",  c.pos);
            printf("c.pos: %f, %f, %f\n", c.pos[0], c.pos[1], c.pos[2]);
		//}else if (r->shaders[program]->uniforms[i]->semantic == TIME)
			//setShaderConstant1f(program, "Time", TIMER::getInstance().getElapsedTime());
		//	  setShaderConstant1f(program, "Time", 0.1);
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

unsigned int initializeFramebuffer(void* data, int width, int height, int format, int internalFormat, int type, int  flags){

	
	unsigned int texid = initializeTextureFromMemory(data, width,  height, TEXTURE_2D, format, internalFormat, type, flags);
	unsigned int id;
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texid, 0);

	//r->framebuffers[id] = id;
    framebuffer *fb = dlmalloc(sizeof(framebuffer));
    fb->id = id;
    fb->width = width;
    fb->height = height;

    fparray_inspos(fb, id, r->framebuffers);

	bindMainFramebuffer();

}

void bindFramebuffer(int id){
	if (r->prevFramebuffer != id){
		r->prevFramebuffer = id;
		glBindFramebuffer(GL_FRAMEBUFFER, id);
        framebuffer* fb = fparray_getdata(id, r->framebuffers);
		glViewport(0, 0, fb->width,  fb->height); 
	}
}

void bindMainFramebuffer(){
	r->prevFramebuffer = 0;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, r->viewPortWidth, r->viewPortHeight);
}
