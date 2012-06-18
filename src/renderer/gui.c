#include "renderer.h"
#include "gui.h"
#include "../math/matrix.h"
#include <stdlib.h>
#include <math.h>
#ifdef __APPLE__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif
#include <stdio.h>
#include <string.h>
#include "util/fontstash.h"
#include "../glapp.h"
#include "glime.h"

GUI* gui;
event* guiEv;
mat4 ortho;

#define BORDER_SIZE 4

const char* WidgetVSSource = {
    "#version 330\n\
    layout (location = 0) in vec3 inpos;\n\
    layout (location = 8) in vec3 intexcoords; \n\
    uniform mat4 ortho; \n\
    out vec3 texcoords; \n\
    \n\
    void main()\n\
    {\n\
        gl_Position = ortho * vec4(inpos, 1.0);\n\
	texcoords = intexcoords; \n\
    }\n\
    "};

//fragshader que so imprime uma cor
const char* FSColorSource = {
    "#version 330 \n\
    uniform vec4 color; \n\
    out vec4 outcolor; \n\
    void main() \n\
    { \n\
        outcolor = color; \n\
    } \n\
"};
    
const char* WidgetFSSource = {
    "#version 330\n\
    uniform vec4 fillColor;\n\
    uniform vec4 borderColor /*= vec4( 1.0, 1.0,1.0,1.0)*/;\n\
    uniform vec2 zones;\n\
    out vec4 outcolor;\n\
    in vec3 texcoords;\n\
    \n\
    void main()\n\
    {\n\
        float doTurn = float(texcoords.y > 0);\n\
        float radiusOffset = doTurn * abs( texcoords.z );\n\
        float turnDir = sign(texcoords.z );\n\
        vec2 uv = vec2(texcoords.x + turnDir*radiusOffset, texcoords.y);\n\
        float l = abs( length(uv) - radiusOffset );\n\
        float a = clamp( l - zones.x, 0.0, 2.0);\n\
        float b = clamp( l - zones.y, 0.0, 2.0);\n\
        b = exp2(-2.0*b*b);\n\
        outcolor = ( fillColor * b + (1.0-b)*borderColor );\n\
        outcolor *= exp2(-2.0*a*a);\n\
    }\n\
    "};

const char* TexViewWidgetFSSource = {
    "#version 120\n\
    uniform float mipLevel /*= 0*/;\n\
    uniform float texelScale /*= 1.0*/;\n\
    uniform float texelOffset /*= 0.0*/;\n\
    uniform ivec4 texelSwizzling /*= ivec4( 0, 1, 2, 3)*/;\n\
    uniform sampler2D samp;\n\
    \n\
    void main()\n\
    {\n\
        vec4 texel;\n\
        if (mipLevel > 0)\n\
            texel = texture2DLod( samp, gl_TexCoord[0].xy, mipLevel);\n\
        else\n\
            texel = texture2D( samp, gl_TexCoord[0].xy);\n\
      //  texel = texel * texelScale + texelOffset;\n\
    //    gl_FragColor  = texel.x * vec4( texelSwizzling.x == 0, texelSwizzling.y == 0, texelSwizzling.z == 0, texelSwizzling.w == 0 );\n\
     //   gl_FragColor += texel.y * vec4( texelSwizzling.x == 1, texelSwizzling.y == 1, texelSwizzling.z == 1, texelSwizzling.w == 1 );\n\
     //   gl_FragColor += texel.z * vec4( texelSwizzling.x == 2, texelSwizzling.y == 2, texelSwizzling.z == 2, texelSwizzling.w == 2 );\n\
      //  gl_FragColor += texel.w * vec4( texelSwizzling.x == 3, texelSwizzling.y == 3, texelSwizzling.z == 3, texelSwizzling.w == 3 );\n\
	gl_FragColor = vec4(texel.rgb, 1.0); \n\
//	gl_FragColor = vec4(gl_TexCoord[0].x, 0.0, 0.0, 1.0); \n\
    }\n\
    "};




