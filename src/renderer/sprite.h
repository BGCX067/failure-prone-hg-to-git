#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "util/utarray.h"
#include "renderer.h"
#include "vec3.h"
#include "matrix.h"
#include "mesh.h"

enum {
	FLIP_X = 1 << 1,
	FLIP_Y = 1 << 2,
	REPEAT_LAST = 1 << 3
};

typedef struct sprite_{
    UT_array *frames;
	int lastFrame;

    vec3 pos;
    float h, w;

    mat4 transform;
    Mesh *m;

    Shader *shdr;

} Sprite;


Sprite* InitializeSprite(float x, float y, float sizex, float sizey, Shader *s);
//adiciona varios sprites de uma vez
void AddSprites(Sprite* s, char* path, int numframes, float delay);
//adiciona 1 frame composto por 1 imagem
void AddSprite(Sprite* s, char* filename, int nrm, float delay);
void DrawSprite(Sprite* s, float elapsedtime, int framenum, int flags);

void TranslateSprite(Sprite *s, float tx, float ty);

#endif
