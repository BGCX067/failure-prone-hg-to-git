#include "sprite.h"
#include "../renderer/renderer.h"
#include <stdlib.h>
#include "scene.h"
#include "../math/matrix.h"
#include "camera.h"

static Shader* spriteShader = NULL;
mat4 ortho;

void initializeSpriteShaders(){

/*    const char* SpriteVSSource = {
    "#version 120\n\
    \n\
    void main()\n\
    {\n\
        gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n\
        gl_TexCoord[0] = gl_MultiTexCoord0;\n\
    }\n\
    "};

    const char* SpriteFSSource = {
    "#version 120\n\
    uniform sampler2D samp;\n\
    \n\
    void main()\n\
    {\n\
        vec4 texel;\n\
        texel = texture2D( samp, gl_TexCoord[0].xy);\n\
	gl_FragColor = vec4(texel.rgba); \n\
    }\n\
    "};*/

    const char* SpriteVSSource = {
    "#version 330\n\
    layout(location = 0) in vec3 inpos;\n\
    layout(location = 8) in vec2 intexcoord;\n\
    out vec2 texcoord;\n\
    uniform mat4 ortho;\n\
    void main(void)\n\
    {\n\
        texcoord = intexcoord;\n\
        gl_Position = ortho * vec4(inpos, 1.0);\n\
    }\n\
    "};

    const char* SpriteFSSource = {
    "#version 330\n\
    in vec2 texcoord;\n\
    out vec4 outcolor;\n\
    uniform sampler2D tex;\n\
    void main() {\n\
        outcolor = texture(tex, texcoord);\n\
    }\n\
    "};


    //spriteShader = initializeShader( SpriteVSSource, SpriteFSSource );
    //spriteShader = initializeShader(readTextFile("data/shaders/phong.vert"), 
        //                            readTextFile("data/shaders/phong.frag"));

}

sprite* initializeSprite(float x, float y, float sizex, float sizey, Shader *s){
	sprite* anim = malloc(sizeof(sprite));
	anim->frames = fparray_init(NULL, free, sizeof(frames*));
	anim->lastFrame = -1;

    anim->pos[0] = x;
    anim->pos[1] = y;
    anim->pos[2] = 0.0f;
    anim->h = sizey;
    anim->w = sizex;

    anim->m = initMesh();
    Triangles *t = addTris(anim->m);

    float vertices[] = {x, y, 0.0f,
                        x, y + sizey, 0.0,
                        x + sizex, y + sizey, 0.0,
                        x + sizex, y, 0.0};

    float texcoords[] = {1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };
    unsigned int indices[] = {0, 1, 2, 0, 2, 3};

    addVertices(t, 12, 3, vertices);
    addIndices(t, 6, indices);
    addTexCoords(t, 8, 2, 0, texcoords);
    prepareMesh(anim->m);

    fpIdentity(anim->transform);

    anim->shdr = s;
	
    return anim;
}

int addSprite(sprite* s, char* filename, int nrm, float delay){
	if (s == NULL || filename == NULL)
		return 0;

	frames* frame = malloc(sizeof(frames));
	frame->delay = delay;
	frame->images = malloc(sizeof(Texture*)*1);
	frame->numImages = 1;
	frame->timeCounter = 0;
	frame->currentImage = 0;
    frame->normal = NULL;

	frame->images[0] = initialize2DTexture(filename);
	if (frame->images[0] == NULL){
		free(frame);
		printf("File not found: %s \n", filename);
		return 0;
	}

    if(nrm == 1) {
        //Verificar se strtok muda filename
        char origfilename[strlen(filename) + 1];
        strcpy(origfilename, filename);
        char *nrmExt = "_NRM.png";
        char *prefix = strtok(origfilename, ".");
        char nrmFilename[strlen(nrmExt) + strlen(origfilename) + 1];
        strcat(nrmFilename, origfilename);
        strcat(nrmFilename, nrmExt);
        frame->normal = initializeTexture(nrmFilename, TEXTURE_2D, RGB, RGB8, UNSIGNED_BYTE);
    }

	return fparray_insback(frame, s->frames);	

}

