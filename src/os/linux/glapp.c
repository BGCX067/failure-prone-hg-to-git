#include "glapp.h"

//#ifndef WIN

#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/xf86vmode.h>
#include <GL/glx.h>
//#include <GL/glxext.h>
#include "GL3/glxext.h"
#include <sys/time.h>
#include <time.h>
#include <string.h> //memset

float keysDelay[512];
static Display* display = NULL;
static Window* window = NULL;
static int screen;
static GLXContext context;
static XF86VidModeModeInfo** modes;
static Atom WM_DELETE_WINDOW;
int warpmouse_ = 0;
glapp* app;

static int modescmp(const void* pa, const void* pb){
	XF86VidModeModeInfo* a = *(XF86VidModeModeInfo**) pa;
	XF86VidModeModeInfo* b = *(XF86VidModeModeInfo**) pb;
	if (a->hdisplay  > b->hdisplay) 
		return -1;
	return b->vdisplay - a->vdisplay;
}

void SetVideoMode(int w, int h, int fullscreen){
	if (window){
		XDestroyWindow(display, window);
		/* mudar de modo? */
	}

	int attrib [] = {
		GLX_X_RENDERABLE    , True,
	        GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
	        GLX_RENDER_TYPE     , GLX_RGBA_BIT,
	        GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
		GLX_RED_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 24,
		GLX_STENCIL_SIZE, 8,
		GLX_DOUBLEBUFFER, True,
		None
	};

	display = XOpenDisplay(NULL);

	screen = DefaultScreen(display);
	Window  rootWindow = RootWindow(display, screen);
	glapp* appwin =  (glapp*)malloc(sizeof(glapp));
	appwin->width = w;
	appwin->height = h;
	appwin->warpmouse = 1;

	GLXFBConfig* fbc = None;
	int fbcCount = 0;
	fbc =  glXChooseFBConfig( display, screen, attrib, &fbcCount );
	if (!fbc){
		printf("Invalid framebuffer config. \n");
		return NULL;
	}
		
	XVisualInfo* visualInfo  = glXGetVisualFromFBConfig( display, fbc[ 0 ] ); // glXChooseVisual(display, screen, attrib);
	if (!visualInfo){
		printf("Invalid visual info\n");
		return NULL;
	}

	Colormap currentColormap  = XCreateColormap(display, rootWindow, visualInfo->visual, AllocNone);
 	XSetWindowAttributes currentSetWindowAttibutes;
	currentSetWindowAttibutes.colormap = currentColormap;
	currentSetWindowAttibutes.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | StructureNotifyMask | PointerMotionMask;
	window = XCreateWindow(display, rootWindow, 0, 0, appwin->width, appwin->height, 0, visualInfo->depth, InputOutput, visualInfo->visual, CWColormap | CWEventMask, &currentSetWindowAttibutes);

	if (!window){
		printf("Cant create window\n");
		return NULL;
	}

	XMapWindow(display, window);

	/*if(fullscreen) {
		int i, nmodes;
		XF86VidModeModeLine mode;
		if(XF86VidModeGetModeLine(display,screen,&nmodes,&mode) && XF86VidModeGetAllModeLines(display,screen,&nmodes,&modes)) {
			qsort(modes,nmodes,sizeof(XF86VidModeModeInfo*),modescmp);
			for(i = nmodes - 1; i > 0; i--) if (modes[i]->hdisplay >= appwin->width && modes[i]->vdisplay >= appwin->height) break;
				if(modes[i]->hdisplay != mode.hdisplay || modes[i]->vdisplay != mode.vdisplay) {
					appwin->width = modes[i]->hdisplay;
					appwin->height = modes[i]->vdisplay;
					XF86VidModeSwitchToMode(display,screen,modes[i]);
				}
				XF86VidModeSetViewPort(display,screen,0,0);
		} else{
			printf("Nao aceita fullscreen \n");
			fullscreen = 0;
		}
	}
		
	attr.background_pixel = 0;
	attr.border_pixel = 0;
	if(fullscreen) {
		mask = CWBackPixel | CWColormap | CWOverrideRedirect | CWSaveUnder | CWBackingStore | CWEventMask;
		attr.override_redirect = True;
		attr.backing_store = NotUseful;
		attr.save_under = False;
	} else {
		mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
	}
	
	appwin->depth = visualInfo->depth;
	*/	
	
	if(fullscreen) {
		XMoveWindow(display,window,0,0);
		XRaiseWindow(display,window);
		XWarpPointer(display,None,window,0,0,0,0, appwin->width / 2, appwin->height / 2);
		XFlush(display);
		XF86VidModeSetViewPort(display,screen,0,0);
		XGrabPointer(display,window,True,0,GrabModeAsync,GrabModeAsync,window,None,CurrentTime);
		XGrabKeyboard(display,window,True,GrabModeAsync,GrabModeAsync,CurrentTime);
	} else {
		WM_DELETE_WINDOW = XInternAtom(display,"WM_DELETE_WINDOW",False);
		XSetWMProtocols(display,window,&WM_DELETE_WINDOW,1);
	}
		
	XFlush(display);


	// criacao de contexto anterior ao opengl 3
	//context = glXCreateContext(display, visualInfo, NULL, True);

	 PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = NULL;

        unsigned int attribList[] =
        {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
		GLX_CONTEXT_MINOR_VERSION_ARB, 0,
		GLX_CONTEXT_FLAGS_ARB, 0,
		GLX_CONTEXT_PROFILE_MASK_ARB, 0,
		0
	};

	//TODO passar por parametro?
	attribList[1] = 3;
	attribList[3] = 3;
	attribList[5] = GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
	attribList[7] = GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;

	//inicializa a extension na mao
	glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress( (const unsigned  char *)"glXCreateContextAttribsARB");
	if (!glXCreateContextAttribsARB){
		printf("glCreateContextAttribs not supported.\n");
		return;
	}

	 if (!(context = glXCreateContextAttribsARB( display, fbc[ 0 ], 0, True, attribList ))){
	 	printf("glCreateContetAttribs returned null.");
	 	return;
	}

		
	if (!context){
		printf("context ta null\n");
		return;
	}

	glXMakeCurrent(display, window, context);

	app = appwin;
	return;	

}

