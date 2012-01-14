#include "renderer.h"
#include "gui.h"
#include <GL/gl.h>
#include <stdio.h>
#include "util/fontstash.h"
//#include "../glapp.h"
#include "glime.h"
//#include "util/fparray.h"
#define STB_TRUETYPE_IMPLEMENTATION
//#include "../util/stb_truetype.h"

GUI* gui;
event* guiEv;
struct sth_stash* stash;

//armazena a geometria de um widget
typedef struct widgetGeom{
	batch** batches;
	int numBatches;
} widgetMesh;
widgetMesh* widgetmeshes = NULL;
widgetCount = 0;
maxWidgets = 100;

#define norm255( i ) ( (float) ( i ) / 255.0f )

enum Color
{
    cBase = 0,
    cBool = 4,
    cOutline = 8,
    cFont = 12,
    cFontBack = 16,
    cTranslucent = 20,
    cDark = 24,
    cNbColors = 28,
};

const static float colors[cNbColors][4] =
{
    // cBase
    { norm255(89), norm255(89), norm255(89), 0.7f },
    { norm255(166), norm255(166), norm255(166), 0.8f },
    { norm255(212), norm255(228), norm255(60), 0.5f },
//    { norm255(96), norm255(96), norm255(96), 0.5f },
    { norm255(227), norm255(237), norm255(127), 0.5f },

    // cBool
    { norm255(99), norm255(37), norm255(35), 1.0f },
    { norm255(149), norm255(55), norm255(53), 1.0f },
    { norm255(212), norm255(228), norm255(60), 1.0f },
    { norm255(227), norm255(237), norm255(127), 1.0f },

    // cOutline
    { norm255(255), norm255(255), norm255(255), 1.0f },
    { norm255(255), norm255(255), norm255(255), 1.0f },
    { norm255(255), norm255(255), norm255(255), 1.0f },
    { norm255(255), norm255(255), norm255(255), 1.0f },

    // cFont
    { norm255(255), norm255(255), norm255(255), 1.0f },
    { norm255(255), norm255(255), norm255(255), 1.0f },
    { norm255(255), norm255(255), norm255(255), 1.0f },
    { norm255(255), norm255(255), norm255(255), 1.0f },

    // cFontBack
    { norm255(79), norm255(129), norm255(189), 1.0 },
    { norm255(79), norm255(129), norm255(189), 1.0 },
    { norm255(128), norm255(100), norm255(162), 1.0 },
    { norm255(128), norm255(100), norm255(162), 1.0 },
    
    // cTranslucent
    { norm255(0), norm255(0), norm255(0), 0.0 },
    { norm255(0), norm255(0), norm255(0), 0.0 },
    { norm255(0), norm255(0), norm255(0), 0.0 },
    { norm255(0), norm255(0), norm255(0), 0.0 },

	//cDark
    { norm255(64), norm255(64), norm255(64), 0.0 },
    { norm255(96), norm255(96), norm255(96), 0.0 },
    { norm255(255), norm255(0), norm255(0), 0.0 },
    { norm255(0), norm255(255), norm255(0), 0.0 }
};


const char* WidgetVSSource = {
    "#version 120\n\
    \n\
    void main()\n\
    {\n\
        gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n\
        gl_TexCoord[0] = gl_MultiTexCoord0;\n\
    }\n\
    "};

// @@ IC: Use standard GLSL. Do not initialize uniforms.
    
const char* WidgetFSSource = {
    "#version 120\n\
    uniform vec4 fillColor /*= vec4( 1.0, 0.0,0.0,1.0)*/;\n\
    uniform vec4 borderColor /*= vec4( 1.0, 1.0,1.0,1.0)*/;\n\
    uniform vec2 zones;\n\
    \n\
    void main()\n\
    {\n\
        float doTurn = float(gl_TexCoord[0].y > 0);\n\
        float radiusOffset = doTurn * abs( gl_TexCoord[0].z );\n\
        float turnDir = sign( gl_TexCoord[0].z );\n\
        vec2 uv = vec2(gl_TexCoord[0].x + turnDir*radiusOffset, gl_TexCoord[0].y);\n\
        float l = abs( length(uv) - radiusOffset );\n\
        float a = clamp( l - zones.x, 0.0, 2.0);\n\
        float b = clamp( l - zones.y, 0.0, 2.0);\n\
        b = exp2(-2.0*b*b);\n\
        gl_FragColor = ( fillColor * b + (1.0-b)*borderColor );\n\
        gl_FragColor.a *= exp2(-2.0*a*a);\n\
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
	gl_FragColor = vec4(texture2D(samp, gl_TexCoord[0].xy).rgb, 1.0); \n\
//	gl_FragColor = vec4(gl_TexCoord[0].x, 0.0, 0.0, 1.0); \n\
    }\n\
    "};




