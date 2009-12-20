#include "scene.h"
#include "../util/ezxml.h"
#include "../util/malloc.h"
#include <stdio.h>

scene* initializeDae(char* filename){

	ezxml_t dae = ezxml_parse_file(filename);
	if (!dae){
		printf("File not found: %s\n", filename);
		return NULL;
	}

	scene* s = dlmalloc(sizeof(scene));
	s->meshCount  = 0;
	s->textureCount = 0;

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



	}




	ezxml_free(dae);
}