int initializeGUI(int w, int h){

	gui = malloc( sizeof(GUI));
	gui->widgetShader  = initializeShader( WidgetVSSource, WidgetFSSource );
	gui->colorShader = initializeShader( WidgetVSSource, FSColorSource);
	gui->textureViewShader = initializeShader( WidgetVSSource, TexViewWidgetFSSource  );

	gui->w = w;
	gui->h = h;
	gui->hotitem = gui->activeitem = gui->twophase = 0;
	gui->drawingMenu = 0;
	gui->menuoffsetx = gui->menuoffsety = 0.0;

	gui->stash = sth_create(512, 512);

	//da pra desenhar a gui mesmo sem fonte
	if (!sth_add_font(gui->stash, 0, "data/fonts/DroidSerif-Regular.ttf")){
		printf("Font not found\n");
	}

	gui->widgetColor[0][0] = 89.0/255.0; gui->widgetColor[0][1] = 89.0/255.0; gui->widgetColor[0][2] = 89.0/255.0; gui->widgetColor[0][3] = 0.7;
	gui->widgetColor[1][0] = 166.0/255.0; gui->widgetColor[1][1] = 166.0/255.0; gui->widgetColor[1][2] = 166.0/255.0; gui->widgetColor[1][3] = 0.8;
	gui->widgetColor[2][0] = 212.0/255.0; gui->widgetColor[2][1] = 228.0/255.0; gui->widgetColor[2][2] = 60.0/255.0; gui->widgetColor[2][3] = 0.5;
	gui->widgetColor[3][0] = 227.0/255.0; gui->widgetColor[3][1] = 237.0/255.0; gui->widgetColor[3][2] = 127.0/255.0; gui->widgetColor[3][3] = 0.5;

	gui->borderColor[0][0] = 255.0/255.0; gui->borderColor[0][1] = 255.0/255.0; gui->borderColor[0][2] = 255.0/255.0; gui->borderColor[0][3] = 1.0;
	gui->borderColor[1][0] = 255.0/255.0; gui->borderColor[1][1] = 255.0/255.0; gui->borderColor[1][2] = 255.0/255.0; gui->borderColor[1][3] = 1.0;
	gui->borderColor[2][0] = 255.0/255.0; gui->borderColor[2][1] = 255.0/255.0; gui->borderColor[2][2] = 255.0/255.0; gui->borderColor[2][3] = 1.0;
	gui->borderColor[3][0] = 255.0/255.0; gui->borderColor[3][1] = 255.0/255.0; gui->borderColor[3][2] = 255.0/255.0; gui->borderColor[3][3] = 1.0;

	gui->fontColor[0][0] = 255.0/255.0; gui->fontColor[0][1] = 255.0/255.0; gui->fontColor[0][2] = 255.0/255.0; gui->fontColor[0][3] = 1.0;
	gui->fontColor[1][0] = 255.0/255.0; gui->fontColor[1][1] = 255.0/255.0; gui->fontColor[1][2] = 255.0/255.0; gui->fontColor[1][3] = 1.0;
	gui->fontColor[2][0] = 255.0/255.0; gui->fontColor[2][1] = 255.0/255.0; gui->fontColor[2][2] = 255.0/255.0; gui->fontColor[2][3] = 1.0;
	gui->fontColor[3][0] = 255.0/255.0; gui->fontColor[3][1] = 255.0/255.0; gui->fontColor[3][2] = 255.0/255.0; gui->fontColor[3][3] = 1.0;

	return 1;
}

void beginGUI(event* e){

	gui->hotitem = 0;
	gui->kbditem = 0;
	gui->lastwidget = 0;

	guiEv = e;

/*	glDisable(GL_STENCIL_TEST);
	glStencilMask( 0 );
	glDisable(GL_DEPTH_TEST);
	glDepthMask( GL_FALSE );

	glEnable(GL_BLEND);
*/	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	begin2d();
	fpOrtho(ortho, 0, gui->w, 0, gui->h, -1.0, 1.0);

}

void endGUI(){
	end2d();
//	glDisable(GL_BLEND);
}

//testa se um ponto ta dentro de um rect
int insideRect(rect* r, int x, int y){
	y = gui->h -y;
	return (x  >= r->x) && (x < r->x + r->w) && (y >= r->y) && (y < r->y + r->h );
}

//verifica se esta na area do widget
int isHover(rect *r){

	if (gui->twophase) return  insideRect(r, gui->focusx, gui->focusy );
	return insideRect(r, guiEv->x, guiEv->y);

}

void drawRect(rect* r, int fillColor, int borderColor){

	bindShader(gui->widgetShader);	
	setShaderConstant4f(gui->widgetShader, "fillColor", gui->widgetColor[fillColor]);
	setShaderConstant4f(gui->widgetShader, "borderColor", gui->borderColor[borderColor] );
	setShaderConstant4x4f(gui->widgetShader, "ortho", ortho);
	float zones[] = {0.0, 0.0};
	setShaderConstant2f(gui->widgetShader, "zones", zones);

	float x0 = r->x;
	float x1 = r->x + r->w;
	        
	float y0 = r->y;
	float y1 = r->y + r->h;

	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0, 0);
		glVertex2f( x0, y0);
		glVertex2f( x1, y0);
		glVertex2f( x0, y1);
		glVertex2f( x1, y1);
	glEnd();

	bindShader(0);
}