int addSprites(sprite* s, char* path, int numframes, float delay){

	frames* frame = malloc(sizeof(frames));
	frame->delay = delay;
    frame->images = malloc(sizeof(Texture*)*numframes);
	frame->numImages = numframes;
	frame->timeCounter = 0;
	frame->currentImage = 0;
    frame->normal = NULL;

	int i;

	for (i = 1; i <= numframes; i++){ //comeca com 1 pq nao tem imagem com 0 no filename
		char filename[50];
		sprintf(filename, path, i);
		//printf("filename: %s \n", filename);
		//checar o initialize
		frame->images[i-1] = initializeTexture(filename, TEXTURE_2D, RGBA, RGBA8, UNSIGNED_BYTE );
		if (frame->images[i-1] == NULL){
			free(frame);
			return 0;
		}
		printf(" %s %d %d \n ", filename, i, frame->images[i-1]->texid);
	}

//    frame->normal = initializeTexture("data/sprites/normalmap.png", TEXTURE_2D, RGB, RGB8, UNSIGNED_BYTE);

	//check antes
	return fparray_insback(frame, s->frames);

}

//enum pode ser negativo ou maior que o numero de frames
void drawSprite(sprite* s, float elapsedtime, int framenum, int flags){
	frames* f = fparray_getdata(framenum, s->frames);
	if (f == NULL){
	//	printf("Frame not found: %d \n", s->frames->size);
		return;
	}
   	f->timeCounter += elapsedtime;

	if (framenum == 1)
	    printf("currrent frame: %d previous frame: %d current image %d \n",  framenum, s->lastFrame, f->currentImage);

	if (f->timeCounter > f->delay){
		if ( (flags & REPEAT_LAST) && (f->currentImage == f->numImages-1) && (s->lastFrame == framenum) )
			f->currentImage = f->numImages-1;
		else
			f->currentImage++;
		f->timeCounter = 0;
	}

	if ( (f->currentImage >= f->numImages) ){
		if ( (flags & REPEAT_LAST) && (f->currentImage == f->numImages-1) && (s->lastFrame == framenum) )
			f->currentImage = f->numImages-1;
		else
			f->currentImage = 0;
	}
    
    Triangles *tri = s->m->tris->first->data;
    float *texcoords = mapVBO(tri->texVBO[0], GL_WRITE_ONLY);
    texcoords[0] = 1.0; texcoords[1] = 1.0; texcoords[2] = 1.0; texcoords[3] = 0.0;
    texcoords[4] = 0.0; texcoords[5] = 0.0; texcoords[6] = 0.0; texcoords[7] = 1.0;  
    if ( (flags & FLIP_Y) && (flags & FLIP_X) ){
		texcoords[0] = 0.0;
		texcoords[1] = 0.0;
		texcoords[2] = 0.0;
		texcoords[3] = 1.0;
		texcoords[4] = 1.0;
		texcoords[5] = 1.0;
		texcoords[6] = 1.0;
		texcoords[7] = 0.0;
	}else if (flags & FLIP_Y){
		texcoords[0] = 0.0;
		texcoords[1] = 1.0;
		texcoords[2] = 0.0;
		texcoords[3] = 0.0;
		texcoords[4] = 1.0;
		texcoords[5] = 0.0;
		texcoords[6] = 1.0;
		texcoords[7] = 1.0;
	}else if (flags & FLIP_X){
		texcoords[0] = 1.0;
		texcoords[1] = 0.0;
		texcoords[2] = 1.0;
		texcoords[3] = 1.0;
		texcoords[4] = 0.0;
		texcoords[5] = 1.0;
		texcoords[6] = 0.0;
		texcoords[7] = 0.0;
	}
    unmapVBO(tri->texVBO[0]);

	bindSamplerState(f->images[f->currentImage]->state, 0);
	bindTexture( f->images[f->currentImage], 0);

    if(f->normal) {
        bindSamplerState(f->normal->state, 1);
        bindTexture( f->normal, 1);
    }
	
    extern Camera c;
	setShaderConstant4x4f(s->shdr, "mvp", c.mvp);
	setShaderConstant4x4f(s->shdr, "modelTransform", s->transform);
	bindShader(s->shdr);
    drawIndexedVAO(tri->vaoId, tri->indicesCount, GL_TRIANGLES);
	s->lastFrame = framenum;
    bindShader(0);
}


void translateSprite(sprite *s, float tx, float ty) {
    fpIdentity(s->transform);
    fptranslatef(s->transform, tx, ty, 0.0f);
    s->pos[0] = tx;
    s->pos[1] = ty;
}

