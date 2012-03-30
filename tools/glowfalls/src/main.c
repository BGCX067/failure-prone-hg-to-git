#include <stdio.h>

#include "glapp.h"
//#include "math/algebra.h"
#include "renderer/renderer.h"
#include "renderer/sprite.h"
#include "renderer/camera.h"
#include "chipmunk/chipmunk.h"

#define GRABABLE_MASK_BIT (1<<31)
#define NOT_GRABABLE_MASK (~GRABABLE_MASK_BIT)

renderer *mainrenderer;
int spriteOrientation = 0;
sprite *s;

static cpSpace *space;

typedef struct PlayerStruct {
	cpFloat u;
	cpShape *shape;
	cpVect groundNormal;
	cpArray *groundShapes;
	int isJumping;
	float fair;
	float vair;
} PlayerStruct;

PlayerStruct playerInstance;
cpVect arrowDirection;
float ElapsedTime = 0;

static cpBool
begin(cpArbiter *arb, cpSpace *space, void *ignore)
{
	CP_ARBITER_GET_SHAPES(arb, a, b);
	PlayerStruct *player = (PlayerStruct *)a->data;
	
	cpVect n = cpvneg(cpArbiterGetNormal(arb, 0));
	if(n.y > 0.0f){
		cpArrayPush(player->groundShapes, b);
	}
	
	return cpTrue;
}

static cpBool
preSolve(cpArbiter *arb, cpSpace *space, void *ignore)
{
	CP_ARBITER_GET_SHAPES(arb, a, b);
	PlayerStruct *player = (PlayerStruct *)a->data;
	
	if(cpArbiterIsFirstContact(arb)){
		a->u = player->u;
		
		// pick the most upright jump normal each frame
		cpVect n = cpvneg(cpArbiterGetNormal(arb, 0));
		if(n.y >= player->groundNormal.y){
			player->groundNormal = n;
		}
	}
	
	return cpTrue;
}

static void
separate(cpArbiter *arb, cpSpace *space, void *ignore)
{
	CP_ARBITER_GET_SHAPES(arb, a, b);
	PlayerStruct *player = (PlayerStruct *)a->data;
	
	cpArrayDeleteObj(player->groundShapes, b);
	
	if(player->groundShapes->num == 0){
		a->u = 0.0f;
		player->groundNormal = cpvzero;
	}
}


static void
playerUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt)
{
	cpBodyUpdateVelocity(body, gravity, damping, dt);
	body->v.y = cpfmax(body->v.y, -700);
	body->v.x = cpfclamp(body->v.x, -400, 400);
}

static cpSpace* initSpace(void)
{
	cpResetShapeIdCounter();
	
	space = cpSpaceNew();
	space->iterations = 10;
	space->gravity = cpv(0, -1500);

	cpBody *body, *staticBody = &space->staticBody;
	cpShape *shape;
	
	// Create segments around the edge of the screen.
	shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(0,0), cpv(0,800), 0.0f));
	shape->e = 1.0f; shape->u = 1.0f;
	shape->layers = NOT_GRABABLE_MASK;
	shape->collision_type = 2;

	
	// add some other segments to play with
	shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(0,100), cpv(800,100), 0.0f));
	shape->e = 1.0f; shape->u = 1.0f;
	shape->layers = NOT_GRABABLE_MASK;
	shape->collision_type = 2;

	shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(0,220), cpv(500,220), 0.0f));
	shape->e = 1.0f; shape->u = 1.0f;
	shape->layers = NOT_GRABABLE_MASK;
	shape->collision_type = 2;

	shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(500,100), cpv(800,250), 0.0f));
	shape->e = 1.0f; shape->u = 1.0f;
	shape->layers = NOT_GRABABLE_MASK;
	shape->collision_type = 2;

	
	// Set up the player
	cpFloat radius = 45.0f;
	body = cpSpaceAddBody(space, cpBodyNew(10.0f, INFINITY));
	body->p = cpv(50, 160);
	body->velocity_func = playerUpdateVelocity;

	shape = cpSpaceAddShape(space, cpCircleShapeNew(body, radius, cpvzero));
	shape->e = 0.0f; shape->u = 2.0f;
	shape->collision_type = 1;
	
	playerInstance.u = shape->u;
	playerInstance.shape = shape;
	playerInstance.groundShapes = cpArrayNew(0);
	shape->data = &playerInstance;
	playerInstance.isJumping = 0;
	playerInstance.fair = 0;
	playerInstance.vair = 0;
	
	cpSpaceAddCollisionHandler(space, 1, 2, begin, preSolve, NULL, separate, NULL);
	
	return space;
}


