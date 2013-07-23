#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "light.h"
#include "renderer.h"

enum MatComponentSource {
    VEC, TEXTURE
};

typedef struct Material{
	float shininess;
	float ks[4];
	float kd[4];
	float ka[4];
    float ke[4];

    //TODO outras texturas (specular map, normal map, etc)
    Texture *diffmap;
    int diffsource;
    Texture *transfermap;
    Texture *textures[8];
    unsigned int texCount;

    //FIXME so precisa pra hora de ler do arquivo também
    char *id;

    Shader *shdr;

    struct Material *prev, *next;
}Material;

void bindMaterial(Material* m, Light* l);

Material* colorMaterialDir();
Material* colorMaterialDir2();
Material* colorMaterialNoLight();
Material* colorMaterialLightSimple();
//Material* colorMateralPoint();
Material* colorMaterialPhong(vec3 ka, vec3 kd, vec3 ks, float shininess);
Material* colorMaterialShadowPhong(vec3 ka, vec3 kd, vec3 ks, float shininess);

Material* texturedMaterialDir(Texture *t);
//Material* texturedMaterialPoint();

Material* volumeMaterial(Texture* t);
Material* volumeMaterialTransfer(Texture* volData, Texture* transferData);



//---------------------------------------
// API LOW LEVEL

Material* materialInit();
void materialAddTex(Material *m, Texture *t);
void materialSetShader(Material *m, Shader *s);


#endif
