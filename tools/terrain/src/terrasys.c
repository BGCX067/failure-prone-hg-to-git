#include "terrasys.h"
#include <stdlib.h>
#include "util/sdnoise1234.h"
#include "math/util.h"

void ParseTerrainNodes(TerrainParam* node){


    int sizex = 512;
    int sizey = 512;

    if (node == NULL)
	return;

     ParseTerrainNodes(node->inputs[0]);
     //dotrick
     if (node->type == NOISE){
	for(int j = 0; j < sizey; j++)
		for(int i = 0; i < sizex; i++)
			node->terrain[i + j*sizex] = fbm2( (float) (i),  (float) (j), node->frequency/1.0, node->lacunarity*10.0, node->persistence/10.0, (int)(node->octaves*10))*10;

     } else if ( node->type == MIX ) {
	for (int j = 0; j < sizey; j++)
		for(int i = 0; i < sizex; i++)
			node->terrain[i + j*sizex] = node->inputs[0]->terrain[i+j*sizex]*(1-node->a) + node->inputs[1]->terrain[i+j*sizex]*node->a;
     } else if ( node->type == MAX ) {
	for (int j = 0; j < sizey; j++)
		for(int i = 0; i < sizex; i++)
			node->terrain[i + j*sizex] = max(node->inputs[0]->terrain[i+j*sizex],  node->inputs[1]->terrain[i+j*sizex]);
     } else if ( node->type == MIN ) {
	for (int j = 0; j < sizey; j++)
		for(int i = 0; i < sizex; i++)
			node->terrain[i + j*sizex] = min(node->inputs[0]->terrain[i+j*sizex], node->inputs[1]->terrain[i+j*sizex]);
     } else if ( node->type == ADD ) {
	for (int j = 0; j < sizey; j++)
		for(int i = 0; i < sizex; i++)
			node->terrain[i + j*sizex] = node->inputs[0]->terrain[i+j*sizex] + node->inputs[1]->terrain[i+j*sizex];
     } else if ( node->type == MUL ) {
	for (int j = 0; j < sizey; j++)
		for(int i = 0; i < sizex; i++)
			node->terrain[i + j*sizex] = node->inputs[0]->terrain[i+j*sizex] * node->inputs[1]->terrain[i+j*sizex];

    }
    

     ParseTerrainNodes(node->inputs[1]);

}

TerrainParam* InitNoiseParam(int sizex, int sizey, float persistence, float lacunarity, float frequency, float octaves){

	TerrainParam* ret = malloc(sizeof(TerrainParam));
	if (ret == NULL)
		return NULL;

    	ret->lacunarity =lacunarity;
	ret->octaves = octaves;
    	ret->frequency = frequency;
	ret->persistence = persistence;
    	ret->terrain = malloc(sizex*sizey*(sizeof(float)));
	ret->type = NOISE;
	ret->inputs[0] = NULL;
	ret->inputs[1] = NULL;
	
	return ret;
}

TerrainParam* InitTerrainParam(int type){

	TerrainParam* ret = malloc(sizeof(TerrainParam));
	if (ret == NULL)
		return NULL;

	ret->type = type;
	ret->inputs[0] = NULL;
	ret->inputs[1] = NULL;
	
	return ret;
}
