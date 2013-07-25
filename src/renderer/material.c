#include "material.h"
#include "renderer.h"
#include <stdlib.h>
#include <string.h>
#include "util/textfile.h"

const char* colorDirVertex = {
"#version 330 \n\
layout(location = 0) in vec3 inpos; \n\
layout(location = 2) in vec3 innormal; \n\
out vec3 vertexpos; \n\
out vec3 normal; \n\
uniform mat4 modelview; \n\
uniform mat3 normalmatrix; \n\
uniform mat4 mvp; \n\
void main(void) \n\
{ \n\
    normal = normalize(normalmatrix*innormal); \n\
    vertexpos = vec3(modelview*vec4(inpos, 1.0)); \n\
    gl_Position = mvp*vec4(inpos, 1.0); \n\
} \n\
"};

const char* colorDirFrag = {
"#version 330 \n\
in vec3 vertexpos; \n\
in vec3 normal; \n\
out vec4 outcolor; \n\
uniform vec3 LightPosition; \n\
uniform vec3 LightColor; \n\
uniform vec3 ka; \n\
uniform vec3 ks; \n\
uniform vec3 kd; \n\
uniform float shininess; \n\
uniform vec3 eyepos; \n\
void main() { \n\
    vec3 n = normalize(normal); \n\
    vec3 s = normalize(LightPosition - vertexpos); \n\
    vec3 v = normalize(eyepos - vertexpos); \n\
    vec3 r = reflect(-v, n); \n\
    //vec3 kd = vec3(1.0, 0.0, 0.0); \n\
    vec3 outcolor3 = LightColor*(kd*max(dot(s, n), 0.0)); \n\
    float gamma = 2.2; \n\
    outcolor = vec4(pow(outcolor3, vec3(1.0/gamma)), 1.0); \n\
    //outcolor = vec4(n, 1.0); \n\
}\n\
"};

const char* colorNoLightVertex = {
"#version 330 \n\
layout(location = 0) in vec3 inpos; \n\
layout(location = 3) in vec4 incolor; \n\
out vec4 color; \n\
uniform mat4 mvp; \n\
void main(void) \n\
{ \n\
    color = incolor; \n\
    gl_Position = mvp*vec4(inpos, 1.0); \n\
} \n\
"};

const char* colorNoLightFrag = {
"#version 330 \n\
in vec4 color; \n\
out vec4 outcolor; \n\
void main() { \n\
    float gamma = 2.2; \n\
    //outcolor = vec4(pow(color.rgb, vec3(1.0/gamma)), color.a); \n\
    outcolor = color;\n\
}\n\
"};

const char* colorLightSVertex = {
"#version 330 \n\
layout(location = 0) in vec3 inpos; \n\
layout(location = 2) in vec3 innormal; \n\
layout(location = 3) in vec4 incolor; \n\
out vec3 vertexpos; \n\
out vec4 color; \n\
out vec3 normal; \n\
uniform mat4 mvp; \n\
uniform mat3 normalmatrix; \n\
uniform mat4 modelview; \n\
void main(void) \n\
{ \n\
    color = incolor; \n\
    normal = normalize(normalmatrix*innormal); \n\
    vertexpos = vec3(modelview*vec4(inpos, 1.0)); \n\
    gl_Position = mvp*vec4(inpos, 1.0); \n\
} \n\
"};

const char* colorLightSFrag = {
"#version 330 \n\
in vec3 vertexpos; \n\
in vec4 color; \n\
in vec3 normal; \n\
out vec4 outcolor; \n\
void main() { \n\
    float gamma = 2.2; \n\
    vec3 lpos = vec3(0.0, 10.0, 0.0); \n\
    vec3 lcolor = vec3(1.0, 1.0, 1.0); \n\
    vec3 kd = color.xyz; \n\
    vec3 n = normalize(normal); \n\
    vec3 s = normalize(lpos - vertexpos); \n\
    vec3 outcolor3 = lcolor*(kd*max(dot(s, n), 0.0)); \n\
    outcolor = vec4(pow(outcolor3, vec3(1.0/gamma)), 1.0); \n\
}\n\
"};

