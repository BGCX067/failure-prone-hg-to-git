#include "particles.h"
#include "renderer.h"
#include <stdlib.h>
#include <math.h>
#include "glime.h"

static float testNoise(float a){
	return sqrt(-2 * log((float) rand()/ RAND_MAX)) * sin(2 * 3.1415 * (float)rand() / RAND_MAX)*a;
}

ParticleSystem* InitParticles(unsigned int numparticles, vec3 pos, float maxLife, float speed, vec3 force) {
    ParticleSystem* ps = malloc(sizeof(ParticleSystem));

    ps->pos[0] = pos[0]; ps->pos[1] = pos[1]; ps->pos[2] = pos[2];
    ps->maxlifetime =  maxLife;
    ps->n = numparticles;
    ps->p = malloc(sizeof(Particles)*ps->n);
    ps->time = 0.0;
    ps->initialSpeed = speed;
    Setv(ps->force, force);
    for(unsigned int i = 0; i < ps->n; i++) {
        ps->p[i].pos[0] = pos[0];/*20.0*((float)rand()/RAND_MAX) - 10.0;*/
        ps->p[i].pos[1] = pos[1];/*20.0*((float)rand()/RAND_MAX) - 10.0;*/
        ps->p[i].pos[2] = pos[2];

        ps->p[i].v[0] = testNoise(speed);//2.0*((float)rand()/RAND_MAX) - 1.0;
        ps->p[i].v[1] = testNoise(speed);//2.0*((float)rand()/RAND_MAX) - 1.0;
        ps->p[i].v[2] = testNoise(speed);//0.0;

        ps->p[i].lifetime = ((float) i / (float) ps->n * maxLife);
    }

    return ps;
}

void UpdateParticles(ParticleSystem *ps, float elapsedtime) {
    ps->time += elapsedtime;
    vec3 f;
    vec3 s;
    f[0] = ps->force[0]*elapsedtime;
    f[1] = ps->force[1]*elapsedtime;
    f[2] = ps->force[2]*elapsedtime;
    for(unsigned int i = 0; i < ps->n; i++) {

	ps->p[i].v[0] += f[0];
	ps->p[i].v[1] += f[1];
	ps->p[i].v[2] += f[2];

	s[0] = ps->p[i].v[0] * elapsedtime;
	s[1] = ps->p[i].v[1] * elapsedtime;
	s[2] = ps->p[i].v[2] * elapsedtime;


	ps->p[i].pos[0] += s[0];
	ps->p[i].pos[1] += s[1];
	ps->p[i].pos[2] += s[2];

    ps->p[i].lifetime += elapsedtime;
	if (ps->p[i].lifetime > ps->maxlifetime){
		ps->p[i].pos[0] = ps->pos[0];
		ps->p[i].pos[1] = ps->pos[1];
		ps->p[i].pos[2] = ps->pos[2];
		ps->p[i].v[0] = testNoise(ps->initialSpeed);//2.0*((float)rand()/RAND_MAX) - 1.0;
        	ps->p[i].v[1] = testNoise(ps->initialSpeed);//2.0*((float)rand()/RAND_MAX) - 1.0;
        	ps->p[i].v[2] = testNoise(ps->initialSpeed);//0.0;

		ps->p[i].lifetime = 0;
	}
    }
}


void RenderParticles(ParticleSystem* ps) {
    Batch* particles;
    //particles = malloc(sizeof(batch));

    particles = InitializeBatch();

    /* conta o numero de particulas ativas */
    int numactive = 0;
    for(unsigned int i = 0; i < ps->n; i++)
        if(ps->p[i].lifetime > 0)
            numactive++;

    Begin(particles, GL_POINTS, numactive, 0);
        for(unsigned int i = 0; i < ps->n; i++)
            /* considera apenas as particulas ativas */
            if(ps->p[i].lifetime > 0){
	    	Normal3f(particles, 1.0, 1.0, ps->p[i].lifetime/ps->maxlifetime );
            Vertex3f(particles, ps->p[i].pos[0], ps->p[i].pos[1], ps->p[i].pos[2]);
	    }

    End(particles);
    Draw(particles);
    free(particles);
}
