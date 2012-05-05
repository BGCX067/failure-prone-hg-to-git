#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "fparray.h"
#include "renderer.h"

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

} sprite;

sprite* initializeSprite();
//adiciona varios sprites de uma vez
int addSprites(sprite* s, char* path, int numframes, float delay);
//adiciona 1 frame composto por 1 imagem
int addSprite(sprite* s, char* filename, float delay);
void drawSprite(sprite* s, float x, float y, float sizex, float sizey,  float elapsedtime, int framenum, int flags);

#endif