Material* ColorMaterialDir(){
	Material *m = malloc(sizeof(Material));
    memset(m, 0, sizeof(Material));
    
    m->diffsource = VEC;

	m->shininess = 8.0;
	m->ks[0] = 0.0; m->ks[1] = 0.0; m->ks[2] = 0.0; m->ks[3] = 1.0;
	m->ka[0] = 0.0; m->ka[1] = 0.0; m->ka[2] = 0.0; m->ka[3] = 1.0;
	m->kd[0] = 0.2; m->kd[1] = 0.7; m->kd[2] = 0.2; m->kd[3] = 1.0;
	m->ke[0] = 0.0; m->ke[1] = 0.0; m->ke[2] = 0.0; m->ke[3] = 1.0;

	m->shdr = InitializeShader( NULL, colorDirVertex, colorDirFrag);

	return m;
}

Material* ColorMaterialDir2(){
	Material *m = malloc(sizeof(Material));
    memset(m, 0, sizeof(Material));

    m->diffsource = VEC;

	m->shininess = 8.0;
	m->ks[0] = 0.0; m->ks[1] = 0.0; m->ks[2] = 0.0; m->ks[3] = 1.0;
	m->ka[0] = 0.0; m->ka[1] = 0.0; m->ka[2] = 0.0; m->ka[3] = 1.0;
	m->kd[0] = 0.0; m->kd[1] = 0.0; m->kd[2] = 0.8; m->kd[3] = 1.0;
	m->ke[0] = 0.0; m->ke[1] = 0.0; m->ke[2] = 0.0; m->ke[3] = 1.0;
    
    char *vertshader = ReadTextFile("data/shaders/skyfromatmosphere.vert");
    char *fragshader = ReadTextFile("data/shaders/skyfromatmosphere.frag");
    m->shdr = InitializeShader( NULL, vertshader, fragshader); 
	//m->shdr = initializeShader(colorDirVertex, colorDirFrag);

	return m;
}

Material* ColorMaterialPhong(vec3 ka, vec3 kd, vec3 ks, float shininess) {
    Material *m = malloc(sizeof(Material));
    memset(m, 0, sizeof(Material));

    m->diffsource = VEC;
	m->shininess = shininess;
	memcpy(m->ka, ka, sizeof(vec3));
	memcpy(m->kd, kd, sizeof(vec3));
	memcpy(m->ks, ks, sizeof(vec3));
//	memcpy(m->ke, ke, sizeof(vec3));
    
    char *vertshader = ReadTextFile("data/shaders/phong.vert");
    char *fragshader = ReadTextFile("data/shaders/phong.frag");
    m->shdr = InitializeShader( NULL, vertshader, fragshader); 
	return m;
}

Material* ColorMaterialShadowPhong(vec3 ka, vec3 kd, vec3 ks, float shininess) {
    Material *m = malloc(sizeof(Material));
    memset(m, 0, sizeof(Material));

    m->diffsource = VEC;
	m->shininess = shininess;
	memcpy(m->ka, ka, sizeof(vec3));
	memcpy(m->kd, kd, sizeof(vec3));
	memcpy(m->ks, ks, sizeof(vec3));
//	memcpy(m->ke, ke, sizeof(vec3));
    
    char *vertshader = ReadTextFile("data/shaders/shadowsecondpass.vert");
    char *fragshader = ReadTextFile("data/shaders/shadowsecondpass.frag");
    m->shdr = InitializeShader( NULL,vertshader, fragshader); 
	return m;
}


Material* ColorMaterialNoLight() {
    Material *m = malloc(sizeof(Material));
    memset(m, 0, sizeof(Material));
	m->shdr = InitializeShader( NULL, colorNoLightVertex, colorNoLightFrag);

    return m;
}

Material* ColorMaterialLightSimple() {
    Material *m = malloc(sizeof(Material));
    memset(m, 0, sizeof(Material));
	m->shdr = InitializeShader( NULL, colorLightSVertex, colorLightSFrag);

    return m;
}

//
Material* TexturedMaterialDir(Texture *t) {
    Material *m = malloc(sizeof(Material));

    m->diffsource = TEXTURE;
    m->diffmap = t;
    m->transfermap = NULL;

    m->shininess = 0.0;
    m->ks[0] = 0.0; m->ks[1] = 0.0; m->ks[2] = 0.0; m->ks[3] = 1.0;
	m->ka[0] = 0.0; m->ka[1] = 0.0; m->ka[2] = 0.0; m->ka[3] = 1.0;
	m->kd[0] = 0.0; m->kd[1] = 0.0; m->kd[2] = 0.0; m->kd[3] = 1.0;
	m->ke[0] = 0.0; m->ke[1] = 0.0; m->ke[2] = 0.0; m->ke[3] = 1.0;

    char *vertshader = ReadTextFile("data/shaders/vertshader.vert");
    char *fragshader = ReadTextFile("data/shaders/fragshader.frag");
    m->shdr = InitializeShader( NULL, vertshader, fragshader); 

    return m;
}

