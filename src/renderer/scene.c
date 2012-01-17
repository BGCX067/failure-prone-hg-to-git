#include "scene.h"
#include "../util/ezxml.h"
#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
//#include "renderer.h"

/*ezxml_t getVertexSource(char* sourceName, ezxml_t mesh){

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
	*array  = malloc(sizeof(float)*count);
	char* tok  = strtok(values, " ");
	int k = 0;
	while(tok != NULL){
		(*array)[k] = atof(tok);
		tok = strtok(NULL, " ");
		k++;
	}
}


Scene* initializeDae(char* filename){

	ezxml_t dae = ezxml_parse_file(filename);
	if (!dae){
		printf("File not found: %s\n", filename);
		return NULL;
	}

	Scene* s = initializeScene();//malloc(sizeof(scene));
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
		printf("library_geometries not found.\n");
		return 0;
	}else{
		ezxml_t geometry;
		for ( geometry = ezxml_child(library_geometries, "geometry"); geometry;  geometry = geometry->next){
			//ezxml_t mesh;
			//for(mesh = ezxml_child(geometry, "mesh"); mesh; mesh = mesh->next){
			//	s->meshCount++;	
			//}
			s->meshCount++;	
		}
		
		for(geometry = ezxml_child(library_geometries, "geometry"); geometry; geometry = geometry->next){
			ezxml_t meshxml;
			int i = 0;
            meshxml = ezxml_child(geometry, "mesh");
            if(!meshxml) {
                printf("geometry without mesh tag, skipping.\n");
                continue;
            }

            //for(meshxml = ezxml_child(geometry, "mesh"); meshxml; meshxml = meshxml->next){
            Mesh* m = malloc(sizeof(Mesh));
            ezxml_t trixml;
            m->trianglesCount = 0;
            for( trixml = ezxml_child(meshxml, "triangles");  trixml; trixml = trixml->next){
                m->trianglesCount++;
            }
            //TODO passar  destrutor?
            printf("m->trianglesCount: %d\n", m->trianglesCount);
            //m->tris = fplist_init(NULL); //malloc(sizeof(triangles)*s->meshes[i]->trianglesCount);
            m->tris = fplist_init(free);
            int triCount = 0;
            //triangles* tri;
            for (trixml = ezxml_child(meshxml, "triangles"); trixml; trixml = trixml->next){
                Triangles* tri = malloc(sizeof(Triangles));
                //TODO memset?
                initializeTriangles( tri );
                int count = atoi( ezxml_attr(trixml, "count"))*3;
                tri->indicesCount  = count;
                //TODO setando material default aqui
                tri->mat.shininess = 4;
                tri->mat.ks[0] = 0.3;
                tri->mat.ks[1] = 0.3;
                tri->mat.ks[2] = 0.3;
                tri->mat.ks[3] = 1.0;
                tri->mat.ka[0] = 0.1;
                tri->mat.ka[1] = 0.1;
                tri->mat.ka[2] = 0.1;
                tri->mat.ka[3] = 1.0;
                tri->mat.kd[0] = 0.6;
                tri->mat.kd[1] = 0.6;
                tri->mat.kd[2] = 0.6;
                tri->mat.kd[3] = 1.0;
                //tri->mat.shaderid = initializeShader( readTextFile("data/shaders/phong.vert"), readTextFile("data/shaders/phong.frag") );

                //char* material = ezxml_attr(trixml, "material");
                //printf("material: %s \n", material);
                //TODO pode ser short int
                tri->indices = malloc( sizeof(unsigned int)*count );
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
                    //printf("sourceName: %s semantic: %s  \n", sourceName, semantic);
                    if ( sourceName[0] = '#'){
                        for(int k = 1; k <strlen(sourceName);k++)
                            sourceName[k-1] = sourceName[k];
                        sourceName[strlen(sourceName)-1] = '\0';
                    }
                    printf("sourceNAme: %s  semantic: %s  \n", sourceName, semantic);
                    if (strcmp(semantic, "VERTEX") == 0){
                        printf("\tVertex \n");
                        ezxml_t source = getVertexSource(sourceName, meshxml);
                        if (source){
                            printf("\t\tsource \n");
                            ezxml_t floatArray = ezxml_child(source, "float_array");
                            if (floatArray){
                                printf("\t\t\tfloatArray\n");
                                tri->verticesCount = atoi(ezxml_attr(floatArray, "count"));
                                getFloatArray( tri->vertices, floatArray->txt, atoi(ezxml_attr(floatArray, "count")) );
                            }else{
                                ezxml_t vertexInput;
                                printf("\t\t\tvertexInput\n");
                                for(vertexInput = ezxml_child(source, "input"); vertexInput; vertexInput = vertexInput->next){
                                    char* semantic = ezxml_attr(vertexInput, "semantic");
                                    printf("\t\t\t\tsemantic: %s\n", semantic);
                                    if (strcmp(semantic, "POSITION" ) == 0){
                                        sourceName = ezxml_attr(vertexInput, "source");
                                        printf("\t\t\t\t\tsourcename: %s\n", sourceName);
                                        if ( sourceName[0] = '#'){
                                            for(int k = 1; k <strlen(sourceName);k++)
                                                sourceName[k-1] = sourceName[k];
                                            sourceName[strlen(sourceName)-1] = '\0';
                                        }
                                        ezxml_t source = getSource(sourceName, meshxml);
                                        if (source){
                                            ezxml_t float_array = ezxml_child(source, "float_array");
                                            if (float_array){
                                                printf("\t\t\t\t\tfloat array pro source la\n");
                                                tri->verticesCount = atoi(ezxml_attr(float_array, "count"));
                                                printf("\t\t\t\t\ttri->verticesCount: %d\n", tri->verticesCount);
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
                        printf("\tNormal\n");
                        ezxml_t source = getSource(sourceName, meshxml);
                        printf("\tsourcename: %s\n", sourceName);
                        if (source){
                            ezxml_t float_array = ezxml_child(source,  "float_array");
                            if (float_array){
                                tri->normalsCount = atoi(ezxml_attr(float_array, "count"));
                                printf("\t\ttri->normalsCount: %d\n", tri->normalsCount);
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
                        TexCoord* texSet = malloc(sizeof(TexCoord));
                        texSet->set = atoi( ezxml_attr(input, "set"));
                        printf("texcoord set: %d\n", texSet->set);
                        ezxml_t source = getSource(sourceName, meshxml);
                        if (source){
                            printf("\tachou o source\n");
                            ezxml_t float_array = ezxml_child(source, "float_array");
                            if (float_array){
                                printf("\t\tachou o float_array\n");
                                texSet->count = atoi(ezxml_attr(float_array, "count"));
                                printf("\t\tcount :%d\n", texSet->count);
                                //printf("texcoords: %s\n", float_array->txt);
                                getFloatArray( &(texSet->texCoords), float_array->txt, texSet->count);
                                printf("\t\tfloat array lida\n");
                            }
                            ezxml_t technique_common = ezxml_child(source, "technique_common");
                            if (technique_common){
                                //printf("lendo technique_common\n");
                                ezxml_t accessor = ezxml_child(technique_common, "accessor");
                                if (accessor){
                                    //	printf("tem accessor\n");
                                    texSet->components = atoi( ezxml_attr(accessor, "stride"));
                                    //	printf("accessor: %d\n", atoi( ezxml_attr(accessor,"stride" )));
                                }
                                else{
                                    //	printf("nao tem accessor\n");
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
                printf("\t\tinserindo tri em m->tris\n");
                fplist_insback(tri, m->tris);
                triCount++;
            }
            printf("\t\tinserindo mesh em s->meshes\n");
            i++;
            fplist_insback(m, s->meshes);
            //}
		}

	}

	ezxml_free(dae);
	printf("scena lida.\n");
	return s;
} */

