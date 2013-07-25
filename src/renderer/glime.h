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

typedef struct _batch{
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
}Batch;

Batch* InitializeBatch();
void DestroyBatch(Batch *b);

void Begin(Batch* b, int primitive, int nverts, int texSets);
void End(Batch* b);
void Draw(Batch* b);

void Vertex3f(Batch* b, float x, float y, float z);
void Vertex2f(Batch* b, float x, float y);
void Normal3f(Batch* b, float x, float y, float z);
void Color4f(Batch* b,float x, float y, float z, float w);
void TexCoord2f(Batch* b, unsigned int texUnit, float s, float t);

//funcoes de desenho
Batch* MakeCube(float radius);

#endif