GUI* initializeGUI(int w, int h){

	gui = malloc( sizeof(GUI));
	gui->widgetShader  = initializeShader( WidgetVSSource, WidgetFSSource );
	gui->textureViewShader = initializeShader( WidgetVSSource, TexViewWidgetFSSource  );

	gui->w = w;
	gui->h = h;
	gui->hotitem = gui->activeitem = gui->twophase = 0;

	stash = sth_create(512, 512);
	if (!sth_add_font(stash, 0, "data/fonts/DroidSerif-Regular.ttf")){
		printf("Font not found\n");
	}
	gui->stash = stash;
	//widgetmeshes = malloc(sizeof(widgetMesh)*maxWidgets );
//	for(int i = 0; i < maxWidgets; i++)
//		widgetMeshes[i] = malloc(sizeof(batch));
	//memset(widgetmeshes, sizeof(widgetMesh), 0);

	return gui;
}

void beginGUI(event* e){

	gui->hotitem = 0;
	gui->kbditem = 0;
	gui->lastwidget = 0;

	guiEv = e;

	//TODO nao usar comandos diretamente do opengl aqui, usar apenas o renderer.
	glPushAttrib( GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glDisable(GL_STENCIL_TEST);
	glStencilMask( 0 );
	glDisable(GL_DEPTH_TEST);
	glDepthMask( GL_FALSE );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

	glMatrixMode( GL_MODELVIEW );
        glPushMatrix();
        glLoadIdentity();

	//coloca no modo ortho
	glMatrixMode( GL_PROJECTION );
    	glPushMatrix();
        glLoadIdentity();
	gluOrtho2D( 0, gui->w, 0, gui->h);

}

void endGUI(){

	//printf("enf gui\n");
//	gui->activeitem = 0;
	glPopAttrib();
	glMatrixMode( GL_PROJECTION);
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW);
	glPopMatrix();
}

//testa se um ponto ta dentro de um rect
int insideRect(rect* r, int x, int y){

//	printf("x: %d y: %d r.x: %d r.y: %d r.w: %d r.h: %d  \n", x, gui->h-y, r->w, r->y, r->w, r->h  );
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
	setShaderConstant4f(gui->widgetShader, "fillColor", colors[fillColor] );
	setShaderConstant4f(gui->widgetShader, "borderColor", colors[borderColor] );
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
	//bindShader(0);
}

