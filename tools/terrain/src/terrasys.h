#ifndef _TERRASYS_H_
#define _TERRASYS_H_



enum TerrainNodeType {
  
          ROOT,
          NOISE,
          MIX,
          ADD,
          MUL,
          MAX,
          MIN
 };
  
typedef struct _TerrainParam TerrainParam;
  
struct _TerrainParam{
  
          int type;
  
          //se for noise
          float persistence;
          float lacunarity;
          float frequency;
          float octaves;
  
          float* terrain;
  
          //se for blend
          float a;
  
          //se for erosion
  
          TerrainParam* inputs[2];
  
};

//recebe a arvore dos terrenos e gera os dados de cada no
void ParseTerrainNodes(TerrainParam* node);

TerrainParam* InitNoiseParam( int sizex, int sizey, float persistence, float lacunarity, float frequency, float octaves);
TerrainParam* InitTerrainParam(int type);

#endif