void CloseVideo(){
	if(context)
        glXDestroyContext(display, context);
	if(window)
        XDestroyWindow(display,  window);
	if(display)
        XCloseDisplay(display);
}

void SetWindowTitle(char* title){
	XStoreName(display, window, title);
	XSetIconName(display, window, title);
}

void SetMouse(int x, int y){
	XWarpPointer(display, None, window, 0, 0, 0, 0, x, y);
	XFlush(display);
}

//retorna o tempo em milisegundos
double GetTime(){
	struct timespec time;
	clock_gettime(CLOCK_REALTIME, &time);
	return time.tv_sec + time.tv_nsec*1e-9;
}

static int getKeyCode(int key){
	int ret = 0;
    
    //TODO ordenar alfabeticamente as teclas pra facilitar a leitura
	switch(key){
		case XK_Escape: ret = KEY_ESC; break;
		case XK_Left: 	ret = KEY_LEFT; break;
		case XK_Up: 	ret = KEY_UP; break;
		case XK_Right:	ret = KEY_RIGHT; break;
		case XK_Down:	ret = KEY_DOWN; break;
		case XK_BackSpace: ret = KEY_BACKSPACE; break;
		case XK_space:	ret = KEY_SPACE; break;
		case XK_a:	ret = KEY_a; break;
		case XK_s:	ret = KEY_s; break;
		case XK_d:	ret = KEY_d; break;
		case XK_w:	ret = KEY_w; break;
		case XK_e:	ret = KEY_e; break;
		case XK_r:	ret = KEY_r; break;
		case XK_t:	ret = KEY_t; break;
		case XK_y:	ret = KEY_y; break;
		case XK_u:	ret = KEY_u; break;
		case XK_i:	ret = KEY_i; break;
		case XK_o:	ret = KEY_o; break;
		case XK_p:	ret = KEY_p; break;
		case XK_q:	ret = KEY_q; break;
		case XK_f:	ret = KEY_f; break;
		case XK_g:	ret = KEY_g; break;
		case XK_h:	ret = KEY_h; break;
		case XK_j:	ret = KEY_j; break;
		case XK_k:	ret = KEY_k; break;
		case XK_l:	ret = KEY_l; break;
		case XK_z:	ret = KEY_z; break;
		case XK_x:	ret = KEY_x; break;
		case XK_c:	ret = KEY_c; break;
		case XK_v:	ret = KEY_v; break;
		case XK_b:	ret = KEY_b; break;
		case XK_n:	ret = KEY_n; break;
		case XK_m:	ret = KEY_m; break;
		case XK_0:	ret = KEY_0; break;
		case XK_1:	ret = KEY_1; break;
		case XK_2:	ret = KEY_2; break;
		case XK_3:	ret = KEY_3; break;
		case XK_4:	ret = KEY_4; break;
		case XK_5:	ret = KEY_5; break;
		case XK_6:	ret = KEY_6; break;
		case XK_7:	ret = KEY_7; break;
		case XK_8:	ret = KEY_8; break;
		case XK_9:	ret = KEY_9; break;
		case XK_period:	ret = KEY_period; break;
		case XK_minus:	ret = KEY_minus; break;
		case XK_Control_L:	ret = KEY_LCONTROL; break;
		case XK_Control_R:	ret = KEY_RCONTROL; break;
		case XK_Shift_L:	ret = KEY_LSHIFT; break;
		case XK_Shift_R:	ret = KEY_RSHIFT; break;
	}
	return ret;
}

