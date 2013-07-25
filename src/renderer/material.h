#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "light.h"
#include "renderer.h"

enum MatComponentSource {
    VEC, TEXTURE
};

typedef struct Material{
	float shininess;
	float ks[3];
	float kd[3];
	float ka[3];
    float ke[3];

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

void BindMaterial(Material* m, Light* l);

Material* ColorMaterialDir();
Material* ColorMaterialDir2();
Material* ColorMaterialNoLight();
Material* ColorMaterialLightSimple();
//Material* colorMateralPoint();
Material* ColorMaterialPhong(vec3 ka, vec3 kd, vec3 ks, float shininess);
Material* ColorMaterialShadowPhong(vec3 ka, vec3 kd, vec3 ks, float shininess);

Material* TexturedMaterialDir(Texture *t);
//Material* texturedMaterialPoint();

Material* VolumeMaterial(Texture* t);
Material* VolumeMaterialTransfer(Texture* volData, Texture* transferData);



//---------------------------------------
// API LOW LEVEL

Material* MaterialInit();
void MaterialAddTex(Material *m, Texture *t);
void MaterialSetShader(Material *m, Shader *s);


#endif
