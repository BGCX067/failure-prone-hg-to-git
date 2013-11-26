#include "material.h"
#include "renderer.h"
#include <stdlib.h>
#include <string.h>
#include "util/textfile.h"


Shader* PhongMaterial(vec3 ka, vec3 kd, vec3 ks, float shininess, vec3 lightpos, vec3 lightcolor) {
    //TODO tá lendo o shader da pasta local da tool, poder ser melhor armazenar esses shaders
    //pre-definidos na pasta data da engine.
    char* vertsrc = ReadTextFile("data/shaders/phong.vert");
    char* fragsrc = ReadTextFile("data/shaders/phong.frag"); 
    Shader *shdr = InitializeShader(NULL, vertsrc, fragsrc);
    free(vertsrc);
    free(fragsrc);
    SetShaderConstant(shdr, "ka", ka);
    SetShaderConstant(shdr, "kd", kd);
    SetShaderConstant(shdr, "ks", ks);
    SetShaderConstant(shdr, "shininess", &shininess);
    SetShaderConstant(shdr, "LightPosition", lightpos);
    SetShaderConstant(shdr, "LightColor", lightcolor);
    return shdr;
}

Shader* ShadowPhongMaterial(vec3 ka, vec3 kd, vec3 ks, float shininess, vec3 lightpos, vec3 lightcolor) {
    char* vertsrc = ReadTextFile("data/shaders/shadowsecondpass.vert");
    char* fragsrc = ReadTextFile("data/shaders/shadowsecondpass.frag"); 
    Shader *shdr = InitializeShader(NULL, vertsrc, fragsrc);
    free(vertsrc);
    free(fragsrc);
    SetShaderConstant(shdr, "ka", ka);
    SetShaderConstant(shdr, "kd", kd);
    SetShaderConstant(shdr, "ks", ks);
    SetShaderConstant(shdr, "shininess", &shininess);
    SetShaderConstant(shdr, "LightPosition", lightpos);
    SetShaderConstant(shdr, "LightColor", lightcolor);

    return shdr; 
}

Shader* AtmosphereMaterial(){
    char *vertshader = ReadTextFile("data/shaders/skyfromatmosphere.vert");
    char *fragshader = ReadTextFile("data/shaders/skyfromatmosphere.frag");
    Shader *shdr = InitializeShader(NULL, vertshader, fragshader);
    free(vertshader);
    free(fragshader);

    return shdr;
}

//FIXME criar um arquivo para colocar as strings com código dos shaders
/*const char *phongVert {
"#version 330\n\
layout(location = 0) in vec3 inpos;\n\
layout(location = 2) in vec3 innormal;\n\
out vec3 vertexpos;\n\
out vec3 normal;\n\
out vec3 lPos;\n\
uniform mat4 modelview;\n\
uniform mat4 view;\n\
uniform mat3 normalmatrix;\n\
uniform mat4 mvp;\n\
uniform vec3 LightPosition;\n\
void main(void)\n\
{\n\
    normal = normalize(normalmatrix*innormal);\n\
    vertexpos = vec3(modelview*vec4(inpos, 1.0));\n\
    lPos = vec3(view*vec4(LightPosition, 1.0));\n\
	gl_Position = mvp*vec4(inpos, 1.0);\n\
}\n\
"};

const char *phongFrag {
"#version 330\n\
in vec3 vertexpos;\n\
in vec3 normal;\n\
in vec3 lPos;\n\
out vec4 outcolor;\n\
//uniform vec3 LightPosition;\n\
uniform vec3 LightColor;\n\
uniform vec3 ka;\n\
uniform vec3 ks;\n\
uniform vec3 kd;\n\
uniform float shininess;\n\
//Não precisa do eyepos pq já tá transformando tudo pro eyespace, logo eyepos = (0, 0, 0)\n\
//uniform vec3 eyepos;\n\
void main() {\n\
    vec3 n = normalize(normal);\n\
    vec3 l = normalize(lPos - vertexpos);\n\
    vec3 v = normalize(-vertexpos);\n\
    vec3 h = normalize(l + v);\n\
    float diffuseLight = max(dot(n, l), 0.0);\n\
    float specularLight = pow(max(dot(n, h), 0.0), shininess);\n\
    if(diffuseLight == 0.0)\n\
        specularLight = 0.0;\n\
    vec3 globalAmbient = vec3(0.1, 0.1, 0.1);\n\
    vec3 amb = globalAmbient*ka;\n\
    vec3 diff = LightColor*kd*diffuseLight;\n\
    vec3 spec = LightColor*ks*specularLight;\n\
    vec3 outcolor3 = amb + diff + spec;\n\
    float gamma = 2.2;\n\
    outcolor = vec4(pow(outcolor3, vec3(1.0/gamma)), 1.0);\n\
    //outcolor = vec4(outcolor3, 1.0);\n\
}\n\
"};

static void bindPhong(Material *m);

//FIXME materiais do mesmo tipo deveriam usar o mesmo shader, mas uniforms diferentes
Material *MaterialInitType(int type) {
    Material *m = malloc(sizeof(Material));
    m->type = type;
    switch(m->type) {
        case PHONG:
            m->bind = bindPhong;
            m->shdr = InitializeShader(NULL, phongVert, phongFrag); 
            break;
        default:
            m->type = CUSTOM;
            break;
    }
}

void BindMaterial(Material *m) {
    //m->bind(m);
    BindShader(m->shdr);
}

static void bindPhong(Material *m) {
    
}*/




/*const char* colorDirVertex = {
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
	SetShaderConstant(m->shdr, "kd", kd );
	SetShaderConstant(m->shdr, "ka", ka );
	SetShaderConstant(m->shdr, "ks", ks );
	SetShaderConstant(m->shdr, "shininess", &m->shininess);

	SetShaderConstant(m->shdr, "LightPosition", l->pos);
    vec3 lcolor = {l->color[0], l->color[1], l->color[2]};
	SetShaderConstant(m->shdr, "LightColor", lcolor);

	float ambient[4];
	ambient[0] = 0.1;
	ambient[1] = 0.1;
	ambient[2] = 0.1;
	ambient[3] = 0.1;
	SetShaderConstant(m->shdr, "globalAmbient", ambient);

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
}*/

