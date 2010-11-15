#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include "math/vec3.h"

typedef struct _particles{
    /* posicao da particula */
    vec3 pos;
    /* velocidade da partícula */
    vec3 v;
    /* tempo de vida de uma partícula */
    float lifetime;
    float size;
} particles;

typedef struct _particlesystem {
    /* numero de particulas */
    unsigned int n;
    /* vetor de partículas */
    particles* p;
    /* tempo do sistema de partículas */
    float t;
}particlesystem;

particlesystem* initparticles(unsigned int numparticles);
void updateparticles(particlesystem* ps, float elapsedtime);
void renderparticles(particlesystem* ps);

#endif
