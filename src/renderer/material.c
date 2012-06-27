#include "material.h"
#include "renderer.h"
#include <stdlib.h>

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

Material* colorMaterialDir(){

	Material *m = malloc(sizeof(Material));

	m->diffmap = NULL;

	m->shininess = 8.0;
	m->ks[0] = 0.0; m->ks[1] = 0.0; m->ks[2] = 0.0; m->ks[3] = 1.0;
	m->ka[0] = 0.0; m->ka[1] = 0.0; m->ka[2] = 0.0; m->ka[3] = 1.0;
	m->kd[0] = 0.2; m->kd[1] = 0.7; m->kd[2] = 0.2; m->kd[3] = 1.0;
	m->ke[0] = 0.0; m->ke[1] = 0.0; m->ke[2] = 0.0; m->ke[3] = 1.0;

	m->shdr = initializeShader(colorDirVertex, colorDirFrag);

	return m;
}

Material* colorMaterialDir2(){
	Material *m = malloc(sizeof(Material));

	m->diffmap = NULL;
    m->diffsource = VEC;

	m->shininess = 8.0;
	m->ks[0] = 0.0; m->ks[1] = 0.0; m->ks[2] = 0.0; m->ks[3] = 1.0;
	m->ka[0] = 0.0; m->ka[1] = 0.0; m->ka[2] = 0.0; m->ka[3] = 1.0;
	m->kd[0] = 0.0; m->kd[1] = 0.0; m->kd[2] = 0.8; m->kd[3] = 1.0;
	m->ke[0] = 0.0; m->ke[1] = 0.0; m->ke[2] = 0.0; m->ke[3] = 1.0;
    
    char *vertshader = readTextFile("data/shaders/skyfromatmosphere.vert");
    char *fragshader = readTextFile("data/shaders/skyfromatmosphere.frag");
    m->shdr = initializeShader(vertshader, fragshader); 
	//m->shdr = initializeShader(colorDirVertex, colorDirFrag);

	return m;
}

//
Material *volumeMaterial(Texture *t) {
    Material *m = malloc(sizeof(Material));

    m->diffsource = TEXTURE;
    m->diffmap = t;

    m->shininess = 0.0;
    m->ks[0] = 0.0; m->ks[1] = 0.0; m->ks[2] = 0.0; m->ks[3] = 1.0;
	m->ka[0] = 0.0; m->ka[1] = 0.0; m->ka[2] = 0.0; m->ka[3] = 1.0;
	m->kd[0] = 0.0; m->kd[1] = 0.0; m->kd[2] = 0.0; m->kd[3] = 1.0;
	m->ke[0] = 0.0; m->ke[1] = 0.0; m->ke[2] = 0.0; m->ke[3] = 1.0;
    
    char *vertshader = readTextFile("data/shaders/phong.vert");
    char *fragshader = readTextFile("data/shaders/phong.frag");
    m->shdr = initializeShader(vertshader, fragshader); 

    return m;
}

void bindMaterial(Material* m, Light* l ){
	if (!m || !l)
		return;

	setShaderConstant3f(m->shdr, "kd", m->kd );
	setShaderConstant4f(m->shdr, "ka", m->ka );
	setShaderConstant4f(m->shdr, "ks", m->ks );
	setShaderConstant1f(m->shdr, "shininess", m->shininess);

	setShaderConstant3f(m->shdr, "LightPosition", l->pos);
	setShaderConstant4f(m->shdr, "LightColor", l->color);

	float ambient[4];
	ambient[0] = 0.1;
	ambient[1] = 0.1;
	ambient[2] = 0.1;
	ambient[3] = 0.1;
	setShaderConstant4f(m->shdr, "globalAmbient", ambient);

    if(m->diffsource == TEXTURE) {
        bindSamplerState(m->diffmap->state, 0);
        bindTexture(m->diffmap, 0);
    }
}