void drawRoundedRect( rect* rect,  point* corner, int fillColorId, int borderColorId ){

//	printf( " fillcollor: %d  rect.y %d \n", fillColorId, rect->y );
//	bindShader(gui->widgetShader);	
	setShaderConstant4f(gui->widgetShader, "fillColor", colors[fillColorId] );
	setShaderConstant4f(gui->widgetShader, "borderColor", colors[borderColorId] );
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

    //if (widgetmeshes[id].numBatches == 0){
      //  widgetmeshes[id].numBatches = 3;
    //widgetmeshes[id].batches = malloc(sizeof(widgetMesh*)*3);
    /*batch* b1 = initializeBatch();
    batch* b2 = initializeBatch();
    batch* b3 = initializeBatch();
//	widgetmeshes[id].batches[0] = b1;

    begin(b1, GL_TRIANGLE_STRIP, 8, 1 );
    texCoord2f(b1, 0, xb, yb);
    vertex2f(b1, x0, y0);
    texCoord2f(b1, 0, 0, yb);
    vertex2f(b1, x1, y0);

    texCoord2f(b1, 0, xb, 0);
    vertex2f(b1, x0, y1);
    texCoord2f(b1, 0, 0, 0);
    vertex2f(b1, x1, y1);

    texCoord2f(b1, 0, xb, 0);
    vertex2f(b1, x0, y2);
    texCoord2f(b1, 0, 0, 0);
    vertex2f(b1, x1, y2);

    texCoord2f(b1, 0, xb, yb);
    vertex2f(b1, x0, y3);
    texCoord2f(b1, 0, 0, yb);
    vertex2f(b1, x1, y3);
    end(b1);
//}else*/
   //draw(widgetmeshes[id].batches[0]);
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

    
	setShaderConstant4f(gui->widgetShader, "fillColor", colors[fillColorId] );
	setShaderConstant4f(gui->widgetShader, "borderColor", colors[borderColorId] );
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

void drawFrame(rect* rect, point corner, int isHover, int  isDown){

	int colorNb =  cBase + (isHover) + (isDown << 1);

	//printf("ishover: %d isdown: %d colorNb: %d ", isHover, isDown, colorNb);

	if (corner.x + corner.y == 0)
        	drawRect( rect , colorNb, cOutline);//lColorNb, cOutline);
	else
		drawRoundedRect( rect, &corner , colorNb, cOutline );

}

void drawButton(rect *r, char* text, rect textRect, int isDown, int isHover){

	point p;
	p.x = textRect.x; p.y = textRect.y;
//	printf("text %s ", text);
	drawFrame(r, p, isHover, isDown);
	rect rtext;
	rtext.x = r->x + textRect.x;
	rtext.y = r->y + textRect.y + 3;
	rtext.w = textRect.w;
	rtext.h = textRect.h;
	sth_begin_draw(stash);
	glColor4fv(colors[cFont]);
	sth_draw_text(stash, 0, 20.0, rtext.x, rtext.y, text, &rtext.x);
	sth_end_draw(stash);
	//printf("%d \n", rtext.x);
}

void calculateButtonRect(rect* r, char* text, rect* textRect){

	float minx, maxx, miny, maxy, totalwidth;
	sth_dim_text(stash, 0, 20.0, text, &minx, &miny, &maxx, &maxy, &totalwidth  );

	float ascender, descender, lineh;
	sth_vmetrics(stash, 0, 20.0, &ascender, &descender, &lineh );

//	printf("lineh %f \n", lineh);

	textRect->x = 4;
	textRect->y = 4;

	textRect->w = totalwidth+8;//100;//getTextLineWidth(text);
	r->w = textRect->w + 2*textRect->x;

	textRect->h = lineh; //20;//getFongHeight();
	r->h = textRect->h + 2*textRect->y;
}

int doButton(int  id, rect* r, char* text){

	rect textRect;
	calculateButtonRect(r, text,  &textRect);

	int hover = isHover(r);

	if (hover){
		gui->hotitem = id;
		if (guiEv->buttonLeft && (gui->activeitem == 0)){
			gui->activeitem = id;
		}
	}


	drawButton(r, text, textRect, (gui->activeitem == id), hover );

//	printf("%d %d %d \n", guiEv->buttonLeft, gui->hotitem, gui->activeitem );
//	se o mouse ta up mas o item da hot e active entao o cara clicou
	if( !guiEv->buttonLeft && (gui->hotitem == id) && (gui->activeitem == id) ){
		printf("clicou: %s \n", text);
		gui->activeitem = 0;
		return 1;
	}

	return 0;
}

void calculateCheckRect(rect* r, char* text, rect* rt, rect* rc){

	int rcOffset = (int) 0.125*16;
	rc->h = 16 - 2*rcOffset;
	rc->w = rc->h;
	rc->x = 4 + rcOffset;
	rc->y = 4 + rcOffset;

	rt->x = 16 + 2*3;
	rt->y = 3;

	float minx, maxx, miny, maxy, totalwidth;
//	printf("dim text \n");
	sth_dim_text(stash, 0, 20.0, text, &minx, &miny, &maxx, &maxy, &totalwidth  );
//	printf("dim text done \n");

	float ascender, descender, lineh;
	sth_vmetrics(stash, 0, 20.0, &ascender, &descender, &lineh );

	rt->w = totalwidth+8;//100;//getTextLineWidth(text);
	r->w = rt->x + rt->w + 4;

	rt->h = lineh; //20;//getFongHeight();
	r->h = rt->h + 2*rt->y;

//	drawRect(rt, 1, 1);
//	drawRect(r, 1, 1);	
}

void drawBoolFrame( rect* r, point* p, int isHover, int isDown){
	int colorNb =  cBase + (isHover) + (isDown << 1);

	drawRoundedRect( r, p , colorNb, cOutline );

}

void drawCheckButton( rect* r, char* text, rect* rt, rect* rc, int isDown, int hover){

	point p;
	p.x = rc->w/6;
	p.y = rc->h/6;
	rect b;
	b.x = r->x+rt->x;
	b.y = r->y + rt->y;
	b.w = rc->w;
	b.h = rc->h;
	//printf("draw bool frame \n");
	drawBoolFrame(&b, &p, hover, isDown);

	rect rtext;
	rtext.x = r->x + rt->x +20;
	rtext.y = r->y + rt->y ;
	rtext.w = rt->w;
	rtext.h = rt->h;
	//printf("begin draw\n");
	sth_begin_draw(stash);
	glColor4fv(colors[cFont]);
	//printf("draw text \n");
	sth_draw_text(stash, 0, 20.0, rtext.x, rtext.y, text, &rtext.x);
	sth_end_draw(stash);
}

//state eh passado por referencia, caso o botao seja clicado ele fica em 1, caso o contrario em 0
// cada checkbutton precisa do seu proprio state
int doCheckButton(int id, rect  *r, char* text, int * state){

	rect rt, rc;
	calculateCheckRect(r, text, &rt, &rc);

	int hover = isHover(r);

	if (hover){
		gui->hotitem = id;
		if ( (guiEv->buttonLeft) && (gui->activeitem == 0)){
			gui->activeitem = id;
		}
	}

	drawCheckButton( r, text, &rt, &rc, (state) && (*state),  hover);

	if( !guiEv->buttonLeft && (gui->hotitem == id) && (gui->activeitem == id) ){
		printf("clicou: %s \n", text);
		gui->activeitem = 0;
		*state = !*state;
		return 1;
	}

	return 0;

}

//no radiobutton os N botoes precisam do mesmo *state
int doRadioButton(int id, rect* r, char* text, int *state){
	rect rt, rc;
//	printf("do radio button \n");
	calculateCheckRect(r, text, &rt, &rc);
//	printf("calculated rect \n");

	int hover = isHover(r);

	if (hover)
		gui->hotitem = id;

	if ( (guiEv->buttonLeft) && hover){
		//printf("clicou e ta hover no radio box \n");
		if (gui->activeitem == 0){
			gui->activeitem = id;
		//	printf("radio ta active\n");
		}
	}

	//printf("radio %d %d \n", gui->hotitem, gui->activeitem  );
	//printf("draw check button \n");
	drawCheckButton(r, text, &rt, &rc, (state) && (*state == id),  hover);

	if( !guiEv->buttonLeft && (gui->hotitem == id) && (gui->activeitem == id) ){
		printf("clicou: %s \n", text);
		gui->activeitem = 0;
		if (*state == id)
			*state = 0;
		else
			*state = id;
		return 1;
	}
	return 0;
}

void calculateHorizontalSliderRect(rect* r, rect* rs, float f, rect* rc){

	r->w = 100 + 2*3;
	r->h = 12 + 2*3;

	rs->y = 3;
	rs->h = r->h - 2*rs->y;

	rc->y = rs->y;
	rc->h = rs->h;

	rs->x = 0;
	rc->w = rc->h; //circulo quadrado
	rs->w = r->w - 2*rs->x - rc->w;
	rc->x = (int)(f*rs->w);

}

void drawHorizontalSlider(rect* r, rect* rs, float f, rect* rc, int isHover ){

	int sliderHeight = rs->h /3;
	rect rl;
	rl.x = r->x + rs->x;
	rl.y = r->y + rs-> y + sliderHeight;
	rl.w = r->w - 2*rs->x;
	rl.h = sliderHeight;
	point p;
	p.x = sliderHeight/2;
	p.y = sliderHeight/2;
	drawFrame(&rl, p, isHover, 0);

	rl.x = r->x +  + rc->x;
	rl.y = r->y + rc->y;
	rl.w = rc->w;
	rl.h =rc->h;
	p.x = rc->w/2;
	p.y = rc->h/2;
	drawFrame(&rl, p, 0, 0);

}

int doHorizontalSlider(int id, rect* r, float min, float max, float* value){

	//normaliza o valor
	float f = (*value - min) / (max - min);
	if (f < 0.0) f = 0.0;
	else if (f > 1.0) f = 1.0;

	rect rs, rc;

	calculateHorizontalSliderRect(r, &rs, f, &rc);

	int hover = isHover(r);

	if (hover)
		gui->hotitem = id;

	if ( (guiEv->buttonLeft) && hover){

		int xs = r->x + rs.x + rc.w/2;
		int x = guiEv->x - xs;

		if (x < 0) x = 0;
		else if (x > rs.w ) x = rs.w;

		rc.x = x;

		float f = (float) (x) / (float)(rs.w);
		f = f*(max - min);

		if (fabs(f - *value) > (max - min) * 0.01){
			*value = f;
		}
	}
	
	drawHorizontalSlider(r, &rs, f, &rc, hover);
}

void calculateComboRect(rect* r, int numOptions, char* options[], int selected, rect* rt, rect* rd){

	rt->x = 3;
	rt->y = 3;

	float ascender, descender, lineh;
	sth_vmetrics(stash, 0, 20.0, &ascender, &descender, &lineh );

	rt->h = lineh;
	r->h = rt->h + 2*rt->y;

	rd->h = rt->h;
	rd->w = rd->h;
	rd->y = rt->y;

	rt->w = 0;
	for(int i = 0; i <numOptions; i++){
		float minx, maxx, miny, maxy, totalwidth;
	//	printf("text combo: %s \n", options[i]);
		sth_dim_text(stash, 0, 20.0, options[i], &minx, &miny, &maxx, &maxy, &totalwidth  );
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

	sth_begin_draw(stash);
	glColor4fv(colors[cFont]);
	rt->x += r->x;
	rt->y += r->y;
	sth_draw_text(stash, 0, 20.0, rt->x, rt->y, options[selected], &rt->x);
	sth_end_draw(stash);

	ra->x += r->x;
	ra->y += r->y;
	drawDownArrow(ra, ra->h*0.15,  cBase+ (!isHover) + (isDown << 2), cOutline );

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

		sth_begin_draw(stash);
		glColor4fv(colors[cFont]);
		rtext.x = ir.x + rt->x;
		rtext.y = ir.y + rt->y;
		rtext.h = rt->h;
		rtext.w = rt->w;
		sth_draw_text(stash, 0, 20.0, rtext.x, rtext.y, options[i], &rtext.x);
		sth_end_draw(stash);

		ir.y -= ir.h;
	}


}

void drawComboOptions(rect* r, int numOptions, char* options[], rect* ri, rect* rt, int selected, int hovered, int hover, int isDown){

	drawListBox(r, numOptions, options, ri, rt, selected, hovered);

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
		sth_dim_text(stash, 0, 20.0, options[i], &minx, &miny, &maxx, &maxy, &totalwidth  );
		rt->w = ( totalwidth > rt->w ? totalwidth : rt->w);
	}
	ri->w = rt->w + 2*rt->x;
	r->w = ri->w + 2*ri->x;

	rt->h = 20; //lineh
	ri->h = rt->h + rt->y;
	r->h = numOptions*ri->h + 2*ri->y;
}

