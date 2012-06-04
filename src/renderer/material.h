#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "light.h"
#include "renderer.h"

enum MatComponentSource {
    VEC, TEXTURE
};

typedef struct material_{
	float shininess;
	float ks[4];
	float kd[4];
	float ka[4];
    float ke[4];

    //TODO outras texturas (specular map, normal map, etc)
    Texture *diffmap;
    int diffsource;


    //FIXME so precisa pra hora de ler do arquivo também
    char *id;

    Shader *shdr;

}Material;

void bindMaterial(Material* m, Light* l);

Material* colorMaterialDir();
Material* colorMateralPoint();

Material* texturedMaterialDir();
Material* texturedMaterialPoint();

#endif
