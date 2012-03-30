#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "fparray.h"

enum {
	FLIP_X = 1 << 1,
	FLIP_Y = 1 << 2,
	REPEAT_LAST = 1 << 3
};

typedef struct frames_{

	int* images;
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
int addSprites(sprite* s, char* path, int numframes, float delay);
void drawSprite(sprite* s, int x, int y, float elapsedtime, int framenum, int flags);

#endif

