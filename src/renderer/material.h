#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "renderer.h"

//TODO
//1. Shader* PhongShader(ka, kd, ks, shininess, lightpos, lightcolor);
//
Shader* PhongMaterial(vec3 ka, vec3 kd, vec3 ks, float shininess, vec3 lightpos, vec3 lightcolor);
//enum MATERIAL_TYPE {
//    CUSTOM, PHONG,
//};

//FIXME vetores ks, kd, ka e ke tinham tamanho 4
//typedef struct Material{
//    Shader *shdr;

//    Texture *textures[8];
//    unsigned int texCount;

//    void (*bind)(Material *m);

//    struct Material *prev, *next;
	//float shininess;
	//float ks[3];
	//float kd[3];
	//float ka[3];
    //float ke[3];
    //TODO outras texturas (specular map, normal map, etc)
    //Texture *diffmap;
    //int diffsource;
    //Texture *transfermap;
    
    //FIXME so precisa pra hora de ler do arquivo também
//} Material;
//Material* MaterialInitType(int type);
//Material* MaterialInitShader(Shader *shdr);
//void BindMaterial(Material *m);

//void BindMaterial(Material* m, Light* l);

//Material* ColorMaterialDir();
//Material* ColorMaterialDir2();
//Material* ColorMaterialNoLight();
//Material* ColorMaterialLightSimple();
////Material* colorMateralPoint();
//Material* ColorMaterialPhong(vec3 ka, vec3 kd, vec3 ks, float shininess);
//Material* ColorMaterialShadowPhong(vec3 ka, vec3 kd, vec3 ks, float shininess);

//Material* TexturedMaterialDir(Texture *t);
////Material* texturedMaterialPoint();

//Material* VolumeMaterial(Texture* t);
//Material* VolumeMaterialTransfer(Texture* volData, Texture* transferData);



//---------------------------------------
// API LOW LEVEL

//Material* MaterialInit();
//void MaterialAddTex(Material *m, Texture *t);
//void MaterialSetShader(Material *m, Shader *s);


#endif
