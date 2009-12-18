
#include "renderer.h"
#include "camera.h"
#include "math/matrix.h"
#include "../glapp.h"
#include "../util/image.h"
#include "../util/malloc.h"
#include <GL/gl.h>
#include <GL/glu.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

renderer* r;
camera c;

unsigned int tex;

void beginRender(event e) { 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    mat4 m;
    cameraHandleEvent(&c, e);
    setupViewMatrix(&c, m);
    //glLoadMatrixf(m);
    gluLookAt(c.pos[0], c.pos[1], c.pos[2], c.viewDir[0] + c.pos[0],
              c.viewDir[1] + c.pos[1], c.viewDir[2] + c.pos[2],
              c.up[0], c.up[1], c.up[2]);
}

int render(event e){

    beginRender(e);
	glTranslatef(0.0, 0.0, -5.0f);
	GLUquadric* quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_FILL);
	gluQuadricTexture(quadric, GLU_TRUE);
	bindTexture(0, tex);
	gluSphere(quadric,  0.5, 20, 20);

    glFinish();
    glFlush();
}

renderer* initializeRenderer(int w, int h, float znear, float zfar, float fovy){

	r = (renderer*) dlmalloc(sizeof(renderer));
	//FIXME se usar linkedlist ou map remover os memsets
	memset(r->vertexFormats, 0, MAX_VERTEX_FORMAT*sizeof(vertexFormat*));
	memset(r->textures, 0, MAX_TEXTURES*sizeof(texture*));
	r->fovy = fovy;
	r->zfar = zfar;
	r->znear = znear;
	r->prevTexture = -1;
	r->prevVBO = -1;
	r->prevFormat = -1;

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
    tex = initializeTexture("data/textures/duckCM.tga", TEXTURE_2D, RGB, RGB8, UNSIGNED_BYTE,  (MIPMAP |CLAMP_TO_EDGE)); 
	return r;

}

unsigned int initializeTexture(char* filename, int target, int imageFormat, int internalFormat, int type, int flags){

	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(target, textureID);

	if (flags & CLAMP){
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP);
	}
	else if (flags & CLAMP_TO_EDGE) {
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	if (flags & MIPMAP)
		glTexParameteri(target, GL_GENERATE_MIPMAP, GL_TRUE);

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
			 	//TODO nao eh pra ser +5 no buff
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

	r->textures[textureID] = tex;

	if (r->prevTexture >= 0)
		bindTexture(0, r->prevTexture);

	return textureID;

}

void setTextureFilter(int target, int flags){

	if ( flags & NEAREST ){
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	if ( flags & BILINEAR){
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	if ( flags & LINEAR ){
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	if (flags & ANISOTROPY_1)
		glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0);
	else if (flags & ANISOTROPY_2)
		glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2.0);
	else if (flags & ANISOTROPY_4)
		glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0);
	else if (flags & ANISOTROPY_8)
		glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.0);
	else if (flags & ANISOTROPY_16)
		glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0);	
 
}

void bindTexture(int slot, int id){
	if ( r->prevTexture == -1 ){
		r->prevTexture = id;
		glEnable( r->textures[id]->target );
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(r->textures[id]->target, r->textures[id]->id );
	}else{

		if ( r->textures[id]->target != r->textures[r->prevTexture]->target ){
			glDisable(r->textures[r->prevTexture]->target);
			glEnable( r->textures[id]->target );
		}

		glActiveTexture(GL_TEXTURE0 + slot);

		if ( r->textures[id]->id != r->textures[r->prevTexture]->id )
			glBindTexture(r->textures[id]->target, r->textures[id]->id );

		if ( (r->textures[id]->flags & CLAMP) && !(r->textures[r->prevTexture]->flags  &CLAMP) ){
			glTexParameteri(r->textures[id]->target, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(r->textures[id]->target, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(r->textures[id]->target, GL_TEXTURE_WRAP_R, GL_CLAMP);
		}
		if ((r->textures[id]->flags & CLAMP_TO_EDGE) && !(r->textures[r->prevTexture]->flags  &CLAMP_TO_EDGE) ){
			glTexParameteri(r->textures[id]->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(r->textures[id]->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(r->textures[id]->target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
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

	vertexFormat* current = r->vertexFormats[formatID];
	vertexFormat* prev = r->vertexFormats[r->prevFormat];

	glBindBuffer(GL_ARRAY_BUFFER, vertexID);

	//primeiro ativa e desativa as vertex attrib arrays necessarias
	for ( unsigned int i = 0; i < MAX_VERTEX_ATTRS; i++ ){
		if ( current->attributes[i] ){
			if ( !prev->attributes[i] )
				glEnableVertexAttribArray(i);
		}
		if ( !current->attributes[i] && prev->attributes[i]  ){
			glDisableVertexAttribArray(i);
		}
	}

	//seta o attrib pointer, so necessario caso nao tenha sido feito antes
	if ( r->prevVBO != vertexID ){
		r->prevVBO = vertexID;
		for ( unsigned int i = 0; i < MAX_VERTEX_ATTRS; i++ )
			if ( current->attributes[i])
				glVertexAttribPointer(i, current->attributes[i]->components, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(current->attributes[i]->offset));
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
	unsigned int indice;
	//pega o primeiro valor do array de ponteiros que esta vazio
	for (unsigned int i = 0; i < MAX_VERTEX_FORMAT; i++){
		if (!r->vertexFormats[i]){
			indice = i;
			break;
		}
	}
	r->vertexFormats[indice] = format;
	return indice;
}
