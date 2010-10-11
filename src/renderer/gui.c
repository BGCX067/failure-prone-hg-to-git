#include "renderer.h"
#include "gui.h"
#include "util/malloc.h"
#include <GL/gl.h>
#include <stdio.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include "../util/stb_truetype.h"

GUI* gui;

#define norm255( i ) ( (float) ( i ) / 255.0f )

enum Color
{
    cBase = 0,
    cBool = 4,
    cOutline = 8,
    cFont = 12,
    cFontBack = 16,
    cTranslucent = 20,
    cNbColors = 24,
};

const static float colors[cNbColors][4] =
{
    // cBase
    { norm255(89), norm255(89), norm255(89), 0.7f },
    { norm255(166), norm255(166), norm255(166), 0.8f },
    { norm255(212), norm255(228), norm255(60), 0.5f },
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

	gui = dlmalloc( sizeof(GUI));
	gui->widgetShader  = initializeShader( WidgetVSSource, WidgetFSSource );
	gui->textureViewShader = initializeShader( WidgetVSSource, TexViewWidgetFSSource  );

	return gui;
}

void beginGUI(){

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
	//TODO info da GUI na struct
	gluOrtho2D( 0, 800, 0, 600);

}

void endGUI(){

	glPopAttrib();
	glMatrixMode( GL_PROJECTION);
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW);
	glPopMatrix();
}

void drawRect(rect r, int fillColor, int borderColor){

	printf("drawrect \n");
	bindShader(gui->widgetShader);

	
	float fill[] = { 0.4, 0.4, 0.8, 0.7 };
	float border[] = {0.8, 0.8, 0.8, 1.0};
	setShaderConstant4f(gui->widgetShader, "fillColor",border );
	setShaderConstant4f(gui->widgetShader, "borderColor", fill );
	setShaderConstant1f(gui->widgetShader, "zones", 0);

	printf("setou constantes\n");
	float x0 = r.x;
	float x1 = r.x + r.w;
	        
	float y0 = r.y;
	float y1 = r.y + r.h;
	printf("calculou xy: %f %f %f %f\n", x0, x1, y0, y1);

	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0, 0);
		printf("1\n");
		glVertex2f( x0, y0);
		printf("2\n");
		glVertex2f( x1, y0);
		printf("3\n");
		glVertex2f( x0, y1);
		printf("4\n");
		glVertex2f( x1, y1);
		printf("5\n");
	glEnd();
	printf("desenhou");
	//bindShader(0);
}

void drawFrame(rect rect, point p, int isHoover, int  isDown, int isFocus){

	//int colorNb =  Base + (isHover) + (isDown << 1);
	printf("drawFrame\n");

	if (p.x + p.y == 0)
        	drawRect( rect , 1, 0);//lColorNb, cOutline);
	//else
	//	drawRoundedRect( rect, corner , lColorNb, cOutline );

}

void drawButton(rect r, char* text, rect* rt, int isDown, int isHoover, int isFocus, int style){

	point p;
	printf("drawButton2\n");
	p.x = r.x; p.y = r.y;
	printf("drawButton3\n");
	drawFrame(r, p, isHoover, isDown, isFocus);
	//drawText();
}

int doButton(rect* r, char* text, int* state, int style){


	rect rt;
	rt.x = 0; rt.y = 0; rt.w = 20; rt.h = 10;
	printf("dawButton\n");
	drawButton(rt, "lol", NULL, 0, 0, 0, 0 );

}


char ttf_buffer[1<<20];
unsigned char temp_bitmap[1024*1024];

char data[96]; // ASCII 32..126 is 95 glyphs
unsigned int ftex;

void initfont(void)
{
   int x = 1<<20;
   printf("buffer size %d \n", x);
   FILE *f = fopen("data/fonts/spaceman.ttf", "rb");
   if (!f)
   	printf("nao achoua fonte \n");
   fread(ttf_buffer, 1, 1<<20, f);
   stbtt_BakeFontBitmap(data, stbtt_GetFontOffsetForIndex(ttf_buffer,0), 32.0, temp_bitmap,1024,1024, 32,96, data); // no guarantee this fits!
   // can free ttf_buffer at this point
   printf("baked font\n");
   glGenTextures(1, &ftex);
   glBindTexture(GL_TEXTURE_2D, ftex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512,512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
   // can free temp_bitmap at this point
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void stbtt_print(float x, float y, char *text)
{
   // assume orthographic projection with units = screen pixels, origin at top left
   glBindTexture(GL_TEXTURE_2D, ftex);
   glBegin(GL_QUADS);
   while (*text) {
      if (*text >= 32 && *text < 128) {
         stbtt_aligned_quad q;
         stbtt_GetBakedQuad(data, 512,512, *text-32, &x,&y,&q,1);//1=opengl,0=old d3d
         glTexCoord2f(q.s0,q.t1); glVertex2f(q.x0,q.y0);
         glTexCoord2f(q.s1,q.t1); glVertex2f(q.x1,q.y0);
         glTexCoord2f(q.s1,q.t0); glVertex2f(q.x1,q.y1);
         glTexCoord2f(q.s0,q.t0); glVertex2f(q.x0,q.y1);
      }
      ++text;
   }
   glEnd();
}