void drawRoundedRect( rect* rect,  point* corner, int fillColorId, int borderColorId ){

	setShaderConstant4f(gui->widgetShader, "fillColor",  gui->widgetColor[fillColorId]);
	setShaderConstant4f(gui->widgetShader, "borderColor", gui->borderColor[borderColorId] );
	setShaderConstant4x4f(gui->widgetShader, "ortho", ortho);
	float zones[2]; zones[0] = corner->x - 1; zones[1] = corner->x-2;
	setShaderConstant2f(gui->widgetShader, "zones", zones);
	bindShader(gui->widgetShader);


    float xb = corner->x;
    float yb = corner->y;
    float x0 = rect->x;
    float x1 = rect->x + corner->x;
    float x2 = rect->x + rect->w - corner->x;
    float x3 = rect->x + rect->w;
    float y0 = rect->y;
    float y1 = rect->y + corner->y;
    float y2 = rect->y + rect->h - corner->y;
    float y3 = rect->y + rect->h;


    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(xb, yb);
        glVertex2f( x0, y0);
        glTexCoord2f(0, yb);
        glVertex2f( x1, y0);

        glTexCoord2f(xb, 0);
        glVertex2f( x0, y1);
        glTexCoord2f(0, 0);
        glVertex2f( x1, y1);

        glTexCoord2f(xb, 0);
        glVertex2f( x0, y2);
        glTexCoord2f(0, 0);
        glVertex2f( x1, y2);

        glTexCoord2f(xb, yb);
        glVertex2f( x0, y3);
        glTexCoord2f(0, yb);
        glVertex2f( x1, y3);
    glEnd();

   glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0, yb);
        glVertex2f( x2, y0);
        glTexCoord2f(xb, yb);
        glVertex2f( x3, y0);

        glTexCoord2f(0, 0);
        glVertex2f( x2, y1);
        glTexCoord2f(xb, 0);
        glVertex2f( x3, y1);

        glTexCoord2f(0, 0);
        glVertex2f( x2, y2);
        glTexCoord2f(xb, 0);
        glVertex2f( x3, y2);
 
        glTexCoord2f(0, yb);
        glVertex2f( x2, y3);
        glTexCoord2f(xb, yb);
        glVertex2f( x3, y3);
    glEnd();

    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0, yb);
        glVertex2f( x1, y0);
        glTexCoord2f(0, yb);
        glVertex2f( x2, y0);

        glTexCoord2f(0, 0);
        glVertex2f( x1, y1);
        glTexCoord2f(0, 0);
        glVertex2f( x2, y1);

        glTexCoord2f(0, 0);
        glVertex2f( x1, y2);
        glTexCoord2f(0, 0);
        glVertex2f( x2, y2);
 
        glTexCoord2f(0, yb);
        glVertex2f( x1, y3);
        glTexCoord2f(0, yb);
        glVertex2f( x2, y3);
    glEnd();

	bindShader(0);

}

void drawDownArrow( rect* rect, int width, int fillColorId, int borderColorId ){

    float offset = sqrt(2.0)/2.0 ;
   
    float xb = width;
    float yb = width;
    
    float xoff = offset * xb ;
    float yoff = offset * yb ;
    float xoff2 = offset * xb *2.0 ;
    float yoff2 = offset * yb *2.0;

    float x0 = rect->x + xoff2;
    float x1 = rect->x + rect->w * 0.5;
    float x2 = rect->x + rect->w - xoff2;

    float y0 = rect->y + rect->h * 0.1 + yoff2;
    float y1 = rect->y + rect->h * 0.6;

    
	setShaderConstant4f(gui->widgetShader, "fillColor", gui->widgetColor[fillColorId] );
	setShaderConstant4f(gui->widgetShader, "borderColor", gui->borderColor[borderColorId] );
	setShaderConstant4x4f(gui->widgetShader, "ortho", ortho);
	float zones[2]; zones[0] = xb - 1; zones[1] = xb-2;
	setShaderConstant2f(gui->widgetShader, "zones", zones);
	    bindShader(gui->widgetShader);

   glBegin(GL_TRIANGLE_STRIP);
        glTexCoord3f(-xb, -yb, 0);
        glVertex2f( x0, y1 + yoff2);
        glTexCoord3f(xb, -yb, 0);
        glVertex2f( x0 - xoff2, y1);
 
        glTexCoord3f(-xb, 0, 0);
        glVertex2f( x0 + xoff, y1 + yoff);
        glTexCoord3f(xb, 0, 0);
        glVertex2f( x0 - xoff, y1 - yoff);

        glTexCoord3f(-xb, 0, xb);
        glVertex2f( x1, y0 + yoff2);
        glTexCoord3f(xb, 0, xb);
        glVertex2f( x1 - xoff2, y0);

        glTexCoord3f(xb, 2*yb, xb);
        glVertex2f( x1, y0 - yoff2);

    glEnd();
    glBegin(GL_TRIANGLE_STRIP);
        glTexCoord3f(xb, -yb, 0);
        glVertex2f( x2 + xoff2, y1);
        glTexCoord3f(-xb, -yb, 0);
        glVertex2f( x2, y1 + yoff2);

        glTexCoord3f(xb, 0, xb);
        glVertex2f( x2 + xoff, y1 - yoff);
        glTexCoord3f(-xb, 0, xb);
        glVertex2f( x2 - xoff, y1 + yoff);

        glTexCoord3f(xb, 0, xb);
        glVertex2f( x1 + xoff2, y0);
        glTexCoord3f(-xb, 0, xb);
        glVertex2f( x1, y0 + yoff2);

        glTexCoord3f(xb, 2*yb, xb);
        glVertex2f( x1, y0 - yoff2);

    glEnd();

    bindShader(0);
}

void doLine(int x1, int y1, int x2, int y2){

	glDisable(GL_BLEND);
	//printf("x1 %d y1 %d x2 %d y2 %d \n", x1, y1, x2, y2 );
	glBegin(GL_LINES);
		glVertex2f((float)x1,  (float)y1);
		glVertex2f((float)x2,  (float)y2);
	glEnd();
	glEnable(GL_BLEND);
	
}