Material *VolumeMaterial(Texture *t) {
    Material *m = malloc(sizeof(Material));

    m->diffsource = TEXTURE;
    m->diffmap = t;
    m->transfermap = NULL;

    m->shininess = 0.0;
    m->ks[0] = 0.0; m->ks[1] = 0.0; m->ks[2] = 0.0; m->ks[3] = 1.0;
	m->ka[0] = 0.0; m->ka[1] = 0.0; m->ka[2] = 0.0; m->ka[3] = 1.0;
	m->kd[0] = 0.0; m->kd[1] = 0.0; m->kd[2] = 0.0; m->kd[3] = 1.0;
	m->ke[0] = 0.0; m->ke[1] = 0.0; m->ke[2] = 0.0; m->ke[3] = 1.0;
    
    char *vertshader = ReadTextFile("data/shaders/volume.vert");
    char *fragshader = ReadTextFile("data/shaders/volume.frag");
    m->shdr = InitializeShader( NULL, vertshader, fragshader); 

    return m;
}


Material* VolumeMaterialTransfer(Texture* volData, Texture* transferData) {
    Material *m = malloc(sizeof(Material));

    m->diffsource = TEXTURE;
    m->diffmap = volData;

    m->transfermap = transferData;

    m->shininess = 0.0;
    m->ks[0] = 0.0; m->ks[1] = 0.0; m->ks[2] = 0.0; m->ks[3] = 1.0;
	m->ka[0] = 0.0; m->ka[1] = 0.0; m->ka[2] = 0.0; m->ka[3] = 1.0;
	m->kd[0] = 0.0; m->kd[1] = 0.0; m->kd[2] = 0.0; m->kd[3] = 1.0;
	m->ke[0] = 0.0; m->ke[1] = 0.0; m->ke[2] = 0.0; m->ke[3] = 1.0;
    
    char *vertshader = ReadTextFile("data/shaders/volume.vert");
    char *fragshader = ReadTextFile("data/shaders/volumeTransfer.frag");
    m->shdr = InitializeShader( NULL, vertshader, fragshader); 

    return m;
}

void BindMaterial(Material* m, Light* l ){
	if (!m || !l)
		return;

    vec3 kd = {m->kd[0], m->kd[1], m->kd[2]};
    vec3 ka = {m->ka[0], m->ka[1], m->ka[2]};
    vec3 ks = {m->ks[0], m->ks[1], m->ks[2]};
	SetShaderConstant3f(m->shdr, "kd", kd );
	SetShaderConstant3f(m->shdr, "ka", ka );
	SetShaderConstant3f(m->shdr, "ks", ks );
	SetShaderConstant1f(m->shdr, "shininess", m->shininess);

	SetShaderConstant3f(m->shdr, "LightPosition", l->pos);
    vec3 lcolor = {l->color[0], l->color[1], l->color[2]};
	SetShaderConstant3f(m->shdr, "LightColor", lcolor);

	float ambient[4];
	ambient[0] = 0.1;
	ambient[1] = 0.1;
	ambient[2] = 0.1;
	ambient[3] = 0.1;
	SetShaderConstant4f(m->shdr, "globalAmbient", ambient);

    //if(m->diffsource == TEXTURE) {
    //    bindSamplerState(m->diffmap->state, 0);
    //    bindTexture(m->diffmap, 0);
    //}

    for(unsigned int i = 0; i < m->texCount; i++) {
        //printf("binding texture: %d/%d\n", i, m->texCount);
        BindSamplerState(m->textures[i]->state, i);
        BindTexture(m->textures[i], i);
    }

    //if(m->transfermap) {
    //    bindSamplerState(m->transfermap->state, 1);
    //    bindTexture(m->transfermap, 1);
    //}
}

Material *MaterialInit() {
    Material *m = malloc(sizeof(Material));
    m->texCount = 0;

    return m;
}

//FIXME: retornar erro
void MaterialAddTex(Material *m, Texture *t) {
    if(m->texCount < 8) {
        m->textures[m->texCount] = t;
        m->texCount++;
    }
}

void MaterialSetShader(Material *m, Shader *s) {
    m->shdr = s; 
}

