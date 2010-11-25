#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include "math/vec3.h"

typedef struct _particles{
    vec3 pos;
    vec3 v;
    float lifetime;
    float size;
} particles;

typedef struct _particlesystem {
    
    unsigned int n;
    particles* p;
    float time;

    vec3 pos;
    float maxlifetime;
    float initialSpeed;

    vec3 force;
}particlesystem;

particlesystem* initparticles(unsigned int numparticles, vec3 pos, float maxLife, float speed, vec3 force);
void updateparticles(particlesystem* ps, float elapsedtime);
void renderparticles(particlesystem* ps);

#endif