void doCircle(int x, int y, int radius){
	rect r;
	r.x = (int)x;
	r.y = (int)y;
	r.w = r.h = radius*2;
	point p;
	p.x = (int) radius;
	p.y = (int) radius;
	printf("doCircle %d %d \n ", r.x, r.y);
	gui->widgetColor[0][0] = 255.0/255.0; gui->widgetColor[0][1] = 0.0/255.0; gui->widgetColor[0][2] = 0.0/255.0; gui->widgetColor[0][3] = 0.7;
	drawRoundedRect( &r,  &p, 0, 0 );
	//drawRect(&r, 0, 0);
	gui->widgetColor[0][0] = 89.0/255.0; gui->widgetColor[0][1] = 89.0/255.0; gui->widgetColor[0][2] = 89.0/255.0; gui->widgetColor[0][3] = 0.7;

}

//TODO fazer funcionar
void plot1d(float* serie, int num, int x, int y, int width, int h, int r, int g, int b){

	
	glLineWidth(1.0);
	if (serie == NULL)
		return;

	float max, min;
	max = min = serie[0];
	for (int i = 0; i < num; i++){
		if (max < serie[i]) max = serie[i];
		if (min > serie[i]) min = serie[i];
	}

	//draw y axis
	glDisable(GL_BLEND);
	glColor3f(195.0/255.0, 195.0/255.0, 195.0/255.0);
	glBegin(GL_LINES);
		glVertex2f(x, y);
		glVertex2f(x, y+h);
	glEnd();
	//draw x axis
	glBegin(GL_LINES);
		float y0 =  y + ( h - (max - 0)*h/(max-min));
		glVertex2f(x, y0);
		glVertex2f(x+width, y0);
	glEnd();

	glColor3f(0.8, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);
		for (int i = 0; i < num; i++){
			float n = num;
			float x1 = x + (width/n)*i;
			float y1 = y + ( h - (max - serie[i])*h/(max-min));  //y + (serie[i]/max)*h;
		//	printf("i %d x1 %f y1 %f n %f w %f (w/n) %f (w/n)*i %f \n ",i, x1, y1, n, width, (width/n), (width/n)*i);
			glVertex2f( x1 , y1 );
		}
	glEnd();
	glEnable(GL_BLEND);

}

void drawFrame(rect* rect, point corner, int isHover, int  isDown){

	int color =  (isHover) + (isDown << 1);

	if (corner.x + corner.y == 0)
        	drawRect( rect , color, 0);
	else
		drawRoundedRect( rect, &corner , color, 0);

}

float getTextLineWidth(char* text){

	float minx, maxx, miny, maxy, totalwidth;
	sth_dim_text(gui->stash, 0, 20.0, text, &minx, &miny, &maxx, &maxy, &totalwidth  );

	return totalwidth;

}

float getFontHeight(){
	float ascender, descender, lineh;
	sth_vmetrics(gui->stash, 0, 20.0, &ascender, &descender, &lineh );
	return lineh;
}

void doLabel( rect* textRect, char* text ){
	textRect->w = getTextLineWidth(text) + BORDER_SIZE*2;
	textRect->h = getFontHeight();
	if (gui->drawingMenu){
		textRect->x += gui->menuoffsetx;
		textRect->y += gui->menuoffsety;
	}
	sth_begin_draw(gui->stash);
	sth_draw_text(gui->stash, 0, 20, textRect->x, textRect->y, text, &textRect->x, gui->fontColor[0]);
	sth_end_draw(gui->stash);
}

int beginMenu(int id, int x, int y, int w, int h, float* xoffset, float* yoffset, char* text){

	gui->drawingMenu = 1;
	
	rect r;
	r.x = x + (*xoffset);
	r.y = y + (*yoffset);
	r.w = w;
	r.h = h;
	point corner;
	corner.x = 8;
	corner.y = 8;


	gui->menuoffsetx = (*xoffset);
	gui->menuoffsety = (*yoffset);
	drawRoundedRect( &r, &corner , 0, 0);

	return 1;
}

void endMenu(int id, int x, int y, int w, int h, float* xoffset, float* yoffset){

	rect r;
	r.x = x + (*xoffset);
	r.y = y + (*yoffset);
	r.w = w;
	r.h = h;

	int hover = isHover(&r);
	if (hover && (gui->hotitem == 0)){
		gui->hotitem = id;
		if (guiEv->buttonLeft && (gui->activeitem == 0)){
			gui->activeitem = id;
		}
	}

	if ( (guiEv->buttonLeft) && hover && gui->hotitem == id && gui->activeitem == id){

	//	if (gui->activeitem == 0){
	//		gui->activeitem = id;
	//	}

		int xs = r.x + r.w/2;
		int xfinal = guiEv->x - xs;

		int ys = r.y + r.h/2;
		int yfinal = (gui->h - guiEv->y) - ys;

		(*xoffset) += xfinal;
		(*yoffset) += yfinal;
		gui->menuoffsetx = (*xoffset);
		gui->menuoffsety = (*yoffset);
		r.x += xfinal;
		r.y += yfinal;

	}

	if( !guiEv->buttonLeft && (gui->hotitem == id) && (gui->activeitem == id) ){
		gui->activeitem = 0;
		return 1;
	}

	gui->drawingMenu = 0;
	gui->menuoffsetx = gui->menuoffsety = 0;
}

