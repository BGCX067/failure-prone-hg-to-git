#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "fparray.h"
#include "renderer.h"
#include "vec3.h"
#include "matrix.h"
#include "mesh.h"

enum {
	FLIP_X = 1 << 1,
	FLIP_Y = 1 << 2,
	REPEAT_LAST = 1 << 3
};

typedef struct frames_{

    Texture **images;
    Texture *normal;
	float delay;
	int numImages;

	float timeCounter;
	int currentImage;

} frames;


typedef struct sprite_{

	fparray* frames;
	int lastFrame;

    vec3 pos;
    float h, w;

    mat4 transform;
    Mesh *m;

    Shader *shdr;

} sprite;

sprite* initializeSprite(float x, float y, float sizex, float sizey, Shader *s);
//adiciona varios sprites de uma vez
int addSprites(sprite* s, char* path, int numframes, float delay);
//adiciona 1 frame composto por 1 imagem
int addSprite(sprite* s, char* filename, float delay);
void drawSprite(sprite* s, float elapsedtime, int framenum, int flags);

void translateSprite(sprite *s, float tx, float ty);

#endif

