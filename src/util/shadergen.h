#ifndef SHADERGEN_H
#define SHADERGEN_H

enum {
    PHONG = 1 << 0,
    TEX = 1 << 1,
    NORMAL_MAP = 1 << 2,
    ATTENUATION = 1 << 3,
    ENV_MAP = 1 << 4,
    REFRACT = 1 << 5,
    SPOTLIGHT = 1 << 6
};

typedef struct _material {
    int flags; 
}material;

void shadergen(material m, char** vertShader, char** fragShader);


#endif