int doButton(int  id, rect* r, char* text){

	//calcula a posicao do textRect baseado na fonte e na posicao inicial do rect
	rect textRect;
	textRect.x = r->x + BORDER_SIZE;
	textRect.y = r->y + BORDER_SIZE + 3;
	textRect.w = getTextLineWidth(text) + BORDER_SIZE*2;
	textRect.h = getFontHeight();

	//calcula o height e width do rect do button com base no texto
	r->h = textRect.h + 2*BORDER_SIZE;
	r->w = textRect.w + 2*BORDER_SIZE;

	if (gui->drawingMenu){
		r->x += gui->menuoffsetx;
		r->y += gui->menuoffsety;
	}


	int hover = isHover(r);
	if (hover){
		gui->hotitem = id;
		if (guiEv->buttonLeft && (gui->activeitem == 0)){
			gui->activeitem = id;
		}
	}

	point p;
	p.x = 4;
	p.y = 4;

	drawFrame(r, p, hover, (gui->activeitem == id));
	doLabel(&textRect, text);

//	se o mouse ta up mas o item da hot e active entao o cara clicou
	if( !guiEv->buttonLeft && (gui->hotitem == id) && (gui->activeitem == id) ){
		gui->activeitem = 0;
		return 1;
	}

	return 0;
}

void drawBoolFrame( rect* r, point* p, int isHover, int isDown){
	int color =  (isHover) + (isDown << 1);
	drawRoundedRect( r, p , color, 0 );

}

//state eh passado por referencia, caso o botao seja clicado ele fica em 1, caso o contrario em 0
// cada checkbutton precisa do seu proprio state
int doCheckButton(int id, rect  *r, char* text, int * state){

	rect rText, rCheck;

	int rcOffset = (int) 0.125*16;
	rCheck.h = 16 - 2*rcOffset;
	rCheck.w = rCheck.h; //quadrado
	rCheck.x = BORDER_SIZE + rcOffset + r->x;
	rCheck.y = BORDER_SIZE + rcOffset + r->y;

	rText.x = 16 + 2*BORDER_SIZE + r->x;
	rText.y = BORDER_SIZE + r->y;
	rText.w = getTextLineWidth(text) + 2*BORDER_SIZE;
	rText.h = getFontHeight() + BORDER_SIZE; 

	point p;
	p.x = rCheck.w/6;
	p.y = rCheck.h/6;

	if (gui->drawingMenu){
		r->x += gui->menuoffsetx;
		r->y += gui->menuoffsety;
	}


	int hover = isHover(r);
	if (hover){
		gui->hotitem = id;
		if ( (guiEv->buttonLeft) && (gui->activeitem == 0)){
			gui->activeitem = id;
		}
	}

	drawBoolFrame(&rCheck, &p, hover, (state) && (*state));
	doLabel(&rText, text);

	if( !guiEv->buttonLeft && (gui->hotitem == id) && (gui->activeitem == id) ){
		gui->activeitem = 0;
		*state = !*state;
		return 1;
	}

	return 0;

}

//no radiobutton os N botoes precisam do mesmo *state
int doRadioButton(int id, rect* r, char* text, int *state){

	rect rText, rCheck;

	int rcOffset = (int) 0.125*16;
	rCheck.h = 16 - 2*rcOffset;
	rCheck.w = rCheck.h; //quadrado
	rCheck.x = BORDER_SIZE + rcOffset + r->x;
	rCheck.y = BORDER_SIZE + rcOffset + r->y;

	rText.x = 16 + 2*BORDER_SIZE + r->x;
	rText.y = BORDER_SIZE + r->y;
	rText.w = getTextLineWidth(text) + 2*BORDER_SIZE;
	rText.h = getFontHeight() + BORDER_SIZE; 

	point p;
	p.x = rCheck.w/6;
	p.y = rCheck.h/6;

	if (gui->drawingMenu){
		r->x += gui->menuoffsetx;
		r->y += gui->menuoffsety;
	}


	int hover = isHover(r);
	if (hover)
		gui->hotitem = id;

	if ( (guiEv->buttonLeft) && hover){
		if (gui->activeitem == 0){
			gui->activeitem = id;
		}
	}

	drawBoolFrame(&rCheck, &p, hover, (state) && (*state == id));// diferente do checkbox
	doLabel(&rText, text);

	if( !guiEv->buttonLeft && (gui->hotitem == id) && (gui->activeitem == id) ){
		gui->activeitem = 0;
		if (*state == id) //diferente do checkbox
			*state = 0;
		else
			*state = id;
		return 1;
	}
	return 0;
}

