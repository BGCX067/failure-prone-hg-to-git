#include <stdio.h>
#include "glapp.h"
#include "math/vec3.h"
#include "math/mathutil.h"
#include "renderer/renderer.h"
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "renderer/glime.h"
#include "renderer/camera.h"
#include "renderer/gui.h"
#include "util/colladaloader.h"
#include "util/sdnoise1234.h"
#include "util/image.h"
#include "util/utlist.h"
#include "util/textfile.h"
#include "terrasys.h"
#include "terragui.h"
#include <stdlib.h>

Scene* cena;
Shader *shdr;
Camera c;
Light l;

float* terrain;
unsigned int* indices;

float persistence = 0.1;
float lacunarity = 0.2;
float frequency = 0.1;
float octaves = 6.0/10.0;

Mesh* tMesh;

TerrainParam* root;

void changeTerrain(){
    int sizex, sizey;
    sizex = sizey = 512;

    for(int j = 0; j < sizey; j++)
	for(int i = 0; i < sizex; i++)
		terrain[j + i*sizex] = fbm2( (float) (i),  (float) (j), frequency/1.0, lacunarity*10.0, persistence/10.0, (int)(octaves*10))*10;

    float* vertices = malloc(sizeof(float)*sizex*sizey*3);
    for(int j = 0; j < sizey; j++)
        for(int i = 0; i < sizex; i++) {
            vertices[3*i + 3*j*sizex] = (float) i;
            vertices[3*i + 3*j*sizex + 1] = terrain[i + j*sizex];
            vertices[3*i + 3*j*sizex + 2] = (float) j;
        }

    float* normals = calloc(sizex*sizey*3,sizeof(float));
    SetNormals(indices, vertices, normals, (sizex - 1)*(sizey - 1)*2, sizex*sizey);

    UpdateMeshNormals(tMesh, normals);
    UpdateMeshVertices(tMesh, vertices);

    free(normals);
    free(vertices);
}

//TODO receber um intervalo que diz o tamanho em cada dimensão
//sizex e sizey representam o número de vertices nos eixos X e Z
//logo o menor valor será 2 para estes argumentos
void generateTerrain(TerrainParam* root, int sizex, int sizey){

   ParseTerrainNodes(root);

    terrain = root->inputs[0]->terrain;

    indices = malloc(sizeof(unsigned int)*(sizex - 1)*(sizey - 1)*2*3);
    float* vertices = malloc(sizeof(float)*sizex*sizey*3);
    
    for(int j = 0; j < sizey; j++)
        for(int i = 0; i < sizex; i++) {
            vertices[3*i + 3*j*sizex] = (float) i;
            vertices[3*i + 3*j*sizex + 1] = terrain[i + j*sizex];
            vertices[3*i + 3*j*sizex + 2] = (float) j;
        }
    //indices analisando cada tile
    //0--1
    //| /|
    //|/ |
    //2--3
    //
    // .-----> i
    // |     
    // |
    //\|/
    // V
    // j      
    for(int j = 0, index = 0; j < sizey - 1; j++)
        for(int i = 0; i < sizex - 1; i++) {
            int vertIndex = i + j*sizex;
            //First tri
            //0--1
            //| /
            //|/  
            //2
            indices[index] = vertIndex;
            indices[index + 1] = vertIndex + sizex;
            indices[index + 2] = vertIndex + 1;
            //Second tri
            //   1
            //  /|
            // / |
            //2--3
            indices[index + 3] = vertIndex + 1;
            indices[index + 4] = vertIndex + sizex;
            indices[index + 5] = vertIndex + sizex + 1;
            index += 6;
        }
    float* normals = calloc(sizex*sizey*3,sizeof(float));
    SetNormals(indices, vertices, normals, (sizex - 1)*(sizey - 1)*2, sizex*sizey);
    AddVertices(tMesh, sizex*sizey*3, 3, vertices);
    AddIndices(tMesh, (sizex - 1)*(sizey - 1)*2*3, indices);
    AddNormals(tMesh, sizex*sizey*3, 3, normals);
    PrepareMesh(tMesh);
    Node *tNode = AddMesh(cena, tMesh);

    vec3 ka = {0.2, 0.4, 0.2};
    vec3 kd = {0.2, 0.4, 0.2};
    vec3 ks = {0.2, 0.4, 0.2};
    float shininess = 12.0;
    vec3 lightpos = {256, 100, 256};
    vec3 lightcolor = {1.0, 1.0, 1.0};
    tNode->material = PhongMaterial(ka, kd, ks, shininess, lightpos, lightcolor);
    free(vertices);
    free(normals);
    //free(indices);
//    free(terrain);
}

