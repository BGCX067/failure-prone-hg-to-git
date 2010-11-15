#include "particles.h"
#include "renderer.h"
#include <stdlib.h>
#include "glime.h"

particlesystem* initparticles(unsigned int numparticles) {
    particlesystem* ps = malloc(sizeof(particlesystem));

    ps->n = numparticles;
    ps->p = malloc(sizeof(particles)*ps->n);
    ps->t = 0.0;
    for(int i = 0; i < ps->n; i++) {
        ps->p[i].pos[0] = 0.0;/*20.0*((float)rand()/RAND_MAX) - 10.0;*/
        ps->p[i].pos[1] = 0.0;/*20.0*((float)rand()/RAND_MAX) - 10.0;*/
        ps->p[i].pos[2] = 0.0;

        ps->p[i].v[0] = 2.0*((float)rand()/RAND_MAX) - 1.0;
        ps->p[i].v[1] = 2.0*((float)rand()/RAND_MAX) - 1.0;
        ps->p[i].v[2] = 0.0;

        ps->p[i].lifetime = ((float)rand()/RAND_MAX)*10.0;
    }

    return ps;
}

void updateparticles(particlesystem *ps, float elapsedtime) {
    ps->t += elapsedtime;
    for(int i = 0; i < ps->n; i++) {
        ps->p[i].pos[0] = ps->p[i].pos[0] + ps->p[i].v[0]*elapsedtime;
        ps->p[i].pos[1] = ps->p[i].pos[1] + ps->p[i].v[1]*elapsedtime;
        ps->p[i].pos[2] = ps->p[i].pos[2] + ps->p[i].v[2]*elapsedtime;

        ps->p[i].lifetime -= elapsedtime;
        //if(ps->p[i].lifetime < 0.0)
        //    ps->p[i].lifetime = 0.0;

//        if(ps->p[i].pos[1] < -30.0)
//            ps->p[i].pos[1] = 0.0;
        /*if(i%3 == 0)
            ps->p[i].pos[0] += 0.00003;
        else if(i%3 == 1)
            ps->p[i].pos[1] += 0.00003;
        else
            ps->p[i].pos[2] += 0.00003;*/
        //ps->p[i].pos[0] -= 2*rand()/RAND_MAX - 1.0;
//        ps->p[i].pos[1] -= (((float)rand())/RAND_MAX)*0.01;
        
            
            
        /*ps->p[i].pos[0] = _noise(ps->p[i].pos[0]);
        ps->p[i].pos[1] = _noise(ps->p[i].pos[1]);
        ps->p[i].pos[2] = _noise(ps->p[i].pos[2]);*/
//        printf("%f, %f, %f\n", ps->p[i].pos[0], ps->p[i].pos[1], ps->p[i].pos[2]);
    }
}


void renderparticles(particlesystem* ps) {
    batch* particles;
    particles = malloc(sizeof(batch));

    initializeBatch(particles);

    /* conta o numero de particulas ativas */
    int numactive = 0;
    for(int i = 0; i < ps->n; i++)
        if(ps->p[i].lifetime > 0)
            numactive++;

    begin(particles, GL_POINTS, numactive, 0);
        for(int i = 0; i < ps->n; i++)
            /* considera apenas as particulas ativas */
            if(ps->p[i].lifetime > 0)
                vertex3f(particles, ps->p[i].pos[0], ps->p[i].pos[1], ps->p[i].pos[2]);

    end(particles);

    draw(particles);

    free(particles);
}
