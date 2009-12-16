
#include "renderer.h"
#include <GL/gl.h>
#include <GL/glu.h>

renderer* r;

int render(void* data){

	glTranslatef(0.0, 0.0, -5.0f);
	GLUquadric* quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_LINE);
	gluSphere(quadric,  0.5, 20, 20);

}

renderer* initializeRenderer(int w, int h, float znear, float zfar, float fovy){

	r = (renderer*) malloc(sizeof(renderer));
	r->fovy = fovy;
	r->zfar = zfar;
	r->znear = znear;

	float ratio = (float) w / (float) h;
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(fovy, ratio, znear, zfar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor( 0.7, 0.7, 0.7, 1.0 );
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableClientState(GL_VERTEX_ARRAY);

	return r;

}

