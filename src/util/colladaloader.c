#include "colladaloader.h"
#include "ezxml.h"
#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "renderer.h"

enum inputsemantic {
    VERTEX, NORMAL, TEXCOORD
};

//COLLADA STRUCTS - FOR EASIER READING
typedef struct _source_t {
    char *id;
    int count;
    float *values;
} source_t;

typedef struct _input_t {
    int offset;
    int sourceid;
    int semantic;
} input_t;

//INTERNAL FORMAT REPRESENTATION - 

typedef struct _vertex {
    float pos[3];
    float normal[3];
    float s,t;
} vertex_t;

void removefirstchar(char *string) {
    for(unsigned int k = 1; k < strlen(string); k++)
        string[k - 1] = string[k];
    string[strlen(string) - 1] = '\0';
}

int findsourceid(source_t *sources, int numsources, const char *id) {
   for(int i = 0; i < numsources; i++)
        if(strcmp(sources[i].id, id) == 0)
            return i;
    return -1;
}

void getFloatArray(float **array, char *float_array, int count) {
    *array = malloc(sizeof(float)*count);
    char *tok = strtok(float_array, " ");
    int k = 0;
    while(tok) {
        (*array)[k] = atof(tok);
        tok = strtok(NULL, " ");
        k++;
    }
}

void readMaterialComponent(float* component, char *floatarray) {
    float *componentarray;
    getFloatArray(&componentarray, floatarray, 4);
    memcpy(component, componentarray, 4*sizeof(float));
    free(componentarray);
}

void readMaterial(ezxml_t collada, Scene *s) {
    //Começar lendo a <library_images>
    ezxml_t library_images = ezxml_child(collada, "library_images");
    
    for(ezxml_t imagetag = ezxml_child(library_images, "image"); imagetag; imagetag = imagetag->next) {
        Texture *t = malloc(sizeof(Texture));
        t->id = ezxml_attr(imagetag, "id");
        
        char *filepath = ezxml_child(imagetag, "init_from")->txt;
        //int n;
        //t->data = stbi_load(filepath, &t->width, &t->height, &n, 0);
        

        
        addTexture(s, t);
    }

    ezxml_t library_materials = ezxml_child(collada, "library_materials");

    for (ezxml_t materialtag = ezxml_child(library_materials, "material"); materialtag; materialtag = materialtag->next) {
        Material *mat = malloc(sizeof(Material));

        mat->id = ezxml_attr(materialtag, "id");

        ezxml_t instance_effect = ezxml_child(materialtag, "instance_effect");
        char *instance_effect_url = ezxml_attr(instance_effect, "url");
        if(instance_effect_url[0] == '#')
            removefirstchar(instance_effect_url);
        ezxml_t library_effects = ezxml_child(collada, "library_effects");
        //procura o effect correspondente
        ezxml_t effect = ezxml_child(library_effects, "effect");
        for(; effect; effect = effect->next) 
            if(strcmp(instance_effect_url, ezxml_attr(effect, "id")) == 0)
                break;
        //TODO profile GLSL, etc
        ezxml_t profile_COMMON = ezxml_child(effect, "profile_COMMON");
        ezxml_t technique = ezxml_child(profile_COMMON, "technique");

        ezxml_t techniquetype;
        if(ezxml_child(technique, "phong"))
            techniquetype = ezxml_child(technique, "phong");
        if(ezxml_child(technique, "blinn"))
            techniquetype = ezxml_child(technique, "blinn");

        //le as informações de material
        ezxml_t emission = ezxml_child(techniquetype, "emission");
        printf("componente emission do material: %s\n", ezxml_child(emission, "color")->txt);
        readMaterialComponent(mat->ke, ezxml_child(emission, "color")->txt);

        ezxml_t ambient = ezxml_child(techniquetype, "ambient");
        printf("componente ambient do material: %s\n", ezxml_child(ambient, "color")->txt);
        readMaterialComponent(mat->ka, ezxml_child(ambient, "color")->txt);

        ezxml_t diffuse = ezxml_child(techniquetype, "diffuse");

        //lê a componente difusa do material, de uma textura ou de uma cor sólida
        if(ezxml_child(diffuse, "texture")) {
            char *textureid = ezxml_attr(ezxml_child(diffuse, "texture"), "texture");
            printf("componente difusa do material eh uma textura: %s\n", textureid);
            
            //procura a textura com id lido
            Texture *t;
            for(fpnode *n = s->texList->first; n != NULL; n = n->next ) {
                t = (Texture*)n->data;
                if(strcmp(textureid, t->id) == 0)
                    break;
            }
            mat->diffmap = t;
            mat->diffsource = TEXTURE;
        } else if(ezxml_child(diffuse, "color")) {
            readMaterialComponent(mat->kd, ezxml_child(diffuse, "color")->txt);
            mat->diffsource = VEC;
        }
        //TODO possibilidade de ler specular de textura
        ezxml_t specular = ezxml_child(techniquetype, "specular");
        printf("componente specular do material: %s\n", ezxml_child(specular, "color")->txt);
        readMaterialComponent(mat->ks, ezxml_child(specular, "color")->txt);

        ezxml_t shininess = ezxml_child(techniquetype, "shininess");
        printf("componente shininess do material: %s\n", ezxml_child(shininess, "float")->txt);
        mat->shininess = atof(ezxml_child(shininess, "float")->txt);

        //Adiciona o material na lista de materials da cena
        addMaterial(s, mat);
    }
}

