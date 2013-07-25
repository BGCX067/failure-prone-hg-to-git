#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include "math/vec3.h"

typedef struct _particles{
    vec3 pos;
    vec3 v;
    float lifetime;
    float size;
} Particles;

typedef struct _particlesystem {
    
    unsigned int n;
    Particles* p;
    float time;

    vec3 pos;
    float maxlifetime;
    float initialSpeed;

    vec3 force;
}ParticleSystem;

ParticleSystem* InitParticles(unsigned int numparticles, vec3 pos, float maxLife, float speed, vec3 force);
void UpdateParticles(ParticleSystem* ps, float elapsedtime);
void RenderParticles(ParticleSystem* ps);

#endif
