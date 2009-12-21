
#define MAX_TEXCOORDS 8

typedef struct _texcoord{

	unsigned int count;
	unsigned int set;
	unsigned short int components;
	float* texCoords;

}texCoord;

typedef struct _triangles{

	unsigned int indicesCount, verticesCount, normalsCount, tangentsCount, binormalsCount;
	unsigned int vboId, indicesId;
	unsigned int vertexFormatId;
	unsigned int totalAttrs;

	unsigned int* indices; //pode ser short
	float* vertices;
	float* normals;
	float* tangents;
	float* binormals;

	texCoord* texCoords[MAX_TEXCOORDS];
	unsigned int numTexSets;

}triangles;

typedef struct _mesh{

	int trianglesCount;
	triangles* tris;

}mesh;

typedef struct _scene{
	int nodeCount;
	int textureCount;
	int meshCount;

	mesh* meshes;
}scene;

void initializeTriangles(triangles* tri);

scene* initializeDae(char* filename);

