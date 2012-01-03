#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "light.h"

typedef struct material_{
	float shininess;
	float ks[4];
	float kd[4];
	float ka[4];

	unsigned int shaderid;
}Material;

void bindMaterial(Material* m, Light* l);

#endif
