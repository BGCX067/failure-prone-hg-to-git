
#include <stdio.h>
#include <GL/gl.h>
#include "m3.h"

typedef struct ref_{
	unsigned int nEntries;
	unsigned int ref;
} reference;

typedef struct m3header_{

	char id[4];
	unsigned int refOffset;
	unsigned int nrefs;
	reference modl;

} md33;

typedef struct m3tag_{

	char id[4];
	unsigned int offset;
	unsigned int nEntries;
	unsigned int type;

}referenceEntry;


typedef struct modlheader_{

	reference name;
	unsigned int version;
	reference unk1;
	reference unk2;
	reference unk3;
	unsigned int d2;
	unsigned int d3;
	unsigned int d4;
	reference sts;
	reference bones;
	unsigned int d5;

	unsigned int flags;
	reference vertexData;
	reference views;

} modl23;

typedef struct vertex{

	float position[3];
	unsigned char boneweights[4];
	unsigned char boneindices[4];
	unsigned char normal[4];
	unsigned short uv[2];
	unsigned short unk;
	unsigned char tangent[4];

}vertex;

typedef struct div_{

	reference faces;
	reference regions;
	reference bat;
	reference msec;

	unsigned int unknow;

}div;

typedef struct _region{
	
	unsigned int unknow;
	unsigned short verticeOffset;
	unsigned short nVertices;
	unsigned int indicesOffset;
	unsigned int nIndices;
	unsigned char unk[12];

}region;

batch * loadm3(char* filename){

	int i, j;
	unsigned int modlOffset;

	FILE* f = fopen(filename, "rb");
	if (!f){
		printf("Cant read file. \n");
		return 0;
	}

	md33 header;
	fread(&header, sizeof(md33), 1, f);
	printf("HEADER.id: %c%c%c%c header.nrefs %d header.modl.ref %d model.refOffset %d \n", header.id[0], header.id[1], header.id[2], header.id[3],  header.nrefs, header.modl.ref, header.refOffset);

	referenceEntry *refs = malloc(sizeof(referenceEntry)*header.nrefs);
	fseek(f, header.refOffset, SEEK_SET);
	fread(refs, sizeof(referenceEntry), header.nrefs, f);

//	for(i = 0; i < header.nrefs; i++)
//		printf("%c%c%c%c \n", refs[i].id[0],refs[i].id[1], refs[i].id[2],  refs[i].id[3] );

	
	modl23 model;
	vertex* verts = NULL;
	div* views = NULL;
	region* regions = NULL;
	unsigned short* faces = NULL;

	int nVertices = 0;
	int nFaces = 0;

	printf("modl offset %d \n",  refs[header.modl.ref].offset);
	fseek(f, refs[header.modl.ref].offset, SEEK_SET);
	fread(&model, sizeof(modl23), 1,  f);

	if( (model.flags & 0x40000) != 0)
		printf("vertex size 36 bytes \n");
	else
		printf("vertex size 32 bytes \n");


	nVertices = model.vertexData.nEntries/sizeof(vertex);
	printf("nverts %d \n", nVertices);

	printf("vertexData.ref %d vertexOffset %d nVertices: %d \n ", model.vertexData.ref, refs[model.vertexData.ref].offset, refs[model.vertexData.ref].nEntries  );
	printf("Tag dos vertices: %c%c%c%c\n", refs[model.vertexData.ref].id[0], refs[model.vertexData.ref].id[1], refs[model.vertexData.ref].id[2], refs[model.vertexData.ref].id[3]);

	fseek(f, refs[model.vertexData.ref].offset, SEEK_SET);
	verts = malloc(sizeof(vertex)*nVertices);
	fread(verts, sizeof(vertex), nVertices, f);

	//for(i = 0; i < nVertices; i++){
	//	printf("%f %f %f \n", verts[i].position[0], verts[i].position[1], verts[i].position[2]);
	//}

	printf("Numero de views: %d \n", model.views.nEntries);
	views = malloc(sizeof(div)*1); //sempre so 1?
	fseek(f, refs[model.views.ref].offset, SEEK_SET);
	fread(views, sizeof(div), 1, f );

	printf("Numero de regions: %d \n", views->regions.nEntries);
	regions =  malloc(sizeof(region)*views->regions.nEntries);
	fseek(f, refs[views->regions.ref].offset, SEEK_SET);
	fread(regions, sizeof(region), views->regions.nEntries, f);

	nFaces = views->faces.nEntries;
	printf("Numero de faces: %d \n", nFaces);
	faces = malloc(sizeof(unsigned short)*nFaces);
	fseek(f, refs[views->faces.ref].offset, SEEK_SET);
	fread(faces, sizeof(unsigned short), nFaces, f);

	//for(i = 0; i < nFaces; i++)
	//	printf("%d  \n", faces[i]);


	batch* b = initializeBatch(b);

	begin(b, GL_TRIANGLES, nFaces, 1);
	for(i = 0; i < views->regions.nEntries; i++){
//		begin(b, GL_TRIANGLES, nFaces, 1);
		for(j = regions[i].indicesOffset; j<(regions[i].indicesOffset+regions[i].nIndices); j++ ){
			texCoord2f(b, 0, (float)verts[faces[j]].uv[0]/2048.0, (float)verts[faces[j]].uv[1]/2048.0);
			normal3f(b,  (float)2*verts[faces[j]].normal[0]/255.0-1, (float)2*verts[faces[j]].normal[1]/255.0 - 1, (float)2*verts[faces[j]].normal[2]/255.0 - 1);
//			printf("%f, %f, %f\n",  (float)2*verts[faces[j]].normal[0]/255.0-1, (float)2*verts[faces[j]].normal[1]/255.0 - 1, (float)2*verts[faces[j]].normal[2]/255.0 - 1);
//			printf("%f %f \n ", (float)verts[faces[j]].uv[0]/2048, (float)verts[faces[j]].uv[1]/2048);
			vertex3f(b, verts[faces[j]].position[0], verts[faces[j]].position[1], verts[faces[j]].position[2]);
		}
//		end(b);
	}
	end(b);

	return b;
}