void MainLoop(){
	event evt;
    evt.buttonLeft =  evt.buttonRight = 0;
	memset(evt.keys, 0, 512*sizeof(int));
	memset(keysDelay, 0, 512*sizeof(int));
	KeySym key;
	double t = 0.0;
	double dt = 0.01; //TODO deixar configuravel
	double currentTime = 0.0;
	double accumulator = 0.0;
	double deltaTime;
	float fps = 60;
	//float ifps = 1/fps;
    
	//setMouse(app->width/2, app->height/2);
	while(1){
        evt.type = NO_EVENT;
        evt.wheel = 0;
		while(XPending(display)){
			XEvent  event;
			XNextEvent(display,  &event);
			switch(event.type){
				case ClientMessage:
					if (event.xclient.format == 32 && event.xclient.data.l[0] == (long)WM_DELETE_WINDOW)
						return;
					break;
				case ConfigureNotify:
					app->width = event.xconfigure.width;
					app->height = event.xconfigure.height;
					/* Seta o viewport novo*/
					break;
				case KeyPress:
					XLookupString(&event.xkey, NULL, 0, &key, NULL);
					key = getKeyCode(key);
					evt.keys[key] = 1;
					evt.type |= KEYBOARD_EVENT;
                    break;
				case KeyRelease:
					XLookupString(&event.xkey, NULL, 0, &key, NULL);
					key = getKeyCode(key);
					evt.type |= KEYBOARD_EVENT;
					evt.keys[key] = 0;
					break;
				case MotionNotify:
					if(event.xmotion.x != app->width || event.xmotion.y != app->height) {
						evt.x = event.xmotion.x;
						evt.y = event.xmotion.y;
						evt.type |= MOUSE_MOTION_EVENT;
					}
					break;
                /**
                 * FIXME:
                 * No X eventos de mousewheel são tratados como ButtonPress.
                 * Logo ao mesmo tempo é feito um evento de ButtonRelease (?)
                 * No enum de buttons, BUTTON_UP e BUTTON_DOWN correspondem ao
                 * que o X utiliza para WHEEL_UP e WHEEL_DOWN
                 **/
				case ButtonPress:
					evt.button |= 1 << (event.xbutton.button - 1);
                    evt.type |= MOUSE_BUTTON_PRESS;
                    evt.x = event.xbutton.x;
                    evt.y = event.xbutton.y;
					if (event.xbutton.button == 1)
						evt.buttonLeft = 1;
					else if (event.xbutton.button == 3)
						evt.buttonRight = 1;
                    else if (event.xbutton.button == 4) {
                        evt.wheel = 1;
                        evt.type &= ~MOUSE_BUTTON_PRESS;
                        evt.type |= MOUSE_WHEEL;
                    } else if (event.xbutton.button == 5) {
                        evt.wheel = -1;
                        evt.type &= ~MOUSE_BUTTON_PRESS;
                        evt.type |= MOUSE_WHEEL;
                    }
					break;
				case ButtonRelease:
					evt.button  &= ~( 1 <<event.xbutton.button - 1  );
                    if((event.xbutton.button != 4) && (event.xbutton.button != 5))
                        evt.type |= MOUSE_BUTTON_RELEASE;
					evt.buttonLeft = 0;
					evt.buttonRight = 0;
                    evt.x = event.xbutton.x;
                    evt.y = event.xbutton.y;
					break;
			}

		}
		//printf("FPS: %f\n", fps);

		double newTime = GetTime();
		deltaTime = newTime - currentTime;
		currentTime = newTime;

		if (deltaTime > 0.25)
			deltaTime = 0.25;

		accumulator += deltaTime;
		while (accumulator >= dt){
			accumulator -= dt;
			t += dt;
			//TODO chamar a fisica aqui (nao sei como)
//			Update(&evt, &dt);
		}

		//deveria interpolar o estado aqui
		Update(&evt, &deltaTime);
		Render(&evt, &deltaTime);

		glXSwapBuffers(display, window);
		evt.button &= ~(BUTTON_UP | BUTTON_DOWN);

		if (warpmouse_)
            SetMouse(app->width/2, app->height/2);
	}
}

void WarpMouse( int i){
	warpmouse_ = i;
}

void SwapBuffers() {
    glXSwapBuffers(display, window);    
}

unsigned int GetScreenW() {
    return app->width;
}

unsigned int GetScreenH() {
    return app->height;
}

//#endif

