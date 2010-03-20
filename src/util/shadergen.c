#include "shadergen.h"
#include "malloc.h"
#include <string.h>

material* initMaterial(int flags) {
    material *m = (material*) dlmalloc(sizeof(material));
    m->flags = flags;
    return m;
}

char* createVSGlobals(material m) {
    char* ret = NULL;
    switch(m.flags) {
        case PHONG: {
            //position e normal
            char* phong =   "varying vec3 normal;\n"
                            "varying vec3 position;\n\0";
            ret = phong;
            break;
        }
        case NORMAL_MAP: {
            char *normalmap = "attribute vec3 Tangent;\n"
                              "attribute vec3 Binormal;\n"
                              "uniform vec3 LightPosition;\n"
                              "varying vec3 lightDir;\n"
                              "varying vec3 position;\n\0";
            ret = normalmap;
            break;
        }
    }
    return ret;
}

char* createVSFuncs(material m) {
    char *ret = NULL;
    switch(m.flags) {
        case PHONG: {
            char *phong = "\n\0";
            ret = phong;
            break;
        }
        case NORMAL_MAP: {
            char *normalmap = "\n\0";
            ret = normalmap;
            break;
        }
    }
    return ret;
}

char* createVSMain(material m) {
    char *ret = NULL;
    switch(m.flags) {
        case PHONG: {
            char *phong = "void main() {\n"
                          "\tgl_Position = ftransform();\n"
                          "\tposition = gl_Vertex.xyz;\n"
                          "\tnormal = gl_NormalMatrix*gl_Normal;\n"
                          "\tgl_TexCoord[0] = gl_MultiTexCoord0;\n"
                          "}\n\0";
            ret = phong;
            break;
        }
        case NORMAL_MAP: {
            char *normalmap = "void main() {\n"
                              "\tgl_Position = ftransform();\n"
                              "\tgl_TexCoord[0] = gl_MultiTexCoord0;\n"
                              "\tposition = gl_Vertex.xyz;\n"
                              "\tvec3 lightDirection = LightPosition - position.xyz;\n"
                              "\tvec3 normal = gl_NormalMatrix*gl_Normal;\n"
                              "\tvec3 tangent = gl_NormalMatrix*Tangent;\n"
                              "\tvec3 binormal = cross(normal, tangent);\n"
                              "\tlightDir.x = dot(lightDirection, tangent);\n"
                              "\tlightDir.y = dot(lightDirection, binormal);\n"
                              "\tlightDir.z = dot(lightDirection, normal);\n"
                              "}\n\0";
            ret = normalmap;
            break;
        }
    }
    return ret;
}

char* createFSGlobal(material m) {
    char *ret;
    switch(m.flags) {
        case PHONG: {
            char* phong = "varying vec3 normal;\n"
                          "varying vec3 position;\n"
                          "uniform vec3 LightPosition;\n"
                          "uniform vec3 EyePosition;\n"
                          "uniform vec4 Ka;\n"
                          "uniform vec4 Kd;\n"
                          "uniform vec4 Ks;\n"
                          "uniform float Kc;\n"
                          "uniform float Kl;\n"
                          "uniform float Kq;\n"
                          "uniform float shininess;\n"
                          "uniform vec4 globalAmbient;\n"
                          "uniform vec4 LightColor;\n"
                          "uniform sampler2D texture;\n\0";
            ret = phong;
            break;
        }
        case NORMAL_MAP: {
            char* normalmap = "varying vec3 lightDir;\n"
                "varying vec3 position;\n"
                "uniform vec3 EyePosition;\n"
                "uniform vec4 Ka;\n"
                "uniform vec4 Kd;\n"
                "uniform vec4 Ks;\n"
                "uniform float shininess;\n"
                "uniform vec4 globalAmbient;\n"
                "uniform vec4 LightColor;\n"
                "uniform sampler2D normalMap;\n"
                "uniform sampler2D baseTexture;\n\0";
            ret = normalmap;
            break;
        }
    }
    return ret;
}

char* createFSFuncs(material m) {
    char *ret;
    switch(m.flags) {
        case PHONG: {
            char* phong = "\n\0";
            ret = phong;
            break;
        }
        case NORMAL_MAP: {
            char* normalmap = "\n\0";
            ret = normalmap;
            break;
        }
    }
    return ret;
}

