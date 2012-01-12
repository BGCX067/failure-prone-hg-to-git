#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "light.h"
#include "renderer.h"

/*typedef struct {
    //OpenGL related
    unsigned int texid;
    unsigned int state;
    unsigned int target;

    //TODO: Remover essas variáveis
    unsigned char *data;
    unsigned int width, height;

    //FIXME isso aqui não é realmente necessário, só na hora de ler do arquivo
    char* id;
}Texture;*/

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

	unsigned int shaderid;
}Material;

void bindMaterial(Material* m, Light* l);

#endif
