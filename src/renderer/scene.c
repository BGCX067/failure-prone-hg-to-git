#include "scene.h"
#include "../util/ezxml.h"
#include "../util/malloc.h"
#include <stdio.h>
#include <string.h>
ezxml_t getVertexSource(char* sourceName, ezxml_t mesh){

	ezxml_t source = ezxml_child(mesh, "vertices");
	for (source = ezxml_child(mesh, "vertices"); source; source = source->next){
		char* name = ezxml_attr(source, "id");
		if (strcmp(sourceName, name) == 0)
			return source;
	}

	return  NULL;

}

ezxml_t getSource(char* sourceName, ezxml_t mesh){

	ezxml_t source = ezxml_child(mesh, "source");
	for (source = ezxml_child(mesh, "source"); source; source = source->next){
		char* name = ezxml_attr(source, "id");
		if (strcmp(sourceName, name) == 0)
			return source;
	}

	return  NULL;

}
void getFloatArray(float** array, char* values, int count){
	*array  = dlmalloc(sizeof(float)*count);
	char* tok  = strtok(values, " ");
	int k = 0;
	while(tok != NULL){
		(*array)[k] = atof(tok);
		tok = strtok(NULL, " ");
		k++;
	}
}

scene* initializeDae(char* filename){

	ezxml_t dae = ezxml_parse_file(filename);
	if (!dae){
		printf("File not found: %s\n", filename);
		return NULL;
	}

	scene* s = initializeScene();//dlmalloc(sizeof(scene));
	//s->meshCount  = 0;
	//s->textureCount = 0;

	ezxml_t library_images = ezxml_child(dae, "library_images");
	if (library_images){
		ezxml_t image;
		for (  image = ezxml_child(library_images, "image"); image; image = image->next){
			printf("id: %s \n", ezxml_attr(image, "id"));
		}
	}else
		printf("library_images not found.\n");

	ezxml_t  library_geometries = ezxml_child(dae, "library_geometries");
	if (!library_geometries){
		printf("library_geometries not found.");
		return 0;
	}else{
		ezxml_t geometry;
		for ( geometry = ezxml_child(library_geometries, "geometry"); geometry;  geometry = geometry->next){
			ezxml_t mesh;
			for(mesh = ezxml_child(geometry, "mesh"); mesh; mesh = mesh->next){
				s->meshCount++;	
			}
		}
		
		//s->meshes = dlmalloc(sizeof(mesh)*s->meshCount);
		//  s->meshes = dlmalloc(sizeof(mesh*)*MAX_MESHES_PER_SCENE);
		//  for (int j = 0; j < s->meshCount; j++)
		//  	s->meshes[j] = dlmalloc(sizeof(mesh));

		for(geometry = ezxml_child(library_geometries, "geometry"); geometry; geometry = geometry->next){
			ezxml_t meshxml;
			int i = 0;
			for(meshxml = ezxml_child(geometry, "mesh"); meshxml; meshxml = meshxml->next){
				mesh* m = dlmalloc(sizeof(mesh));
				ezxml_t trixml;
				m->trianglesCount = 0;
				for( trixml = ezxml_child(meshxml, "triangles");  trixml; trixml = trixml->next){
					m->trianglesCount++;
				}
				//TODO passar  destrutor?
				printf("inicializando lista\n");
				m->tris = fplist_init(NULL, NULL); //dlmalloc(sizeof(triangles)*s->meshes[i]->trianglesCount);
				int triCount = 0;
				triangles* tri;
				for (trixml = ezxml_child(meshxml, "triangles"); trixml; trixml = trixml->next){
					printf("lendo triangulo \n");
					triangles* tri = dlmalloc(sizeof(triangles));
					//TODO memset?
					printf("initializando triangulo \n");
					initializeTriangles( tri );
					printf("triangulo inicializado \n");
					int count = atoi( ezxml_attr(trixml, "count"))*3;
					printf("vai receber count: %d \n", count);
					tri->indicesCount  = count;
					printf("recebeu count \n");
					//TODO pode ser short int
					tri->indices = dlmalloc( sizeof(unsigned int)*count );
					printf("alocou  indices\n");
					ezxml_t p = ezxml_child(trixml, "p");
					char* tok  = strtok(p->txt, " ");
					int k = 0;
					while(tok != NULL){
						tri->indices[k] = atoi(tok);
						tok = strtok(NULL, " ");
						k++;
					}
					printf("leu os indices\n");
					
					ezxml_t input = ezxml_child(trixml, "input");
					int texSetIndices = -1;

					while(input){
						char* semantic = ezxml_attr(input, "semantic");
						char* sourceName = ezxml_attr(input, "source");
						printf("sourceName: %s semantic: %s  \n", sourceName, semantic);
						if ( sourceName[0] = '#'){
							for(int k = 1; k <strlen(sourceName);k++)
								sourceName[k-1] = sourceName[k];
							sourceName[strlen(sourceName)-1] = '\0';
						}
						printf("sourceNAme: %s  semantic: %s  \n", sourceName, semantic);
						if (strcmp(semantic, "VERTEX") == 0){
							printf(" Vertex \n");
							ezxml_t source = getVertexSource(sourceName, meshxml);
							if (source){
								printf("source \n");
								ezxml_t floatArray = ezxml_child(source, "float_array");
								if (floatArray){
									printf("floatArray\n");
									tri->verticesCount = atoi(ezxml_attr(floatArray, "count"));
									getFloatArray( tri->vertices, floatArray->txt, atoi(ezxml_attr(floatArray, "count")) );
								}else{
									ezxml_t vertexInput;
									printf("vertexInput\n");
									for(vertexInput = ezxml_child(source, "input"); vertexInput; vertexInput = vertexInput->next){
										char* semantic = ezxml_attr(vertexInput, "semantic");
										if (strcmp(semantic, "POSITION" ) == 0){
											sourceName = ezxml_attr(vertexInput, "source");
											if ( sourceName[0] = '#'){
												for(int k = 1; k <strlen(sourceName);k++)
													sourceName[k-1] = sourceName[k];
												sourceName[strlen(sourceName)-1] = '\0';
											}
											ezxml_t source = getSource(sourceName, meshxml);
											if (source){
												ezxml_t float_array = ezxml_child(source, "float_array");
												if (float_array){
													tri->verticesCount = atoi(ezxml_attr(float_array, "count"));
													getFloatArray(&(tri->vertices), float_array->txt, atoi(ezxml_attr(float_array, "count")));
												}
											}
										}
									}
								}
							}else
								printf("nao achou o source %s \n", sourceName);
						}
						else if ( strcmp(semantic,  "NORMAL") == 0){
							ezxml_t source = getSource(sourceName, meshxml);
							if (source){
								ezxml_t float_array = ezxml_child(source,  "float_array");
								if (float_array){
									tri->normalsCount = atoi(ezxml_attr(float_array, "count"));
									getFloatArray(&(tri->normals), float_array->txt, tri->normalsCount);
								}
							}
						}
						else if ( strcmp(semantic,  "TANGENT") == 0){
							ezxml_t source = getSource(sourceName, meshxml);
							if (source){
								ezxml_t float_array = ezxml_child(source,  "float_array");
								if (float_array){
									tri->tangentsCount = atoi(ezxml_attr(float_array, "count"));
									getFloatArray(&(tri->tangents), float_array->txt, tri->tangentsCount);
								}
							}
						}
						else if ( strcmp(semantic,  "BINORMAL") == 0){
							ezxml_t source = getSource(sourceName, meshxml);
							if (source){
								ezxml_t float_array = ezxml_child(source,  "float_array");
								if (float_array){
									tri->binormalsCount = atoi(ezxml_attr(float_array, "count"));
									getFloatArray(&(tri->binormals), float_array->txt, tri->binormalsCount);
								}
							}
						}
						else if ( strcmp(semantic, "TEXCOORD")  == 0){
							printf("lendo texcoords\n");
							texCoord* texSet = dlmalloc(sizeof(texCoord));
							texSet->set = atoi( ezxml_attr(input, "set"));
							printf("texcoord set: %d\n", texSet->set);
							ezxml_t source = getSource(sourceName, meshxml);
							if (source){
								printf("achou o source\n");
								ezxml_t float_array = ezxml_child(source, "float_array");
								if (float_array){
									printf("achou o float_array\n");
									texSet->count = atoi(ezxml_attr(float_array, "count"));
									printf("count :%d\n", texSet->count);
									printf("texcoords: %s\n", float_array->txt);
									getFloatArray( &(texSet->texCoords), float_array->txt, texSet->count);
									printf("float array lida\n");
								}
								ezxml_t technique_common = ezxml_child(source, "technique_common");
								if (technique_common){
									printf("lendo technique_common\n");
									ezxml_t accessor = ezxml_child(technique_common, "accessor");
									if (accessor){
										printf("tem accessor\n");
										texSet->components = atoi( ezxml_attr(accessor, "stride"));
										printf("accessor: %d\n", atoi( ezxml_attr(accessor,"stride" )));
									}
									else{
										printf("nao tem accessor\n");
										texSet->components = 2;
									}
								}else
									texSet->components = 2;
								
							}
							printf("lido components:%d \n", texSet->components);
							tri->numTexSets++;
							texSetIndices++;
							tri->texCoords[texSetIndices] = texSet;
							tri->totalAttrs++;
						}
						input = input->next;

					}
					fplist_insback(tri, m->tris);
					triCount++;
				}
				i++;
				fplist_insback(m, s->meshes);
			}
		}

	}


	ezxml_free(dae);
	printf("scena lida.\n");
	return s;
}

void initializeTriangles(triangles* tri){

	memset(tri, 0, sizeof(triangles));

}

int addMesh(scene* s, mesh *m){
	
	return fplist_insback(s->meshes, m );
}

scene* initializeScene(){

	scene * s = dlmalloc(sizeof(scene));
	memset(s, sizeof(scene),  0);
	//TODO deixa null aqui?
	s->meshes = fplist_init(NULL,  NULL);
	s->nodes = fplist_init(NULL, NULL);
	return s;
}