void InitializeGame(){

	cpInitChipmunk();
	space = initSpace();
	s = initializeSprite();
	addSprites(s, "dude/run%d.png", 27, 0.03);
	addSprites(s, "dude/jumpprep%d.png", 5, 0.1);
	addSprites(s, "dude/idle%d.png", 22, 0.1);

}

static void
updatephysics(float ticks)
{
	static int lastJumpState = 0;
	int jumpState = (arrowDirection.y > 0.0f);
	
	cpBody *body = playerInstance.shape->body;
	
	cpVect groundNormal = playerInstance.groundNormal;
	if(groundNormal.y > 0.0f){
		playerInstance.shape->surface_v = cpvmult(cpvperp(groundNormal), 400.0f*arrowDirection.x);
		if(arrowDirection.x) cpBodyActivate(body);
	} else {
		playerInstance.shape->surface_v = cpvzero;
	}

	// apply jump
	if(jumpState && !lastJumpState && cpvlengthsq(groundNormal)){
//		body->v = cpvmult(cpvslerp(groundNormal, cpv(0.0f, 1.0f), 0.5f), 500.0f);
		body->v = cpvadd(body->v, cpvmult(cpvslerp(groundNormal, cpv(0.0f, 1.0f), 0.75f), 500.0f));
		cpBodyActivate(body);
	}
	
	if(playerInstance.groundShapes->num == 0){
		cpFloat air_accel = body->v.x + arrowDirection.x*(2000.0f);
		body->f.x = body->m*air_accel;
		body->v.x = cpflerpconst(body->v.x, 400.0f*arrowDirection.x, 2000.0f*ticks);
		playerInstance.isJumping = 1;
		playerInstance.fair += body->m*air_accel;
		playerInstance.vair += cpflerpconst(body->v.x, 400.0f*arrowDirection.x, 2000.0f*ticks);
		//printf("sem ground shapes \n");
	}else{
		if (playerInstance.isJumping){
			body->v.x = 0;
			body->f.x = 0;
		//	playerInstance.vair = playerInstance.fair = 0;
			playerInstance.isJumping = 0;
		}
	}
	

	lastJumpState = jumpState;
	cpSpaceStep(space, ticks);
}


int Update( event* e, float* dt ){

	int x = 0, y = 0;

	if (e->keys[KEY_LEFT]){
		spriteOrientation = 0;
		x -= 1;
	}
	if (e->keys[KEY_RIGHT]){
		spriteOrientation = FLIP_Y;
		x += 1;
	}

	if (e->keys[KEY_UP]){
		y += 1;
	}

	if (e->keys[KEY_DOWN]){
		y -= 1;
	}	

	if (!e->keys[KEY_UP])
		y = 0;
	
	arrowDirection = cpv(x, y);

	updatephysics(*dt);

	printf("delta time %f \n", *dt);	
	
}

int HandleEvent(){
}

static void
drawSegmentShape(cpBody *body, cpSegmentShape *seg, cpSpace *space)
{
	cpVect a = seg->ta;
	cpVect b = seg->tb;
	
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES); {
		glVertex2f(a.x, a.y);
		glVertex2f(b.x, b.y);
	} glEnd();
}


