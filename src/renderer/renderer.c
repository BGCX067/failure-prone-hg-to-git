
#include "renderer.h"
#include "camera.h"
#include "math/matrix.h"
#include "../glapp.h"
#include <GL/gl.h>
#include <GL/glu.h>

renderer* r;
camera c;

void beginRender(keyboard *k) { 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    mat4 m;
    cameraHandleEvent(&c, *k);
    setupViewMatrix(&c, m);
    //glLoadMatrixf(m);
    gluLookAt(c.pos[0], c.pos[1], c.pos[2], c.viewDir[0] + c.pos[0],
              c.viewDir[1] + c.pos[1], c.viewDir[2] + c.pos[2],
              c.up[0], c.up[1], c.up[2]);
}

int render(void* data){
<<<<<<< local

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

=======
    beginRender((keyboard*)data);
>>>>>>> other
	glTranslatef(0.0, 0.0, -5.0f);
	GLUquadric* quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_LINE);
	gluSphere(quadric,  0.5, 20, 20);
<<<<<<< local

	glFinish();
=======
    glFinish();
    glFlush();
>>>>>>> other
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

    initCamera(&c);
	return r;

}

