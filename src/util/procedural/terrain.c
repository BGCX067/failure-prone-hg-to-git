#include <stdlib.h>
#include <string.h>
#include "terrain.h"
#include "../image.h"
#include "../../math/vec3.h"
#include "../../renderer/mesh.h"

#include <GL/gl.h>

int sizex, sizey;
float* heights;
float* normals;
float* vertices;

mesh* m;
triangles* tri;

void initializeTerrain(char* filename, float scale){

	int n;
	unsigned char* data = stbi_load(filename, &sizex, &sizey, &n, 0);

	heights = malloc(sizeof(float)*sizex*sizey);

	for(int i = 0; i < sizex*sizey; i++)
		heights[i] = data[i*3]*scale;

	normals = malloc(sizeof(float)*sizex*sizey*3);

	for (int z = 1; z < sizey - 1; z++) {
		for(int x = 1; x < sizex - 1; x++) {

			float dyx =  heights[x+1 + z*sizex]-heights[x-1 + z*sizex];
			float dyz =  heights[x + (z+1)*sizex]-heights[x + (z-1)*sizex];

			vec3 vx;
			vx[0] = 1.0;
			vx[1] = dyx;
			vx[2] = 0.0;
			vec3 vz;
			vz[0] = 0.0f;
			vz[1] = -dyz;
			vz[2] = -1.0;

			vec3 v;
			cross(vx, vx, v);
			vecNormalize(v);	

			normals[3*(x + z*sizex)    ] = v[0];
			normals[3*(x + z*sizex) + 1] = v[1];
			normals[3*(x + z*sizex) + 2] = v[2];
		}
	}

	stbi_image_free(data);

	tri = malloc(sizeof(triangles));
	memset(tri, sizeof(triangles), 0);
	tri->numTexSets = 0;
	tri->verticesCount = sizex*sizey;
	tri->verticesComponents = 3;
	tri->indicesCount = sizex*sizey;
	tri->indices = malloc(sizeof(unsigned int)*sizex*sizey);
	tri->vertices =  malloc(sizeof(float)*sizex*sizey*3);

	tri->tangents = NULL;

	for(int i = 0; i < sizex*sizey; i++)
		tri->indices[i] = i;

	int c = 0;
	for(int i = 0; i < sizex; i++){
		for(int j = 0; j < sizey; j++){
			tri->vertices[c] = i;
			tri->vertices[c+1] = j;
			tri->vertices[c+2] = heights[i + j*sizex];
		}
		c += 3;
	}
	
	m = malloc(sizeof(mesh));
	m->trianglesCount = 1;
	m->tris = fplist_init(NULL);
	fplist_insback( tri, m->tris);

	printf("sizex: %d sizey: %d \n", sizex, sizey);

	createVBO(m);	
}

void drawTest(){

//	glVertexPointer(3, GL_FLOAT, 0, tri->vertices);
//	glDrawArrays(GL_POINTS, 0, tri->indicesCount);

/*	glBegin(GL_POINTS);

		for(int i = 0; i < tri->verticesCount; i += 3){
			glVertex3f(tri->vertices[i], tri->vertices[i+1], tri->vertices[i+2]);

		}

	glEnd();*/

	glPointSize(8.0);
//	triangles* ttri = m->tris->first->data;
//	drawVAO(ttri->vaoId, ttri->indicesCount, GL_POINTS);
	
//	glDrawArrays(GL_TRIANGLE_STRIP, 0, sizex*sizey);

	for(int z = 1; z < sizey-2; z += 8){

		glBegin(GL_TRIANGLE_STRIP);

		for(int x=1; x < sizex - 1; x+=8){
			float fx = (float)x;
			float fz = (float)z;
			glVertex3f( fx, heights[x + z*sizex], fz );
			glVertex3f( fx, heights[x + (z+8)*sizex], fz+8.0f );
		}
		glEnd();
	}

}
