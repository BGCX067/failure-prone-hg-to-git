#include "renderer.h"
#include "gui.h"
#include <GL/gl.h>
#include <stdio.h>
#include "util/fontstash.h"
#include "../glapp.h"
#define STB_TRUETYPE_IMPLEMENTATION
//#include "../util/stb_truetype.h"

GUI* gui;
event* guiEv;
struct sth_stash* stash;


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
        texel = texel * texelScale + texelOffset;\n\
        gl_FragColor  = texel.x * vec4( texelSwizzling.x == 0, texelSwizzling.y == 0, texelSwizzling.z == 0, texelSwizzling.w == 0 );\n\
        gl_FragColor += texel.y * vec4( texelSwizzling.x == 1, texelSwizzling.y == 1, texelSwizzling.z == 1, texelSwizzling.w == 1 );\n\
        gl_FragColor += texel.z * vec4( texelSwizzling.x == 2, texelSwizzling.y == 2, texelSwizzling.z == 2, texelSwizzling.w == 2 );\n\
        gl_FragColor += texel.w * vec4( texelSwizzling.x == 3, texelSwizzling.y == 3, texelSwizzling.z == 3, texelSwizzling.w == 3 );\n\
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

	

	return gui;
}

void beginGUI(event* e){

	gui->hotitem = 0;

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

void drawRoundedRect(  rect* rect,  point* corner, int fillColorId, int borderColorId ){

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


void drawFrame(rect* rect, point corner, int isHover, int  isDown){

	int colorNb =  cDark + (isHover) + (isDown << 1);

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
	sth_dim_text(stash, 0, 20.0, text, &minx, &miny, &maxx, &maxy, &totalwidth  );

	float ascender, descender, lineh;
	sth_vmetrics(stash, 0, 20.0, &ascender, &descender, &lineh );

	rt->w = totalwidth+8;//100;//getTextLineWidth(text);
	r->w = rt->x + rt->w + 4;

	rt->h = lineh; //20;//getFongHeight();
	r->h = rt->h + 2*rt->y;

//	drawRect(rt, 1, 1);
//	drawRect(r, 1, 1);	
}

void drawBoolFrame(rect* r, point* p, int isHover, int isDown){
	int colorNb =  cBase + (isHover) + (isDown << 1);

	drawRoundedRect( r, p , colorNb, cOutline );

}

void drawCheckButton(rect* r, char* text, rect* rt, rect* rc, int isDown, int hover){

	point p;
	p.x = rc->w/6;
	p.y = rc->h/6;
	rect b;
	b.x = r->x+rt->x;
	b.y = r->y + rt->y;
	b.w = rc->w;
	b.h = rc->h;
	drawBoolFrame(&b, &p, hover, isDown);

	rect rtext;
	rtext.x = r->x + rt->x +20;
	rtext.y = r->y + rt->y ;
	rtext.w = rt->w;
	rtext.h = rt->h;
	sth_begin_draw(stash);
	glColor4fv(colors[cFont]);
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

	drawCheckButton(r, text, &rt, &rc, (state) && (*state),  hover);

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
	calculateCheckRect(r, text, &rt, &rc);

	int hover = isHover(r);

	if (hover)
		gui->hotitem = id;

	if ( (guiEv->buttonLeft) && hover){
		if (gui->activeitem == 0)
			gui->activeitem = id;
	}
	
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
	r->w += rt->w + rt->x;
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

		if (i == hovered){
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

int doComboBox(int id, rect* r, int numOptions, char* options[], int* selected){

	rect rt, rd;
	calculateComboRect(r, numOptions, options, *selected, &rt, &rd);

	int hover = isHover(r);

	if (hover)
		gui->hotitem = id;

	if( (guiEv->buttonLeft) && hover){
		gui->activeitem = id;
		rect ro, ri, rit;//ele retorna em ro e nao em r, apenas nesse caso
		calculateComboOptionsRect(r, numOptions, options, &ri, &rit, &ro);
		ro.h += 2;
		int hovered = -1;
		int hoveroptions = insideRect(&ro, guiEv->x, guiEv->y);
		if (hoveroptions){
			hovered = numOptions - 1 - (guiEv->y - (ro.y+ri.y)) / (ri.h);
		}

		drawComboBox(r, numOptions, options, &rt, &rd, *selected, hover, 0 );
		drawComboOptions(&ro, numOptions, options, &ri, &rit, *selected, hovered, hover, 0 );



		if (!gui->twophase){
			gui->twophase = 1;
			gui->focusx = guiEv->x;
			gui->focusy = guiEv->y;
		}else{
			printf("hover %d hoveroptions %d \n", hover, hoveroptions);
			if ((hoveroptions == 0) || (hover == 0) )  {
				printf("SAIU DO HOVER \n");
				gui->twophase = 0;
				gui->activeitem = 0;
			}
			else if ( (hoveroptions || hover) &&  !guiEv->buttonLeft  ){
				gui->twophase = 0;
				gui->activeitem = 0;
			}

			if (hoveroptions && guiEv->buttonLeft){
			}

		}


	}else{
		drawComboBox(r, numOptions, options, &rt, &rd, *selected, hover, 0 );
	}
	
}