char* createFSMain(material m) {
    char* ret;
    switch(m.flags) {
        case PHONG: {
            char* phong = "void main() {\n"
                          "\tvec3 N = normalize(normal);\n"
                          "\tvec3 lightVec = normalize(LightPosition - position);\n"
                          "\tvec3 viewVec = normalize(EyePosition - position);\n"
                          "\tvec3 halfVec = normalize(lightVec + viewVec);\n"
                          "\tfloat diffCoef = max(dot(N, lightVec), 0.0);\n"
                          "\tfloat specCoef = pow(max(dot(N, halfVec), 0.0), shininess);\n"
                          "\tif (diffCoef <= 0.0)\n"
                          "\t\tspecCoef = 0.0;\n"
                          "\tvec4 ambient = globalAmbient*Ka;\n"
                          "\tvec4 diffuse = Kd*LightColor*diffCoef;\n"
                          "\tvec4 specular = Ks*LightColor*specCoef;\n"
                          "\tfloat d = distance(position, LightPosition); \n"
                          "\tfloat attenuation = 1.0/(Kc + Kl*d + Kq*d*d);\n"
                          "\tgl_FragColor = (ambient + attenuation*(diffuse + specular))*texture2D(texture, gl_TexCoord[0].st);\n"
                          "\tgl_FragColor.w = 1.0;\n"
                          "}\n\0";
            ret = phong;
            break;
        }
        case NORMAL_MAP: {
            char* normalmap = "void main() {\n"
                              "\tvec3 lightVec = normalize(lightDir);\n"
                              "\tvec3 N = normalize((texture2D(normalMap, gl_TexCoord[0].xy).xyz*2.0) - 1.0);\n"
                              "\tvec3 viewVec = normalize(EyePosition - position);\n"
                              "\tvec3 halfVec = normalize(lightDir + viewVec);\n"
                              "\tfloat diffCoef = max(dot(N, lightVec), 0.0);\n"
                              "\tfloat specCoef = pow(max(dot(N, halfVec), 0.0), shininess);\n"
                              "\tif (diffCoef <= 0.0)\n"
                              "\t\tspecCoef = 0.0;\n"
                              "\tvec4 ambient = globalAmbient*Ka;\n"
                              "\tvec4 diffuse = Kd*LightColor*diffCoef;\n"
                              "\tvec4 specular = Ks*LightColor*specCoef;\n"
                              "\tvec4 baseColor = texture2D(baseTexture, gl_TexCoord[0].xy);\n"
                              "\tgl_FragColor = (ambient + diffuse + specular)*baseColor;\n"
                              "}\n\0";

            ret = normalmap;
            break;
        }
    }
    return ret;
}

//TODO passar ponteiro pra ponteiro é uma solução ruim
//FIXME: considerar caso em que as strings devam ser concatenadas
//FIXME: dividir os shaders em funções pra facilitar a geração de shaders
void shadergen(material m, char** vertShader, char** fragShader) {
    //create vert shader
    char* vsglobal = createVSGlobals(m);
    char* vsfunc = createVSFuncs(m);
    char* vsmain = createVSMain(m);
    int vslength = strlen(vsglobal) + strlen(vsfunc) + strlen(vsmain);
    *vertShader = (char*) dlmalloc(sizeof(char)*vslength);
    memset(*vertShader, '\0', sizeof(char)*vslength);
    strcat(*vertShader, vsglobal);
    strcat(*vertShader, vsfunc);
    strcat(*vertShader, vsmain);
    
    //create frag shader
    char* fsglobal = createFSGlobal(m);
    char* fsfunc = createFSFuncs(m);
    char* fsmain = createFSMain(m);
    int fslength = strlen(fsglobal) + strlen(fsfunc) + strlen(fsmain);
    *fragShader = (char*) dlmalloc(sizeof(char)*fslength);
    memset(*fragShader, '\0', sizeof(char)*fslength);
    strcat(*fragShader, fsglobal);
    strcat(*fragShader, fsfunc);
    strcat(*fragShader, fsmain);
}