void doHorizontalSlider(int id, rect* r, float* value){

	if (gui->drawingMenu){
		r->x += gui->menuoffsetx;
		r->y += gui->menuoffsety;
	}

	float min = 0;
	float max = 1.0;

	//normaliza o valor
	float f = (*value - min) / (max - min);
	if (f < 0.0) f = 0.0;
	else if (f > 1.0) f = 1.0;

	rect rScroll, rCircle;

	r->w = 100 + 2*BORDER_SIZE + 12 ; //100 = tamanho base do scroll, 12 = tamanho do circulo
	r->h = 4 + 2*BORDER_SIZE;

	rScroll.x = BORDER_SIZE;
	rScroll.y = BORDER_SIZE;
	rScroll.h = 4; //altura do scroll
	rScroll.w = 100 + 12;

	rCircle.x = (int)(f*(rScroll.w));
	rCircle.h = rScroll.h*3;
	rCircle.y = rScroll.y - 4;
	rCircle.w = rCircle.h; //circulo quadrado
	

	int hover = isHover(r);
	if (hover)
		gui->hotitem = id;

	if ( (guiEv->buttonLeft) && hover){

		if (gui->activeitem == 0){
			gui->activeitem = id;
		}

		int xs = r->x + BORDER_SIZE;// + rCircle.w/2;
		int x = guiEv->x - xs;

		if (x < 0) 
			x = 0;
		else if (x > rScroll.w ) 
			x = rScroll.w ;

		rCircle.x = x;

		float f = (float) (x) / (float)(rScroll.w);
		f = f*(max - min);

		if (fabs(f - *value) > (max - min) * 0.001){
			*value = f;
		}
	}

	
	rScroll.x += r->x;	
	rScroll.y += r->y;
	rScroll.w += 6; // meio circulo
	point p;
	p.x = 2;
	p.y = 2;

	drawFrame(&rScroll, p, hover, 0);

	rCircle.x += r->x;
	rCircle.y += r->y;
	p.x = rCircle.w/2;
	p.y = rCircle.h/2;
	drawFrame(&rCircle, p, 0, (gui->activeitem == id));

	if( !guiEv->buttonLeft && (gui->hotitem == id) && (gui->activeitem == id) ){
		gui->activeitem = 0;
	}

}

void doVerticalSlider(int id, rect* r, float* value){

	if (gui->drawingMenu){
		r->x += gui->menuoffsetx;
		r->y += gui->menuoffsety;
	}

	float min = 0;
	float max = 1.0;

	//normaliza o valor
	float f = (*value - min) / (max - min);
	if (f < 0.0) f = 0.0;
	else if (f > 1.0) f = 1.0;

	rect rScroll, rCircle;

	r->h = 100 + 2*BORDER_SIZE + 12 ; //100 = tamanho base do scroll, 12 = tamanho do circulo
	r->w = 4 + 2*BORDER_SIZE;

	rScroll.x = BORDER_SIZE;
	rScroll.y = BORDER_SIZE;
	rScroll.w = 4; //altura do scroll
	rScroll.h = 100 + 12;

	rCircle.y = (int)(f*(rScroll.h));
	rCircle.h = rScroll.w*3;
	rCircle.x = rScroll.y - 4;
	rCircle.w = rCircle.h; //circulo quadrado
	

	int hover = isHover(r);
	if (hover)
		gui->hotitem = id;

	if ( (guiEv->buttonLeft) && hover){

		if (gui->activeitem == 0){
			gui->activeitem = id;
		}

		int ys = r->y + BORDER_SIZE;
		int y = (gui->h - guiEv->y) - ys;

		if (y < 0) 
			y = 0;
		else if (y > rScroll.h ) 
			y = rScroll.h;

		rCircle.y = y;

		float f = (float) (y) / (float)(rScroll.h);
		f = f*(max - min);

		if (fabs(f - *value) > (max - min) * 0.001){
			*value = f;
		}
	}

	
	rScroll.x += r->x;	
	rScroll.y += r->y;
	rScroll.h += 6; // meio circulo
	point p;
	p.x = 2;
	p.y = 2;

	drawFrame(&rScroll, p, hover, 0);

	rCircle.x += r->x;
	rCircle.y += r->y;
	p.x = rCircle.w/2;
	p.y = rCircle.h/2;
	drawFrame(&rCircle, p, 0, (gui->activeitem == id));

	if( !guiEv->buttonLeft && (gui->hotitem == id) && (gui->activeitem == id) ){
		gui->activeitem = 0;
	}


}


void calculateComboRect(rect* r, int numOptions, char* options[], int selected, rect* rt, rect* rd){

	rt->x = 3;
	rt->y = 3;

	float ascender, descender, lineh;
	sth_vmetrics(gui->stash, 0, 20.0, &ascender, &descender, &lineh );

	rt->h = lineh;
	r->h = rt->h + 2*rt->y;

	rd->h = rt->h;
	rd->w = rd->h;
	rd->y = rt->y;

	rt->w = 0;
	for(int i = 0; i <numOptions; i++){
		float minx, maxx, miny, maxy, totalwidth;
	//	printf("text combo: %s \n", options[i]);
		sth_dim_text(gui->stash, 0, 20.0, options[i], &minx, &miny, &maxx, &maxy, &totalwidth  );
		rt->w = ( totalwidth > rt->w ? totalwidth : rt->w);
	}

	r->w = rt->w + 2*rt->x;
	//espaco pra seta
	r->w += rd->w + rt->x;
	rd->x = 2*rt->x + rt->w;

}