void calculateComboOptionsRect(rect* r, int numOptions, int options, rect* ri, rect* rit, rect* ropt){

	rect nullrect;
	calculateListRect(&nullrect, numOptions, options, ri, rit );

	ropt->x = r->x;
	ropt->y = r->y - nullrect.h;

	ropt->w = nullrect.w;
	ropt->h = nullrect.h;

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
		calculateComboOptionsRect(r, numOptions, options, &ri, &rit, &ro);
		ro.h += 2;
		int hoveroptions = insideRect(&ro, guiEv->x, guiEv->y);
		if (hoveroptions){
			hovered = numOptions - 1 - (gui->h - guiEv->y - (ro.y+ri.y)) / (ri.h);
		}

		drawComboBox(r, numOptions, options, &rt, &rd, *selected, hover, 0 );
		drawComboOptions(&ro, numOptions, options, &ri, &rit, *selected, hovered, hover, 0 );

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
	sth_begin_draw(stash);
	glColor4fv(colors[cFont]);
	sth_draw_text(stash, 0, 20.0, rt->x, rt->y, text, &rt->x);
	sth_end_draw(stash);
}

int drawTextureView(rect* r, int texID, rect* rt, rect* rz, int mipLevel, float texelScale, float texelOffset, int red, int green, int blue, int alpha)
{
    point p; 
    p.x = rt->x;
    p.y = rt->y;
    drawFrame( r, p, 0, 0 );

//    bindTexture(0, texID);
//    glBindTexture(GL_TEXTURE_2D,  texID);

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
        glTexCoord2f((float) rz->x / (float) rt->w , (float) (rz->y + rz->h) / (float) rt->h);
        glVertex2f(r->x + rt->x, r->y - rt->y + r->h);
        glTexCoord2f((float) (rz->x+rz->w) / (float) rt->w , (float) (rz->y + rz->h) / (float) rt->h);
        glVertex2f(r->x + rt->x + rt->w, r->y - rt->y + r->h);
        glTexCoord2f((float) (rz->x+rz->w) / (float) rt->w , (float) (rz->y) / (float) rt->h);
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
