#ifndef _GLIME_H_
#define _GLIME_H_

/* glime.h fornece um 'imediate mode emulator' para o opengl
 * para utilizar deve-se fazer:
 * 1- criar o batch e inicializa-lo com initializeBatch()
 * 2- chamar begin(), passando a primitive e numero de vertices
 * 3- chamar as funcoes vertex3f,  normal3f ou texcoord2f. Sempre deve-se chamar a vertex3f antes das outras.
 * 4- quando terminar os vertices, chama end();
 * 5-  para desenhar o batch basta chamar draw(). 
 * Os passos 1-4 ficam em alguma parte de inicializaçao do codigo, depois de pronto basta usar o passo 5 sempre
 */

typedef struct  _batch{

	int  primitive;
	
	unsigned int vertexVBO;
	unsigned int normalVBO;
	unsigned int colorVBO;
	unsigned int *texCoordsVBO;
	
	unsigned int vaoid;
	
	unsigned int numTexSets;
	unsigned int numVerts;
	unsigned int verticesCount;
	
	float* vertices;
	float* normals;
	float* colors;
	float** texCoords;

	int verticesComponents; //se eh xy ou xyz

}batch;

batch* initializeBatch();

void begin(batch* b, int primitive, int nverts, int texSets);
void end(batch* b);
void draw(batch* b);

void vertex3f(batch* b, float x, float y, float z);
void vertex2f(batch* b, float x, float y);
void normal3f(batch* b, float x, float y, float z);
void color4f(batch* b,float x, float y, float z, float w);
void texCoord2f(batch* b, unsigned int texUnit, float s, float t);

//funcoes de desenho
batch* makeCube(float radius);

void destroyBatch(batch *b);

#endif