void drawComboBox(rect* r, int numOptions, char* options[], rect* rt, rect* ra, int selected, int isHover, int isDown ){

	point p;
	p.x = rt->x;
	p.y = rt->y;
	drawFrame(r, p, isHover, 0);

	sth_begin_draw(gui->stash);
	rt->x += r->x;
	rt->y += r->y;
	sth_draw_text(gui->stash, 0, 20.0, rt->x, rt->y, options[selected], &rt->x, gui->fontColor[0]);
	sth_end_draw(gui->stash);

	ra->x += r->x;
	ra->y += r->y;
	drawDownArrow(ra, ra->h*0.15,  (!isHover) + (isDown << 2), 0 );

}

void drawListBox(rect* r, int numOptions, char* options[], rect* ri, rect* rt, int selected, int hovered){

	point p;
	p.x = ri->x;
	p.y = ri->y;
//	printf("RECT %d %d %d %d \n", r->x, r->y, r->w, r->h );
	drawFrame(r, p, 0, 0);

	rect ir;
	ir.x = r->x +ri->x;
	ir.y = r->y + r->h - ri->y -ri->h;
	ir.w = ri->w;
	ir.h = ri->h;

	for(int i = 0;i <numOptions; i++){

		if (i == hovered || i == selected){
			p.x = ri->x;
			p.y = ri->y;
			drawFrame(&ir,p, 0, (i == selected) );

		}

		rect rtext;

		sth_begin_draw(gui->stash);
		rtext.x = ir.x + rt->x;
		rtext.y = ir.y + rt->y;
		rtext.h = rt->h;
		rtext.w = rt->w;
		sth_draw_text(gui->stash, 0, 20.0, rtext.x, rtext.y, options[i], &rtext.x, gui->fontColor[0]);
		sth_end_draw(gui->stash);

		ir.y -= ir.h;
	}


}

void calculateListRect(rect* r, int numOptions, char* options[], rect* ri, rect* rt){

	ri->x = 3;
	ri->y = 3;
	rt->x = 3;
	rt->y = 3;

	rt->w = 0;
	for(int i = 0; i <numOptions; i++){
		float minx, maxx, miny, maxy, totalwidth;
	//	printf("text combo: %s \n", options[i]);
		sth_dim_text(gui->stash, 0, 20.0, options[i], &minx, &miny, &maxx, &maxy, &totalwidth  );
		rt->w = ( totalwidth > rt->w ? totalwidth : rt->w);
	}
	ri->w = rt->w + 2*rt->x;
	r->w = ri->w + 2*ri->x;

	rt->h = 20; //lineh
	ri->h = rt->h + rt->y;
	r->h = numOptions*ri->h + 2*ri->y;
}

int doComboBox(int id, rect* r, int numOptions, char* options[], int* selected, int * state){

	rect rt, rd;
	calculateComboRect(r, numOptions, options, *selected, &rt, &rd);

	int hover = isHover(r);
	int hovered = -1;

	if (hover)
		gui->hotitem = id;

	if ( guiEv->buttonLeft && hover){
		*state = 1;
		gui->activeitem = id;
	}

	if (*state == 1){
		rect ro, ri, rit;//ele retorna em ro e nao em r, apenas nesse caso
		rect nullrect;
		calculateListRect(&nullrect, numOptions, options, &ri, &rit );
		ro.x = r->x;
		ro.y = r->y - nullrect.h;
		ro.w = nullrect.w;
		ro.h = nullrect.h;

		ro.h += 2;
		int hoveroptions = insideRect(&ro, guiEv->x, guiEv->y);
		if (hoveroptions){
			hovered = numOptions - 1 - (gui->h - guiEv->y - (ro.y+ri.y)) / (ri.h);
		}

		drawComboBox(r, numOptions, options, &rt, &rd, *selected, hover, 0 );
		drawListBox(&ro, numOptions, options, &ri, &rit, *selected, hovered);


		if ((!isHover(&ro) || !isHover(&r)) && !guiEv->buttonLeft){
			*state = 0;
			gui->activeitem = 0;
		}

		if (!guiEv->buttonLeft && hovered > -1 ){
			*selected = hovered;
			gui->activeitem = 0;
			*state = 0;
		}

	}else{
		drawComboBox(r, numOptions, options, &rt, &rd, *selected, hover, 0 );
	}


}

int doListBox(int id, rect* r, int numOptions, char* options[], int* selected){

	rect ri, rt;
	calculateListRect(r, numOptions, options, &ri, &rt);

	int hover = isHover(r);
	int hovered = -1;

	if (hover){
		gui->hotitem = id;
		hovered = numOptions - 1 - ( (gui->h - guiEv->y) - (r->y+ri.y))/(ri.h);
		if (guiEv->buttonLeft && (gui->activeitem == 0)){
			gui->activeitem = id;
		}	
	}

	int lSelected = 1;
	if (selected)
		lSelected = *selected;

	drawListBox(r, numOptions, options, &ri, &rt, lSelected, hovered);

	if( !guiEv->buttonLeft && (gui->hotitem == id) && (gui->activeitem == id) && (lSelected != hovered) ){
		printf("clicou hovered: %d selected: %d \n", hovered, *selected);
		gui->activeitem = 0;
		if (selected)
			*selected = hovered;
		return 1;
	}


}

