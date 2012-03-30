#include "sprite.h"
#include "../renderer/renderer.h"
#include <stdlib.h>

static Shader* spriteShader = NULL;


void initializeSpriteShaders(){

    const char* SpriteVSSource = {
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
    "};

    spriteShader = initializeShader( SpriteVSSource, SpriteFSSource );

}

sprite* initializeSprite(){

	if (spriteShader == NULL){
		initializeSpriteShaders();
	}

	sprite* anim = malloc(sizeof(sprite));
	anim->frames = fparray_init(NULL, free, sizeof(frames*));
	anim->lastFrame = -1;
	
	return anim;
}


int addSprites(sprite* s, char* path, int numframes, float delay){

	frames* frame = malloc(sizeof(frames));
	frame->delay = delay;
	frame->images = malloc(sizeof(int)*numframes);
	frame->numImages = numframes;
	frame->timeCounter = 0;
	frame->currentImage = 0;

	int i;

	for (i = 1; i <= numframes; i++){ //comeca com 1 pq nao tem imagem com 0 no filename
		char filename[50];
		sprintf(filename, path, i);
		//printf("filename: %s \n", filename);
		//checar o initialize
		frame->images[i-1] = initializeTexture(filename, TEXTURE_2D, RGBA, RGBA8, UNSIGNED_BYTE );
		printf(" %s %d %d \n ", filename, i, frame->images[i-1]);
	}

	//check antes
	return fparray_insback(frame, s->frames);

}

//enum pode ser negativo ou maior que o numero de frames
void drawSprite(sprite* s, int x, int y, float elapsedtime, int framenum, int flags){

	rect r;
	r.x = x;
	r.y = y;
	r.w = 100;
	r.h = 100;

	frames* f = fparray_getdata(framenum, s->frames);
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	bindTexture(0, f->images[f->currentImage]);
	bindShader(spriteShader);

	float texcoords[] = {1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };	
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

   	glBegin(GL_QUADS);
		glTexCoord2f(  (float) texcoords[0], (float) texcoords[1]);
        	glVertex2f(r.x, r.y );

		glTexCoord2f( (float) texcoords[2], (float) texcoords[3]);
        	glVertex2f(r.x , r.y + r.h);

		glTexCoord2f( (float) texcoords[4], (float) texcoords[5]);
        	glVertex2f(r.x + r.w, r.y + r.h);

		glTexCoord2f( (float) texcoords[6], (float) texcoords[7]);
        	glVertex2f( r.x + r.w, r.y );
    	glEnd();
	bindShader(0);
	glDisable(GL_BLEND);
	s->lastFrame = framenum;

}