void readLibraryGeometries(ezxml_t library_geometries, Scene *s) {
    for(ezxml_t geometry = ezxml_child(library_geometries, "geometry"); geometry; geometry = geometry->next) {
        Mesh *m = malloc(sizeof(Mesh));
        m->id = ezxml_attr(geometry, "id");

        //Lê tag mesh
        ezxml_t meshtag = ezxml_child(geometry, "mesh");
        //Lê cada tag source;
        int numsources = 0;
        for(ezxml_t sourcetag = ezxml_child(meshtag, "source"); sourcetag; sourcetag = sourcetag->next)
            numsources++;

        source_t sources[numsources];
        int currsource = 0;
        for(ezxml_t sourcetag = ezxml_child(meshtag, "source"); sourcetag; sourcetag = sourcetag->next) {
            sources[currsource].id = ezxml_attr(sourcetag, "id");
            ezxml_t float_array = ezxml_child(sourcetag, "float_array");
            sources[currsource].count = atoi(ezxml_attr(float_array, "count"));
            getFloatArray(&sources[currsource].values, float_array->txt, sources[currsource].count);
            currsource++;
        }

        //Mudar o nome do source de vertices pra ficar mais fácil de achar
        ezxml_t verticestag = ezxml_child(meshtag, "vertices");
        ezxml_t inputvertices = ezxml_child(verticestag, "input");
        for(int i = 0; i < numsources; i++) {
            char *inputsource = ezxml_attr(inputvertices, "source");
            if(inputsource[0] == '#')
                removefirstchar(inputsource);
            if(strcmp(sources[i].id, inputsource) == 0) {
                sources[i].id = ezxml_attr(verticestag, "id");
                break;
            }
        }

        //Lê cada <triangles>
        for(ezxml_t trianglestag = ezxml_child(meshtag, "triangles"); trianglestag; trianglestag = trianglestag->next) {
            //Conta número de inputs
            int numinputs = 0;
            for(ezxml_t input = ezxml_child(trianglestag, "input"); input; input = input->next)
                numinputs++;
            input_t inputs[numinputs];
            //Lê cada input
            int currinput = 0;
            int maxoffset = 0;
            for(ezxml_t input = ezxml_child(trianglestag, "input"); input; input = input->next) {
                char *inputsource = ezxml_attr(input, "source");
                if(inputsource[0] == '#')
                    removefirstchar(inputsource);
                inputs[currinput].sourceid = findsourceid(sources, numsources, inputsource);
                inputs[currinput].offset = atoi(ezxml_attr(input, "offset"));

                if(strcmp(ezxml_attr(input, "semantic"), "VERTEX") == 0)
                    inputs[currinput].semantic = VERTEX;
                else if(strcmp(ezxml_attr(input, "semantic"), "NORMAL") == 0)
                    inputs[currinput].semantic = NORMAL;
                else if(strcmp(ezxml_attr(input, "semantic"), "TEXCOORD") == 0)
                    inputs[currinput].semantic = TEXCOORD;
                if(inputs[currinput].offset > maxoffset)
                    maxoffset = inputs[currinput].offset;
                currinput++;
            }
            //Lê p
            ezxml_t ptag = ezxml_child(trianglestag, "p");

            Triangles *t = addTris(m); //malloc(sizeof(Triangles));

            //Lê material - procura na lista de materials o material com o id lido
            char *matlabel = ezxml_attr(trianglestag, "material");
            Material *material;
            for(fpnode *matnode = s->materialList->first; matnode; matnode = matnode->next) {
                material = (Material*)matnode->data;
                if(strcmp(material->id, matlabel) == 0)
                    t->material = material;
            }
            
            int numtriangles = atoi(ezxml_attr(trianglestag, "count"));
            int numindices = 3*numtriangles*(maxoffset + 1);
            int indices[numindices];
            char *tok = strtok(ptag->txt, " ");
            int k = 0;
            while(tok) {
                indices[k] = atoi(tok);
                tok = strtok(NULL, " ");
                k++;
            }
            vertex_t vertices[3*numtriangles];
            for(int i = 0; i < 3*numtriangles; i++){
                vertex_t *vertex = &vertices[i];//&m->triangles[currtri].vertices[i];
                for(int j = 0; j < numinputs; j++) {
                    int sourceid = inputs[j].sourceid;
                    float *array = sources[sourceid].values;
                    int index = indices[(maxoffset + 1)*i + inputs[j].offset];
                    switch(inputs[j].semantic) {
                        case VERTEX:
                            //ler indices[i + inputs[currinput].offset]
                            //printf("vertex: %f, %f, %f\n", array[3*index], array[3*index + 1], array[3*index + 2]);
                            vertex->pos[0] = array[3*index];
                            vertex->pos[1] = array[3*index + 1] ;
                            vertex->pos[2] = array[3*index + 2];
                            break;
                        case NORMAL:
                            //printf("normal: %f, %f, %f\n", array[3*index], array[3*index + 1], array[3*index + 2]);
                            vertex->normal[0] = array[3*index];
                            vertex->normal[1] = array[3*index + 1];
                            vertex->normal[2] = array[3*index + 2];
                            break;
                        case TEXCOORD:
                            //printf("texcoord: %f, %f\n", array[2*index], array[2*index + 1]);
                            vertex->s = array[2*index];
                            vertex->t = array[2*index + 1];
                            break;
                    }
                }
            }
            unsigned int *tIndices = malloc(sizeof(unsigned int)*3*numtriangles);
            for(unsigned int i = 0; i < 3*numtriangles; i++)
                tIndices[i] = i;
            addIndices(t, 3*numtriangles, tIndices);
            
            float *tVerts = malloc(sizeof(float)*3*3*numtriangles);
            for(unsigned int i = 0, k = 0; i < 3*numtriangles; i++, k+=3) {
                tVerts[k] = vertices[i].pos[0];
                tVerts[k + 1] = vertices[i].pos[1];
                tVerts[k + 2] = vertices[i].pos[2];
            }
            addVertices(t, 3*3*numtriangles, 3, tVerts);

            float *tNormals = malloc(sizeof(float)*3*3*numtriangles);
            for(unsigned int i = 0, k = 0; i < 3*numtriangles; i++, k+=3) {
                tNormals[k] = vertices[i].normal[0];
                tNormals[k + 1] = vertices[i].normal[1];
                tNormals[k + 2] = vertices[i].normal[2];
            }
            addNormals(t, 3*3*numtriangles, 3, tNormals);

            float *tTexCoords = malloc(sizeof(float)*2*3*numtriangles);
            for(unsigned int i = 0, k = 0; i < 3*numtriangles; i++, k+=2) {
                tTexCoords[k] = vertices[i].s;
                tTexCoords[k + 1] = vertices[i].t;
            }
            addTexCoords(t, 2*3*numtriangles, 2, 0, tTexCoords);
        }
        //fplist_insback(m, s->meshList);
        addMesh(s, m);
    }
}

