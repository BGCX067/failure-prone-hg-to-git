#include <GL/glfw.h>
#include "../glapp.h"

void setVideoMode(int x, int y, int fullscreen){

	if (!glfwInit())
		return;

	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
	glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int mode = GLFW_WINDOW;
	if (fullscreen)
		mode = GLFW_FULLSCREEN;

	if (!glfwOpenWindow(x, y, 0,0,0,0,24,0, mode)){
		glfwTerminate();
		return;
	}

	printf("OpenGL Version Major %d \n", glfwGetWindowParam(GLFW_OPENGL_VERSION_MAJOR));
	printf("OpenGL Version Minor %d \n", glfwGetWindowParam(GLFW_OPENGL_VERSION_MINOR));
	printf("OpenGL Forward Compat %d \n", glfwGetWindowParam(GLFW_OPENGL_FORWARD_COMPAT));	
	printf("OpenGL Core Profile %d \n", (glfwGetWindowParam(GLFW_OPENGL_PROFILE) == GLFW_OPENGL_CORE_PROFILE));
	printf("Depth Buffer Bits %d \n",   glfwGetWindowParam(GLFW_DEPTH_BITS));	

}

void closeVideo(){
	glfwTerminate();
}

int getKeyCode( int k){

	int ret = 0;
	switch(k){

		case 49: ret = KEY_1; break;
		case 50: ret = KEY_2; break;
		case 51: ret = KEY_3; break;
		case 52: ret = KEY_4; break;
		case 53: ret = KEY_5; break;
		case 54: ret = KEY_6; break;
		case 55: ret = KEY_7; break;
		case 56: ret = KEY_8; break;
		case 57: ret = KEY_9; break;
		case 58: ret = KEY_0; break;

		case 81: ret = KEY_q; break;
		case 87: ret = KEY_w; break;
		case 69: ret = KEY_e; break;
		case 82: ret = KEY_r; break;
		case 84: ret = KEY_t; break;
		case 89: ret = KEY_y; break;
		case 85: ret = KEY_u; break;
		case 73: ret = KEY_i; break;
		case 79: ret = KEY_o; break;
		case 80: ret = KEY_p; break;

		case 65: ret = KEY_a; break;
		case 83: ret = KEY_s; break;
		case 68: ret = KEY_d; break;
		case 70: ret = KEY_f; break;
		case 71: ret = KEY_g; break;
		case 72: ret = KEY_h; break;
		case 74: ret = KEY_j; break;
		case 75: ret = KEY_k; break;
		case 76: ret = KEY_l; break;

		case 90: ret = KEY_z; break;
		case 88: ret = KEY_x; break;
		case 67: ret = KEY_c; break;
		case 86: ret = KEY_v; break;
		case 66: ret = KEY_b; break;
		case 78: ret = KEY_n; break;
		case 77: ret = KEY_m; break;

		case 257: ret = KEY_ESC; break;
		case 258: ret = KEY_F1; break;
		case 259: ret = KEY_F2; break;
		case 260: ret = KEY_F3; break;
		case 261: ret = KEY_F4; break;
		case 262: ret = KEY_F5; break;
		case 263: ret = KEY_F6; break;
		case 264: ret = KEY_F7; break;
		case 265: ret = KEY_F8; break;
		case 266: ret = KEY_F9; break;
		case 267: ret = KEY_F10; break;
		case 268: ret = KEY_F11; break;
		case 269: ret = KEY_F12; break;
		case 32 : ret = KEY_SPACE; break;
		case 283: ret = KEY_UP; break;
		case 284: ret = KEY_DOWN; break;
		case 285: ret = KEY_LEFT; break;
		case 286: ret = KEY_RIGHT; break;
		case 287: ret = KEY_LSHIFT; break;
		case 288: ret = KEY_RSHIFT; break;
		case 289: ret = KEY_LCONTROL; break;
		case 290: ret = KEY_RCONTROL; break;
		case 291: ret = KEY_LALT; break;
		case 292: ret = KEY_RALT; break;
		case 295: ret = KEY_BACKSPACE; break;
		case 293: ret = KEY_TAB; break;
		case 294: ret = KEY_ENTER; break;
		case 296: ret = KEY_INSERT; break;
		case 297: ret = KEY_DEL; break;
		case 298: ret = KEY_PAGEUP; break;
		case 299: ret = KEY_PAGEUP; break;
		case 300: ret = KEY_HOME; break;
		case 301: ret = KEY_END; break;

	}

	return ret;
}

event evt;

void keyboardcallback(int key, int state){
	printf("key %d \n", key);
	evt.type |= KEYBOARD_EVENT;
	if (state == GLFW_PRESS)
		evt.keys[getKeyCode(key)] = 1;
	else
		evt.keys[getKeyCode(key)] = 0;

}

//x y relative to upper left corner
void mousemotioncallback(int x, int y){
	evt.type |= MOUSE_MOTION_EVENT;
	evt.x = x;
	evt.y = y;
}

void mousebuttoncallback(int button, int state){

	if (button == GLFW_MOUSE_BUTTON_LEFT){
		if (state == GLFW_PRESS){
			evt.buttonLeft = 1;
			evt.type |= MOUSE_BUTTON_PRESS;
		}else{
			evt.type |= MOUSE_BUTTON_RELEASE;
			evt.buttonLeft = 0;
		}
	}else if (button == GLFW_MOUSE_BUTTON_RIGHT){
		if (state == GLFW_PRESS){
			evt.buttonRight = 1;
			evt.type |= MOUSE_BUTTON_PRESS;
		}else{
			evt.type |= MOUSE_BUTTON_RELEASE;
			evt.buttonRight = 0;
		}

	}

}

double getTime(){
	return glfwGetTime();
}

void MainLoop(){

	evt.buttonLeft =  evt.buttonRight = 0;
	memset(evt.keys, 0, 512*sizeof(int));
	int mousex, mousey;
	int key;
	glfwGetMousePos(&mousex, &mousey);
	glfwSetKeyCallback(keyboardcallback);
	glfwSetMousePosCallback(mousemotioncallback);
	glfwSetMouseButtonCallback(mousebuttoncallback);

	double t = 0.0;
	double dt = 0.01; // TODO deixar isso configuravel
	double currentTime = getTime();
	double accumulator = 0.0;
	float deltaTime;


	while (glfwGetWindowParam( GLFW_OPENED )) {

		if (evt.keys[KEY_ESC])//TODO turar isso na versao final
			break;

		double newTime = getTime();
		deltaTime = newTime - currentTime;
		currentTime = newTime;
		
		if (deltaTime > 0.25f)
			deltaTime = 0.25f;
		
		accumulator += deltaTime;
		
		while (accumulator >= dt){ //dt é 0.01 que eh a timeslice, a engine é atualiza em timeslaces constantes desse tamanho
			accumulator -= dt;
			t += dt; //t eh o tempo total
		}

		Update(&evt, &dt);
		Render(&evt, &dt);
		glfwSwapBuffers();
		evt.button &= ~(BUTTON_UP | BUTTON_DOWN); // tirar o mouse motion event tb
		evt.type |= ~KEYBOARD_EVENT;
	}


}

void setWindowTitle(char* title){
	glfwSetWindowTitle( title);
}

void warpmouse( int yesno){
}


