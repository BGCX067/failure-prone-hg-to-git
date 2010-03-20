#ifndef SHADERGEN_H
#define SHADERGEN_H

enum Shader_Format{
    PHONG,
    NORMAL_MAP
};

typedef struct _material {
    int flags; 
}material;

material* initMaterial(int flags);
void shadergen(material m, char** vertShader, char** fragShader);


#endif