void readLight(ezxml_t library_lights, Scene *s) {
    //ler cada light
    for(ezxml_t lighttag = ezxml_child(library_lights, "light"); lighttag; lighttag = lighttag->next) {
        //Cria luz
        Light *l = malloc(sizeof(Light));
        
        l->pos[0] = 1.3;
        l->pos[1] = -2.4;
        l->pos[2] = 4.4;
        l->color[0] = 1.0;
        l->color[1] = 1.0;
        l->color[2] = 1.0;
        l->color[3] = 1.0;

        addLight(s, l);
 /*       ezxml_t technique_common = ezxml_child(lighttag, "technique_common");
        //Directional
        ezxml_t directional = ezxml_child(technique_common, "directional");
        if(directional) {
            l->type = DIRECTIONAL;
            ezxml_t color = ezxml_child(directional, "color");
            printf("light color: %s\n", color->txt);
            float *colorvec;
            getFloatArray(&colorvec, color->txt, 3);
            memcpy(l->color, colorvec, sizeof(float)*3);
            free(colorvec);
        }
        //Ambient
        ezxml_t ambient = ezxml_child(technique_common, "ambient");
        if(ambient) {
            l->type = AMBIENT;
            ezxml_t color = ezxml_child(ambient, "color");
            float *colorvec;
            getFloatArray(&colorvec, color->txt, 3);
            memcpy(l->color, colorvec, sizeof(float)*3);
            free(colorvec);
        }
                //TODO Point
        //TODO Spot*/
    }
}


