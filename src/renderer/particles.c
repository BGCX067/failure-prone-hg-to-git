#include "particles.h"
#include "renderer.h"

void initializeParticles(int x, int y, particles* p){

	p->posFbo[0]  = initializeFramebuffer(NULL, x, y, RGB, RGB8, FLOAT, (LINEAR));
	p->posFbo[1] =  initializeFramebuffer(NULL, x, y, RGB, RGB8, FLOAT, (LINEAR));

	


}
