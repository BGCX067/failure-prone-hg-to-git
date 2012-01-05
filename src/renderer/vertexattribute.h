#ifndef VERTEXATTRIBUTE_H_
#define VERTEXATTRIBUTE_H_

#define MAX_VERTEX_ATTRS 16
enum AttributeType{
	ATTR_VERTEX = 0,
	ATTR_NORMAL = 2,
	ATTR_COLOR = 3,
	ATTR_SECONDARY_COLOR = 4,
	ATTR_FOG_COORD = 5,

	ATTR_TANGENT = 6,
	ATTR_BINORMAL = 7,

	ATTR_TEXCOORD0 = 8,
	ATTR_TEXCOORD1 = 9,
	ATTR_TEXCOORD2 = 10,
	ATTR_TEXCOORD3 = 11,
	ATTR_TEXCOORD4 = 12,
	ATTR_TEXCOORD5 = 13,
	ATTR_TEXCOORD6 = 14,
	ATTR_TEXCOORD7 = 15

};

typedef struct _vertexAttribute{
	unsigned int count;
	unsigned int size;
	unsigned int offset;
	unsigned int components;
	int type;
	unsigned int vboID; //opcional
} VertexAttribute;

#endif
