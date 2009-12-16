
#include "glapp.h"

#ifndef WIN

#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/xf86vmode.h>
#include <GL/glx.h>
#include <sys/time.h>
#include <time.h>

static Display* display = NULL;
static Window* window = NULL;
static int screen;
static GLXContext context;
static XF86VidModeModeInfo** modes;
static Atom WM_DELETE_WINDOW;

static int modescmp(const void* pa, const void* pb){
	XF86VidModeModeInfo* a = *(XF86VidModeModeInfo**) pa;
	XF86VidModeModeInfo* b = *(XF86VidModeModeInfo**) pb;
	if (a->hdisplay  > b->hdisplay) 
		return -1;
	return b->vdisplay - a->vdisplay;
}

glapp* setVideoMode(int w, int h, int fullscreen){

	if (window){
		XDestroyWindow(display, window);
		/* mudar de modo? */
	}

	int attrib [] = {
		GLX_RGBA,
		GLX_RED_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 24,
		GLX_STENCIL_SIZE, 8,
		GLX_DOUBLEBUFFER,
		None
	};

	display = XOpenDisplay(NULL);
	screen = DefaultScreen(display);
	Window  rootWindow = RootWindow(display, screen);
	glapp* appwin =  (glapp*)malloc(sizeof(glapp));
	appwin->width = w;
	appwin->height = h;

	XVisualInfo* visualInfo  =glXChooseVisual(display, screen, attrib);
	if (!visualInfo){
		printf("Invalid visual info\n");
		return NULL;
	}

	if(fullscreen) {
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
		
	XSetWindowAttributes attr;
	unsigned long mask;
	attr.background_pixel = 0;
	attr.border_pixel = 0;
	attr.colormap = XCreateColormap(display,rootWindow,visualInfo->visual,AllocNone);
	attr.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask;
	if(fullscreen) {
		mask = CWBackPixel | CWColormap | CWOverrideRedirect | CWSaveUnder | CWBackingStore | CWEventMask;
		attr.override_redirect = True;
		attr.backing_store = NotUseful;
		attr.save_under = False;
	} else {
		mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
	}
	
	appwin->depth = visualInfo->depth;
	window = XCreateWindow(display,rootWindow,0,0,appwin->width,appwin->height,0,visualInfo->depth,InputOutput,visualInfo->visual,mask,&attr);
	XMapWindow(display,window);
		
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
		
	if(!context) 
		context = glXCreateContext(display, visualInfo, NULL, True);
		
	if (!context)
		return NULL;

	glXMakeCurrent(display, window, context);
	
	return appwin;	

}

void closeVideo(){

	if (context)
		glXDestroyContext(display, context);

	if (window)
		XDestroyWindow(display,  window);

	if (display)
		XCloseDisplay(display);

}

void setWindowTitle(char* title){
	XStoreName(display, window, title);
	XSetIconName(display, window, title);
}

void setMouse(int x, int y){
	XWarpPointer(display, None, window, 0, 0, 0, 0, x, y);
	XFlush(display);
}

int getTime(){
	struct timeval tval;
//	struct timezone tzone;

	gettimeofday(&tval, NULL);

	return tval.tv_sec*1000 + tval.tv_usec/1000;
}

int getKeyCode(int key){

	int ret = 0;

	switch(key){
		case XK_Escape: ret = KEY_ESC; break;
		case XK_Left: 	ret = KEY_LEFT; break;
		case XK_Up: 	ret = KEY_UP; break;
		case XK_Right:	ret = KEY_RIGHT; break;
		case XK_Down:	ret = KEY_DOWN; break;
	}

	return ret;
}


void mainloop(glapp* app, keyboard* keyboard, mouse* mouse, int(idle)(void* ), int(render)(void*) ){
	KeySym key;
	int startTime =  getTime();
	int endTime = 0;
	int counter = 0;
	float fps = 60;
	float ifps = 1/fps;

	while(1){
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
					keyboard->keys[key] = 1;
					break;
				case KeyRelease:
					XLookupString(&event.xkey, NULL, 0, &key, NULL);
					key =  getKeyCode(key);
					keyboard->keys[key] = 0;
					break;
				case MotionNotify:
					mouse->x = event.xmotion.x;
					mouse->y = event.xmotion.y;
					break;
				case ButtonPress:
					mouse->button |= 1 << (event.xbutton.button - 1);
					break;
				case ButtonRelease:
					mouse->button  &= ~( 1 <<event.xbutton.button - 1  );
					break;

			}
		}

		if (counter++ == 10){
			endTime = startTime;
			startTime = getTime();
			int elapsedTime = startTime - endTime;
			fps = counter * 1000.0/ (float) (elapsedTime);
			counter= 0;
		}
		ifps = 1/fps;


		if (idle)
			(*idle)( NULL );

		if (render)
			(*render)(NULL);

		glXSwapBuffers(display, window);
		mouse->button &= ~(BUTTON_UP | BUTTON_DOWN);
	}


}

#endif
