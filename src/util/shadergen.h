#ifndef SHADERGEN_H
#define SHADERGEN_H

enum {
    PHONG = 1 << 0, /* OK */
    TEX = 1 << 1, /* OK */
    NORMAL_MAP = 1 << 2, /* FAIL */
    ATTENUATION = 1 << 3, /* OK */
    ENV_MAP = 1 << 4, /* FAIL */
    REFRACT = 1 << 5, /* FAIL */
    SPOTLIGHT = 1 << 6 /* FAIL */
};

typedef struct _shaderflags {
    int flags; 
}shaderflags;

void shadergen(shaderflags m, char** vertShader, char** fragShader);


#endif
