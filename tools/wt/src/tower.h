#ifndef TOWER_H_
#define TOWER_H_

#include "renderer/mesh.h"
#include "util/utarray.h"

typedef struct TowerRoom{
    unsigned int numrows, numcols;
    float r, g, b;
    char *name;
    unsigned int row, col;
    struct TowerRoom *prev, *next;
} TowerRoom;

typedef struct TowerFloor {
    unsigned int numrows, numcols;
    float *vertices; //precisa pro picking
    float *colors; //também pro picking
    unsigned int **grid;
    Mesh *m;

    TowerRoom *roomList;

    struct TowerFloor *prev, *next;
} TowerFloor;

void towerFloorDtor(void *twrfloor);

typedef struct _tower {
    UT_array *floorList;
    //TowerFloor *floorList;
}Tower;

Tower* initTower();
TowerFloor* addFloor(Tower *t, int rows, int cols);
void removeFloor(Tower *t, int floor);

TowerFloor* generateTowerFloor(int rows, int cols);
int placeRoom(TowerFloor *tf, const TowerRoom *tr, int row, int col);
int testRoom(TowerFloor *tf, const TowerRoom *tr, int row, int col);
void changeTileColor(TowerFloor *tf, const TowerRoom *tr, int row, int col);


int loadRoomFile(const char *filename, int *numRooms, TowerRoom **towerRooms);
//FIXME so pra testar tá passando uma TowerFloor
//retornar algum tipo de erro
void saveGameState(Tower *t, const char *filepath);
int loadGameState(Tower *t, const char *filename);
#endif