static const GLfloat circleVAR[] = {
	 0.0000f,  1.0000f,
	 0.2588f,  0.9659f,
	 0.5000f,  0.8660f,
	 0.7071f,  0.7071f,
	 0.8660f,  0.5000f,
	 0.9659f,  0.2588f,
	 1.0000f,  0.0000f,
	 0.9659f, -0.2588f,
	 0.8660f, -0.5000f,
	 0.7071f, -0.7071f,
	 0.5000f, -0.8660f,
	 0.2588f, -0.9659f,
	 0.0000f, -1.0000f,
	-0.2588f, -0.9659f,
	-0.5000f, -0.8660f,
	-0.7071f, -0.7071f,
	-0.8660f, -0.5000f,
	-0.9659f, -0.2588f,
	-1.0000f, -0.0000f,
	-0.9659f,  0.2588f,
	-0.8660f,  0.5000f,
	-0.7071f,  0.7071f,
	-0.5000f,  0.8660f,
	-0.2588f,  0.9659f,
	 0.0000f,  1.0000f,
	 0.0f, 0.0f, // For an extra line to see the rotation.
};
static const int circleVAR_count = sizeof(circleVAR)/sizeof(GLfloat)/2;

static void
drawCircleShape(cpBody *body, cpCircleShape *circle, cpSpace *space)
{
	glVertexPointer(2, GL_FLOAT, 0, circleVAR);

	glPushMatrix(); {
		cpVect center = circle->tc;
		glTranslatef(center.x, center.y, 0.0f);
		glRotatef(body->a*180.0f/M_PI, 0.0f, 0.0f, 1.0f);
		glScalef(circle->r, circle->r, 1.0f);
		
		if(!circle->shape.sensor){
			glDrawArrays(GL_TRIANGLE_FAN, 0, circleVAR_count - 1);
		}
		
		glColor3f(1.0, 1.0, 1.0);
		glDrawArrays(GL_LINE_STRIP, 0, circleVAR_count);
	} glPopMatrix();
}


static void
drawObject(cpShape *shape, cpSpace *space)
{
	cpBody *body = shape->body;
	
	switch(shape->klass->type){
		case CP_CIRCLE_SHAPE:
			drawCircleShape(body, (cpCircleShape *)shape, space);
			int frame = 0;
			if (arrowDirection.x == 0 && arrowDirection.y == 0)
				frame = 2; //parado
			else
				frame = 0; //correndo

			if (body->v.y > 0.001 && arrowDirection.y > 0.001){
				frame = 1; //pulando
				spriteOrientation |= REPEAT_LAST;	
			}
//			else if (boby->v.y < 0)

		//	printf(" %f %f \n ", body->v.x, body->v.y);
			drawSprite(s, body->p.x-45.0, body->p.y-63.0, ElapsedTime, frame, spriteOrientation );
			break;
		case CP_SEGMENT_SHAPE:
			drawSegmentShape(body, (cpSegmentShape *)shape, space);
			break;
		case CP_POLY_SHAPE:
//			drawPolyShape(body, (cpPolyShape *)shape, space);
			break;
		default:
			printf("Bad enumeration in drawObject().\n");
	}
}


int Render(event *e, float* dt){

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);


	beginGUI(e);
		rect r;
		r.x = 30;
		r.y = 500;
		if (doButton(1, &r, "Mob de Braid"))
			printf("Clicou, mazela \n");

//		bindSamplerState(0, 0);
//		drawSprite(s, 200, 100, *dt, 0, spriteOrientation );
		ElapsedTime = *dt;
//		cpSpaceHashEach(space->activeShapes, (cpSpaceHashIterator)drawObject, space);
//		cpSpaceHashEach(space->staticShapes, (cpSpaceHashIterator)drawObject, space);

	endGUI();

	begin2d();
		cpSpaceHashEach(space->activeShapes, (cpSpaceHashIterator)drawObject, space);
		cpSpaceHashEach(space->staticShapes, (cpSpaceHashIterator)drawObject, space);
	end2d();


}

int main(){

	setVideoMode(800, 600, 0);
	warpMouse(0);
	mainrenderer  = initializeRenderer(800,  600, 0.1, 10000.0, 45.0, TRACKBALL);
	setTextureSamplerState(CLAMP, LINEAR, LINEAR);
	initializeGUI(800, 600);
	InitializeGame();
	MainLoop( );


}




