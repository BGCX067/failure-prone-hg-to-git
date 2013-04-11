#ifndef _LIGHT_H_
#define _LIGHT_H_

typedef struct Light{
	float pos[3];
    float dir[3];
	float color[4];
    struct Light *prev, *next;
}Light;

#endif