static void subdivide(vec3 v1, vec3 v2, vec3 v3, int depth, float *vertices)
{
    vec3 v12, v23, v31;
    static int index = 0;
    if (depth == 0) {
        vertices[index] = v1[0];
        vertices[index + 1] = v1[1];
        vertices[index + 2] = v1[2];

        vertices[index + 3] = v2[0];
        vertices[index + 4] = v2[1];
        vertices[index + 5] = v2[2];

        vertices[index + 6] = v3[0];
        vertices[index + 7] = v3[1];
        vertices[index + 8] = v3[2];
        index += 9;
        return;
    }

    Addv(v1, v2, v12);
    Addv(v2, v3, v23);
    Addv(v3, v1, v31);

    Normalizev(v12);
    Normalizev(v23);
    Normalizev(v31);

    subdivide(v1, v12, v31, depth-1, vertices);
    subdivide(v2, v23, v12, depth-1, vertices);
    subdivide(v3, v31, v23, depth-1, vertices);
    subdivide(v12, v23, v31, depth-1,vertices);
}

void generateSkyDome(int ndiv, float radius) {
    int ntris = 20*pow(4, ndiv);

    unsigned int *indices = malloc(sizeof(unsigned int)*ntris*3);
    for(int i = 0; i < 3*ntris; i++)
        indices[i] = i;

    float *vertices = malloc(sizeof(float)*ntris*3*3); // 3 vertices por triangulo, com 3 componentes
    
    const float X = 0.525731112119133606; //TODO MAGIC NUMBERS ?!!
    const float Z = 0.850650808352039932;

    vec3 vdata[12];
    vdata[0][0] = -X; vdata[0][1] = 0.0; vdata[0][2] = Z;
    vdata[1][0] = X; vdata[1][1] = 0.0; vdata[1][2] = Z;
    vdata[2][0] = -X; vdata[2][1] = 0.0; vdata[2][2] = -Z;
    vdata[3][0] = X; vdata[3][1] = 0.0; vdata[3][2] = -Z;
    vdata[4][0] = 0.0; vdata[4][1] = Z; vdata[4][2] = X;
    vdata[5][0] = 0.0; vdata[5][1] = Z; vdata[5][2] = -X;
    vdata[6][0] = 0.0; vdata[6][1] = -Z; vdata[6][2] = X;
    vdata[7][0] = 0.0; vdata[7][1] = -Z; vdata[7][2] = -X;
    vdata[8][0] = Z; vdata[8][1] = X; vdata[8][2] = 0.0;
    vdata[9][0] = -Z; vdata[9][1] = X; vdata[9][2] = 0.0;
    vdata[10][0] = Z; vdata[10][1] = -X; vdata[10][2] = 0.0;
    vdata[11][0] = -Z; vdata[11][1] = -X; vdata[11][2] = 0.0;

    int tindices[20][3] = { {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
                            {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
                            {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
                            {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };
    for(int i = 0; i < 20; i++)
        subdivide(vdata[tindices[i][0]], vdata[tindices[i][1]], vdata[tindices[i][2]], ndiv, vertices);

    for(int i = 0; i < ntris*3*3; i++)
        vertices[i] *= radius;


    Mesh *m = InitMesh();
    float* normals = malloc(sizeof(float)*ntris*3*3);
    SetNormals(indices, vertices, normals, ntris, ntris*3);
    AddVertices(m, ntris*3*3, 3, vertices);
    AddIndices(m, ntris*3, indices);
    AddNormals(m, ntris*3*3, 3, normals);
    PrepareMesh(m);
    Node *skyNode = AddMesh(cena, m);

    skyNode->material = AtmosphereMaterial();

    free(vertices);
    free(normals);
    free(indices);
}

int menux, menuy;

void InitializeGame(){

    CamInit(&c, GetScreenW(), GetScreenH(), TRACKBALL, PERSPECTIVE);
    SetZfar(&c, 2000.0);  
    SetProjection(c.mprojection);
    c.zoom = 5.0;

    menux = menuy = 0;
    tMesh = InitMesh();
    InitializeGUI(1280, 960);

    terrain = malloc(512*512*sizeof(float));

    cena = InitializeScene();
    
    //generateTerrain(512, 512);
    generateSkyDome(5, 1024);

  //  stbi_write_tga("noise.tga", 512, 512, 1, terrainbmp );

    //char *vertshader = ReadTextFile("data/shaders/phong.vert");
    //char *fragshader = ReadTextFile("data/shaders/phong.frag");
    //shdr = InitializeShader(NULL, vertshader, fragshader); 
    //free(vertshader);
    //free(fragshader);

    l.pos[0]= 256.0; l.pos[1] = 10.0; l.pos[2] = 256.0;
    
    //mat = colorMaterialDir();
    glPolygonMode(GL_BACK, GL_LINE);
}

void Update(event* e, double* dt){
    c.update(&c, e, dt);
    SetView(c.mview);

}

void Render(event *e, double* dt){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Desenhar cena
    DrawScene(cena);
    /*Mesh *it;
    DL_FOREACH(cena->meshList, it) {
	SetModel(it->transform);
        BindShader(it->material);
        DrawIndexedVAO(it->vaoId, it->indicesCount, GL_TRIANGLES);
	i++;
    }*/

    BeginGUI(e);
	DrawGUINode();

    EndGUI();

    glFlush();
}


int main(){
	SetVideoMode(1280, 960, 0);
	WarpMouse(0);
	SetWindowTitle("Terra");
	InitializeRenderer(1280, 960, 0.1, 10000.0, 45.0);
	InitializeGame();
	MainLoop();

	CloseVideo();
	return 0;
}



