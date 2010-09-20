#ifndef _PARTICLES_H_
#define _PARTICLES_H_

typedef struct _particles{

	int sizex, sizey;
	unsigned int velFbo[2];
	unsigned int posFbo[2];

} particles;

void initializeParticles(int x, int y, particles* p);

#endif