void initializeTriangles(Triangles* tri){

	memset(tri, 0, sizeof(Triangles));
	tri->verticesComponents = 3;

}

int addMesh(Scene* s, Mesh *m){
	return fplist_insback(m, s->meshList );
}

int addLight(Scene* s, Light* l){
	return fplist_insback(l, s->lightList);
}

int addMaterial(Scene *s, Material *m) {
    return fplist_insback(m, s->materialList);
}

int addTexture(Scene *s, Texture *t) {
    return fplist_insback(t, s->texList);
}

int addNode(Scene* s, Node* n){
	return fplist_insback(n, s->nodes);
}

Scene* initializeScene(){
	Scene * s = malloc(sizeof(Scene));
	memset(s, sizeof(Scene),  0);
	
    s->meshList = fplist_init(free);
	s->lightList = fplist_init(free);
    s->materialList = fplist_init(free);
    s->texList = fplist_init(free);
        
    s->nodes = fplist_init(free);
	return s;
}

int setupScene(Scene* s){
	if (s == NULL)
		return 0;
    s->b.pmin[0] = 99999999;
    s->b.pmin[1] = 99999999;
    s->b.pmin[2] = 99999999;

    s->b.pmax[0] = -999999999;
    s->b.pmax[1] = -999999999;
    s->b.pmax[2] = -999999999;

	if (s->meshList){
		Mesh* m = NULL;
		for( int i = 0; i < s->meshList->size; i++){ // para da mesh da cena
			m = fplist_getdata(i, s->meshList);
			createVBO(m);
            //setmeshboundingbox(m);
            Triangles *t = fplist_getdata(i, m->tris);
            setboundingbox(&(m->b), t->vertices, t->verticesCount/3);

            if(m->b.pmin[0] < s->b.pmin[0])
                s->b.pmin[0] = m->b.pmin[0];
            if (m->b.pmax[0] > s->b.pmax[0])
                s->b.pmax[0] = m->b.pmax[0];

            if(m->b.pmin[1] < s->b.pmin[1])
                s->b.pmin[1] = m->b.pmin[1];
            if (m->b.pmax[1] > s->b.pmax[1])
                s->b.pmax[1] = m->b.pmax[1];

            if(m->b.pmin[2] < s->b.pmin[2])
                s->b.pmin[2] = m->b.pmin[2];
            if (m->b.pmax[2] > s->b.pmax[2])
                s->b.pmax[2] = m->b.pmax[2];
		}
	}
	return 1;
}

void drawScene(Scene* scn){
	if (scn == NULL)
		return;

	if (scn->meshList){
		Mesh* m = NULL;
		for( int i = 0; i < scn->meshList->size; i++){ // para da mesh da cena
			m = fplist_getdata(i, scn->meshList);
			if (m->tris){
				Triangles* tri = NULL;
				for( int k = 0; k < m->tris->size; k++){ //para cada chunk de triangles do mesh
					tri = fplist_getdata(k, m->tris);
					//bindMaterial(&tri->mat, scn->lights->first->data);
					//bindShader(tri->mat.shaderid);
					drawIndexedVAO(tri->vaoId, tri->indicesCount, GL_TRIANGLES);
				}
			}
		}
	}

}
