#include <stdio.h>
#include <stdlib.h> //malloc
#include "glapp.h"
#include "math/vec3.h"
#include "renderer/renderer.h"
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "renderer/glime.h"
#include "renderer/camera.h"
#include "renderer/gui.h"
#include "util/colladaloader.h"
#include "util/textfile.h"

Scene* cena;
Shader *shdr;
renderer *mainrenderer;
Camera c;
BoundingBox bbox;

Mesh* m;

batch* b;

Texture *t;
Texture* t2;
Texture *t3;
Texture* t4;
Texture* t5;

void initializeGame(){
    initCamera(&c, TRACKBALL);
    
    bbox.pmin[0] = -1.0;
    bbox.pmin[1] = -1.0;
    bbox.pmin[2] = -1.0;
    bbox.pmax[0] = 1.0;
    bbox.pmax[1] = 1.0;
    bbox.pmax[2] = -1.0;
    camerafit(&c, bbox, 45.0, 800/600, 0.1, 1000.0);

    initializeGUI(800, 600);

	t = initialize2DTexture("data/textures/dialog1.png");
	t->state = initializeSamplerState(CLAMP, LINEAR, LINEAR, 0);
	t2 = initialize2DTexture("data/textures/mago1.tga");
	t3 = initialize2DTexture("data/textures/hunchback.png");
	t4 = initialize2DTexture("data/textures/experimenter.png");
	t5 = initialize2DTexture("data/textures/tooltip_bg.png");
	t5->state = t3->state = t4->state = t2->state = t->state;
}

int Update(event* e, float* dt){
    cameraHandleEvent(&c, e);
    setupViewMatrix(&c);
    if (e->keys[KEY_ESC])
	screenshot("menuloko.tga");
    return 1;
}

float offsetx = 0;
float offsety = 0;
float offx = 0;
float offy = 0;

int Render(event* e, float* dt){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec3 bboxcenter;
    bboxcenter[0] = 0.0;
    bboxcenter[1] = 0.0;
    bboxcenter[2] = -1.0;

    //translada para o centro
    fptranslatef(c.modelview, -bboxcenter[0], -bboxcenter[1], -bboxcenter[2]);
    setView(c.modelview);
    setProjection(c.projection); //TODO isso so precisaria ser calculado/setado 1x

	beginGUI(e);
		rect r;
		r.x = 50;
		r.y = 50;
//		doButton(1, &r, L"Button");
		setFontSize(14);
		beginMenu(3, 200, 100, 540*0.6, 680*0.6,&offsetx, &offsety, "nada", t );
			doLabel( 298, 483, "Character Profile");
			doLabel(345, 430, "Abnerius Bacon");
			doLabel(345, 412, "67 years");
			doLabel(345, 392, "Catholic"); 
			doImage(4, 270, 380, 64, 64, t2);
			doLabel(270, 362, "Traits");
			doImage(5, 270, 330, 24, 24, t3);
			doImage(6, 300, 330, 24, 24, t4);
			if (doTooltip(100, 270, 330, 24, 24)){
				beginMenu(7, e->x+2, e->y+2, 120, 60, &offx, &offy, "", NULL );
					doLabel(e->x+2+10, e->y+35, "Hunchback");
					doLabel(e->x+2+10, e->y+18, "-2 Presence");
				endMenu(7, e->x+2, e->y+2, 100, 50, &offx, &offy);
			}
			if (doTooltip(101, 300, 330, 24, 24)){
				beginMenu(7, e->x+2, e->y+2, 240, 60, &offx, &offy, "", NULL );
					doLabel(e->x+2+10, e->y+35, "Alchemyst");
					doLabel(e->x+2+10, e->y+18, "Increased Potions Strength");
				endMenu(7, e->x+2, e->y+2, 100, 50, &offx, &offy);
			}
		endMenu(3, 200, 100, 100, 100, &offsetx, &offsety);

		doLabel(545, 400, "Know no Fear");
		setFontSize(15);
		doLabel(545, 380, "Know no Fear");
		setFontSize(16);
		doLabel(545, 360, "Know no Fear");
		setFontSize(17);
		doLabel(545, 340, "Know no Fear");
		setFontSize(18);
		doLabel(545, 320, "Know no Fear");
		setFontSize(19);
		doLabel(545, 300, "Know no Fear");
		setFontSize(20);
		doLabel(545, 280, "Know no Fear");
		setFontSize(21);
		doLabel(545, 260, "Know no Fear");
		setFontSize(22);
		doLabel(545, 240, "Know no Fear");
		setFontSize(22);
		doLabel(545, 220, "Know no Fear");
		setFontSize(23);
		doLabel(545, 200, "Know no Fear");




	endGUI();

    glFlush();

    return 1;
}


int main(){
	setVideoMode(800, 600, 0);
	warpmouse( 0);
	setWindowTitle("Font Test");
	mainrenderer  = initializeRenderer(800, 600, 0.1, 10000.0, 45.0);
	initializeGame();
	MainLoop( );

	closeVideo();
	return 0;
}