enum { CAMERA_NODE, LIGHT_NODE, GEOMETRY_NODE };
int getNodeType(ezxml_t nodetag) {
    if(ezxml_child(nodetag, "instance_camera"))
        return CAMERA_NODE;
    else if(ezxml_child(nodetag, "instance_light"))
        return LIGHT_NODE;
    else if(ezxml_child(nodetag, "instance_geometry"))
        return GEOMETRY_NODE;
}

/*
//TODO lendo apenas node de geometry
node_t* readNode(ezxml_t nodetag, scene *s, node_t *parent) {
    int nchild = 0;
    for(ezxml_t childnodetag = ezxml_child(nodetag, "node"); childnodetag; childnodetag = childnodetag->next) 
        nchild++;

    node_t *n = malloc(sizeof(node_t));
    n->parent = parent;
    n->m = NULL;
    n->numchildren = nchild;
    fpIdentity(n->transformation);

    //Procura o mesh e o associa ao node criado
    char *geometryid = ezxml_attr(ezxml_child(nodetag, "instance_geometry"), "url");
    removefirstchar(geometryid);
    for(int i = 0; i < s->nummeshes; i++)
        if(strcmp(s->meshes[i].id, geometryid) == 0) 
            n->m = &s->meshes[i];
    
    //Associa a transformação devida
    //TODO considerar tags <rotate> e <matrix>
    //TODO considerar a ordem das transformações
    ezxml_t translate = ezxml_child(nodetag, "translate");
    if(translate) {
        float dx, dy, dz;
        dx = atof(strtok(translate->txt, " "));
        dy = atof(strtok(NULL, " "));
        dz = atof(strtok(NULL, " "));
        
        fptranslatef(n->transformation, dx, dy, dz);
    }
    if(nchild > 0) {
        n->children = malloc(sizeof(node_t*)*nchild);
        int i = 0;
        for(ezxml_t childnodetag = ezxml_child(nodetag, "node"); childnodetag; childnodetag = childnodetag->next, i++) {
            n->children[i] = readNode(childnodetag, s, n);
        }
    }

    return n;
}


void readScene(ezxml_t library_visual_scenes, scene* s) {
    ezxml_t visual_scene = ezxml_child(library_visual_scenes, "visual_scene");
    printf("<visual_scene>\n");
    
    //TODO função pra criar um nó e ajustar os ponteiros
    s->sgroot = malloc(sizeof(node_t));
    s->sgroot->parent = NULL;
    s->sgroot->numchildren = 0;
    s->sgroot->m = NULL;
    fpIdentity(s->sgroot->transformation);
    //Conta os filhos
    //TODO considerando apenas os nós de geometry
    for(ezxml_t nodetag = ezxml_child(visual_scene, "node"); nodetag; nodetag = nodetag->next)
        if(ezxml_child(nodetag, "instance_geometry"))
            s->sgroot->numchildren++;

    //aloca o vetor de filhos
    s->sgroot->children = malloc(sizeof(node_t*)*s->sgroot->numchildren);
    
    int i = 0;
    for(ezxml_t nodetag = ezxml_child(visual_scene, "node"); nodetag; nodetag = nodetag->next) {
        int nodetype = getNodeType(nodetag);
        if(nodetype == GEOMETRY_NODE) {
           s->sgroot->children[i] = readNode(nodetag, s, s->sgroot);
           i++;
        }
        else if(nodetype == LIGHT_NODE)
            printf("light node - ignoring\n");
        else if(nodetype == CAMERA_NODE)
            printf("camera node - ignoring\n");

    }
}*/

Scene* readColladaFile(const char *filename) {
    ezxml_t collada  = ezxml_parse_file(filename);

    if(!collada) {
        printf("Invalid file: %s\n", filename);
        return NULL;
    }
    Scene *s = initializeScene();
    //Read material
    readMaterial(collada, s);
    //Read geometry
    readLibraryGeometries(ezxml_child(collada, "library_geometries"), s);
    //Read lights
    readLight(ezxml_child(collada, "library_lights"), s);
    
    //readScene(ezxml_child(collada, "library_visual_scenes"), s);

    //setup scene
    //setupScene(s);
    return s;
}
