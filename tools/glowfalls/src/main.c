#include <stdio.h>
#include <stdlib.h>
#include "glapp.h"
//#include "math/algebra.h"
#include "renderer/renderer.h"
#include "renderer/sprite.h"
#include "renderer/camera.h"
#include "chipmunk/chipmunk.h"
#include "util/fparray.h"

#define GRABABLE_MASK_BIT (1<<31)
#define NOT_GRABABLE_MASK (~GRABABLE_MASK_BIT)

renderer *mainrenderer;
int spriteOrientation = 0;
sprite *s;
sprite* grass;

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

typedef struct gameObject {

	cpShape* shape;	
	sprite* gfx;
	int layer;

} GameObject;

fplist *gameobjects;

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


void InitializeGame(){

	gameobjects = fplist_init( free);

	s = initializeSprite();
	addSprites(s, "dude/run%d.png", 27, 0.03);
	addSprites(s, "dude/jumpprep%d.png", 5, 0.1);
	addSprites(s, "dude/idle%d.png", 22, 0.1);

	GameObject* g1 = malloc(sizeof(GameObject));
	grass = initializeSprite();
	if (addSprite(grass, "data/sprites/longgrass_1.png", 0) == 0)
		printf("grass sprite not found \n");

	g1->gfx = grass;
	g1->layer = 0;
	fplist_insback(g1, gameobjects);


	cpInitChipmunk();
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
	g1->shape = shape;

	/*shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(0,220), cpv(500,220), 0.0f));
	shape->e = 1.0f; shape->u = 1.0f;
	shape->layers = NOT_GRABABLE_MASK;
	shape->collision_type = 2;

	shape = cpSpaceAddShape(space, cpSegmentShapeNew(staticBody, cpv(500,100), cpv(800,250), 0.0f));
	shape->e = 1.0f; shape->u = 1.0f;
	shape->layers = NOT_GRABABLE_MASK;
	shape->collision_type = 2;
*/
	
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
	

	printf("Game Initialization Done. \n");

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


int Update( event* e, double* dt ){

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

//	printf("delta time %f \n", *dt);	
	
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


static void
drawObject(cpShape *shape, cpSpace *space)
{
	cpBody *body = shape->body;
	
	switch(shape->klass->type){
		case CP_CIRCLE_SHAPE:
		{	int frame = 0;
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
			drawSprite(s, body->p.x-45.0, body->p.y-63.0, 100, 100, ElapsedTime, frame, spriteOrientation );
			break;
		}
		default:
			printf("Bad enumeration in drawObject().\n");
	}
}


int Render(event *e, double* dt){

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	ElapsedTime = *dt;


	begin2d();

		for( int i = 0; i < gameobjects->size; i++){
			GameObject* obj = fplist_getdata(i, gameobjects);
			drawSprite(obj->gfx, 0, 10, 800, 100, 0, 0, REPEAT_LAST);
		}

//		drawSprite(grassObject.gfx, 0, 10, 800, 100, 0, 0, REPEAT_LAST);

		cpSpaceHashEach(space->activeShapes, (cpSpaceHashIterator)drawObject, space);
		//cpSpaceHashEach(space->staticShapes, (cpSpaceHashIterator)drawObject, space);
	end2d();


	beginGUI(e);
		rect r;
		r.x = 30;
		r.y = 500;
		if (doButton(1, &r, "Mob de Braid"))
			printf("Clicou, mazela \n");

	endGUI();


}

int main(){

	setVideoMode(800, 600, 0);
	warpmouse(0);
	mainrenderer  = initializeRenderer(800,  600, 0.1, 10000.0, 45.0, TRACKBALL);
	initializeGUI(800, 600);
	InitializeGame();
	MainLoop( );


}