void getLineEditRect(rect* r, char* text, rect* rt, int maxTextLength){

	rt->x = 3;
	rt->y = 3;
	rt->w = 100;
	rt->h = 20;

	r->w = rt->w + 2*rt->x;
	r->h = rt->h + 2*rt->y;

}

void drawLineEdit(rect* r, char* text, rect* rt, int hover ){
	point p;
	p.x = rt->x; p.y = rt->y;
	drawFrame(r, p, hover, 0);
	rt->x = r->x + rt->x;
	rt->y = r->y + rt->y + 3;
	sth_begin_draw(gui->stash);
	sth_draw_text(gui->stash, 0, 20.0, rt->x, rt->y, text, &rt->x, gui->fontColor[0]);
	sth_end_draw(gui->stash);
}

int drawTextureView(rect* r, int texID, rect* rt, rect* rz, int mipLevel, float texelScale, float texelOffset, int red, int green, int blue, int alpha)
{
    point p; 
    p.x = rt->x;
    p.y = rt->y;
    drawFrame( r, p, 0, 0 );

    bindTexture(0, texID);
//   glBindTexture(GL_TEXTURE_2D,  texID);

    setShaderConstant1f( gui->textureViewShader, "mipLevel", (float) mipLevel);
    setShaderConstant1f( gui->textureViewShader, "texelScale", texelScale);
    setShaderConstant1f( gui->textureViewShader, "texelOffset", texelOffset);
    float swizz[4];
    swizz[0] = red;
    swizz[1] = green;
    swizz[2] = blue;
    swizz[3] = alpha;
    setShaderConstant4f( gui->textureViewShader, "texelSwizzling", swizz);
    bindShader(gui->textureViewShader);

    glBegin(GL_QUADS);
	glTexCoord2f( (float) rz->x / (float) rt->w , (float) rz->y / (float) rt->h);
        glVertex2f(r->x + rt->x, r->y + rt->y );

	glTexCoord2f( (float) rz->x / (float) rt->w , (float) (rz->y + rz->h) / (float) rt->h);
        glVertex2f(r->x + rt->x, r->y - rt->y + r->h);

	glTexCoord2f( (float) (rz->x+rz->w) / (float) rt->w , (float) (rz->y + rz->h) / (float) rt->h);
        glVertex2f(r->x + rt->x + rt->w, r->y - rt->y + r->h);

	glTexCoord2f( (float) (rz->x+rz->w) / (float) rt->w , (float) (rz->y) / (float) rt->h);
        glVertex2f(r->x + rt->x + rt->w, r->y + rt->y);
    glEnd();

    bindShader(0);
}

int doLineEdit(int id,  rect* r, char* text, int maxTextLength ){

	rect rt;
	int len = strlen(text);
	int changed = 0;
	getLineEditRect(r, text, &rt, maxTextLength);

	int hover = isHover(r);

	if (hover){
		gui->hotitem = id;
		if (guiEv->buttonLeft && (gui->activeitem == 0)){
		//	printf("button left no line edit %d \n ", id);
			gui->activeitem = id;
		}	
	}


	drawLineEdit(r, text, &rt, hover);
	if (gui->activeitem == id && len < maxTextLength){
		for(int kid = 32; kid < 127; kid++){
			if (guiEv->keys[kid]){
				text[len] = (char) kid;
				len++;
				text[len] = 0;
				changed = 1;
				guiEv->keys[kid] = 0;
			}
		}
		if (guiEv->keys[KEY_BACKSPACE]){
			if (len > 0){
				len--;
				text[len] = 0;
				changed = 1;
			}
			guiEv->keys[KEY_BACKSPACE] = 0;
		}
	}


	if (!hover && guiEv->buttonLeft && gui->activeitem == id){
//		printf("nao ta hover e clicou fora do lineedit \n");
		gui->activeitem = 0;
	}

	return changed;
}

void calculateTextureViewRect(rect* r, rect* rt){

	if (r->w == 0)
		r->w = 100;

	if (r->h == 0)
		r->h = r->w;

	rt->x = 3;
	rt->y = 3;
	rt->w = r->w - 2*3;
	rt->h = rt->h - 2*3;

}

//valores default
// miplevel -1 texscale 1.0 texeloffset 0.0 red 0, green 1, blue 2, alpha 3
int doTextureView( rect* r, unsigned int texid,  rect* zoomrect, int miplevel, float texscale, float texeloffset,
int red, int green, int blue, int alpha){

	rect rt;
	calculateTextureViewRect(r, &rt);

	if ( zoomrect->w == 0 || zoomrect->h == 0){
		zoomrect->x = zoomrect->y = 0;
		zoomrect->h = rt.h;
		zoomrect->w = rt.w;
	}

	drawTextureView(r, texid, &rt, zoomrect, miplevel, texscale, texeloffset, red, green, blue, alpha);

}

