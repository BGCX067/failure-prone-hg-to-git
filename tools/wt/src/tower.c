#include <string.h>
#include "tower.h"
#include "util/ezxml.h"
#include "util/utlist.h"

static UT_icd floorptricd = {sizeof(TowerFloor*), NULL, NULL, NULL};

void towerFloorDtor(void *twrfloor) {
    TowerFloor *tf = *((TowerFloor**)twrfloor);
    TowerRoom *it, *tmp;
    //FIXME liberar memória das salas
    DL_FOREACH_SAFE(tf->roomList, it, tmp) {
        DL_DELETE(tf->roomList, it);
    }
    //liberar grid, mesh vertices, colors etc
    free(tf);
}

Tower* initTower() {
    Tower *t = malloc(sizeof(Tower));
    utarray_new(t->floorList, &floorptricd);
    //t->floorList = NULL;
    return t;
}

TowerFloor* addFloor(Tower *t, int rows, int cols) {
    TowerFloor *tf = generateTowerFloor(rows, cols);
    utarray_push_back(t->floorList, &tf);

    //TODO free(tf); ?
    //DL_APPEND(t->floorList, tf);
    return *((TowerFloor**)utarray_back(t->floorList));
}


void removeFloor(Tower *t, int floor) {
    utarray_erase(t->floorList, floor, 1);
    /*TowerFloor *it;
    int i = 0;
    DL_FOREACH(t->floorList, it) {
        if(i == floor)
            DL_DELETE(t->floorList, it);
        i++;
    }*/
}

TowerFloor* generateTowerFloor(int rows, int cols) {
    TowerFloor *twr = malloc(sizeof(TowerFloor));
    twr->numrows = rows;
    twr->numcols = cols;
    twr->grid = malloc(sizeof(unsigned int*)*rows);
    for(unsigned int i = 0; i < twr->numrows; i++)
        twr->grid[i] = malloc(sizeof(unsigned int)*cols);
    for(int i = 0; i < rows; i++)
    for(int j = 0; j < cols; j++)
        twr->grid[i][j] = 0;

    unsigned int *indices = malloc( sizeof(unsigned int)*rows*cols*2*3 );
    for(int i = 0; i < rows*cols*2*3; i++)
        indices[i] = i;
    
    twr->vertices = malloc( sizeof(float)*rows*cols*6*3 );

    for(int i = 0, index = 0; i < rows; i++){
	for(int j = 0; j < cols; j++){
        //Vert 1
		twr->vertices[index] = (float)i;
		twr->vertices[index+1] = 0.0;
		twr->vertices[index+2] = (float)j;
        
        //Vert 3
		twr->vertices[index+3] = (float)i;
		twr->vertices[index+4] = 0.0; 
		twr->vertices[index+5] = (float)(j+1.0);

        //Vert 2
		twr->vertices[index+6] = (float)(i+1.0);
		twr->vertices[index+7] = 0.0;
		twr->vertices[index+8] = (float)j;

        //Vert 4
		twr->vertices[index+9] = (float)i;
		twr->vertices[index+10] = 0.0;
		twr->vertices[index+11] = (float)(j+1.0);
        
        //Vert 6
		twr->vertices[index+12] = (float)(i+1.0);
		twr->vertices[index+13] = 0.0;
		twr->vertices[index+14] = (float)(j+1.0);

        //Vert 5
		twr->vertices[index+15] = (float)(i+1.0);
		twr->vertices[index+16] = 0.0;
		twr->vertices[index+17] = (float)j;

		index += 18;
	}
    }

    float* normals = malloc(sizeof(float)*rows*cols*6*3);
    for(int i = 0; i < rows*cols*6; i++) {
        normals[3*i] = 0.0;
        normals[3*i + 1] = 1.0;
        normals[3*i + 2] = 0.0;
    }

    //float* colors = malloc(sizeof(float)*rows*cols*6*3);
    twr->colors = malloc(sizeof(float)*rows*cols*6*3);
    for(int i = 0; i < rows*cols*6; i++) {
        twr->colors[3*i] = 0.0;
        twr->colors[3*i + 1] = 1.0;
        twr->colors[3*i + 2] = 0.0;
    }
    
    //Triangles *t = initTris();
    Mesh *m = initMesh();
    addVertices(m, rows*cols*6*3, 3, twr->vertices);
    addIndices(m, rows*cols*6, indices);
    addNormals(m, rows*cols*6*3, 3, normals);
    addColors(m, rows*cols*6*3, twr->colors);
    m->material = colorMaterialNoLight();
    prepareMesh(m);

    twr->m = m;

    twr->roomList = NULL;

    //free(indices);
    free(normals);
    
    return twr;
}

int placeRoom(TowerFloor *tf, const TowerRoom *tr, int row, int col) {
    if(testRoom(tf, tr, row, col) == 0)
        return 0;
    changeTileColor(tf, tr, row, col);

    TowerRoom *newRoom = malloc(sizeof(TowerRoom));
    memcpy(newRoom, tr, sizeof(TowerRoom));
    newRoom->row = row;
    newRoom->col = col;
    DL_APPEND(tf->roomList, newRoom);
    
    return 1;
}


int testRoom(TowerFloor *tf, const TowerRoom *tr, int row, int col) {
    if(row + tr->numrows > tf->numrows)
        return 0;
    if(col + tr->numcols > tf->numcols)
        return 0;

    //Verificar se algum tile já está ocupado
    for(unsigned int i = 0; i < tr->numrows; i++)
        for(unsigned int j = 0; j < tr->numcols; j++)
            if(tf->grid[row + i][col + j])
                //printf("tile ocupado: %d, %d!\n", i, j);
                return 0;
    return 1;
}


void changeTileColor(TowerFloor *tf, const TowerRoom *tr, int row, int col) {
    for(unsigned int i = 0; i < tr->numrows; i++)
        for(unsigned int j = 0; j < tr->numcols; j++) {
            //Marca tile como ocupado
            tf->grid[row + i][col + j] = 1;
            //Precisa, a partir de row e col pegar o tilenum
            int currtile = (col + j)*tf->numrows + (row + i);
            for(int k = 0; k < 6; k++) {
                tf->colors[3*(6*currtile + k)] = tr->r;
                tf->colors[3*(6*currtile + k) + 1] = tr->g;
                tf->colors[3*(6*currtile + k) + 2] = tr->b;
            }
        }
}

int loadRoomFile(const char *filename, int *numRooms, TowerRoom **tr) {
    //TowerRoom *tr = *towerRooms;
    
    ezxml_t wtxml = ezxml_parse_file(filename);
	if (!wtxml){
		printf("File not found: %s \n", filename);
		return 0;
	}

    int roomCount = 0;
    for(ezxml_t room = ezxml_child(wtxml, "room"); room; room = room->next)
        roomCount++;

    if(roomCount == 0) {
        printf("Empty file\n");
        return 0;
    }
    
    *numRooms = roomCount;
    (*tr) = malloc(sizeof(TowerRoom)*roomCount);
    
    printf("Roomcount: %d\n", roomCount);
    int i = 0;
    for(ezxml_t room = ezxml_child(wtxml, "room"); room; room = room->next) {
        printf("id: %s, name: %s\n", ezxml_attr(room, "id"), ezxml_attr(room, "name"));
        printf("numrows: %s, numcols: %s\n", ezxml_child(room, "numrows")->txt, ezxml_child(room, "numcols")->txt);

        (*tr)[i].name = (char*)ezxml_attr(room, "name");
        (*tr)[i].numrows = atoi(ezxml_child(room, "numrows")->txt);
        (*tr)[i].numcols = atoi(ezxml_child(room, "numcols")->txt);

        char *color = ezxml_child(room, "color")->txt;
        float rgb[3];
        char *tok = strtok(color, " ");
        int k = 0;
        while(tok) {
            rgb[k] = atof(tok);
            tok = strtok(NULL, " ");
            k++;
        }
        (*tr)[i].r = rgb[0];
        (*tr)[i].g = rgb[1];
        (*tr)[i].b = rgb[2];
        i++;
    }

    ezxml_free(wtxml);

    return 1;
}


void saveGameState(Tower *t, const char *filepath) {
    ezxml_t gameState = ezxml_new("wizard-tower");
    
    TowerFloor **twrf;
    for(TowerFloor **it = utarray_front(t->floorList); it; it = utarray_next(t->floorList, it)){
    //DL_FOREACH(t->floorList, tf) {
        TowerFloor *tf = *it;
        ezxml_t towerFloor = ezxml_add_child(gameState, "tower-floor", 0);
        char buffer[32];
        sprintf(buffer, "%d", tf->numrows);
        ezxml_set_attr_d(towerFloor, "numrows", buffer);
        sprintf(buffer, "%d", tf->numcols);
        ezxml_set_attr_d(towerFloor, "numcols", buffer);

        int roomListSize = 0;
        TowerRoom *it;
        DL_FOREACH(tf->roomList, it)
            roomListSize++;
        sprintf(buffer, "%d", roomListSize);
        ezxml_set_attr_d(towerFloor, "numrooms", buffer);
        
        TowerRoom *room;
        DL_FOREACH(tf->roomList, room) {
            ezxml_t towerRoom = ezxml_add_child(towerFloor, "tower-room", 0);
            ezxml_set_attr_d(towerRoom, "name", room->name);

            ezxml_t nrows = ezxml_add_child(towerRoom, "numrows", 0);
            char buff[32];
            sprintf(buff, "%d", room->numrows);
            ezxml_set_txt_d(nrows, buff);

            ezxml_t ncols = ezxml_add_child(towerRoom, "numcols", 0);
            sprintf(buff, "%d", room->numcols);
            ezxml_set_txt_d(ncols, buff);

            ezxml_t color = ezxml_add_child(towerRoom, "color", 0);
            sprintf(buff, "%f %f %f", room->r, room->g, room->b);
            ezxml_set_txt_d(color, buff);

            ezxml_t row = ezxml_add_child(towerRoom, "row", 0);
            sprintf(buff, "%d", room->row);
            ezxml_set_txt_d(row, buff);

            ezxml_t col = ezxml_add_child(towerRoom, "col", 0);
            sprintf(buff, "%d", room->col);
            ezxml_set_txt_d(col, buff);
        }
    }

    char *xmltext = ezxml_toxml(gameState);
    static char *header = "<?xml version=\"1.0\"?>";

    FILE *fp = fopen(filepath, "w");
    fprintf(fp, "%s\n", header);
    fprintf(fp, "%s\n", xmltext);
    fclose(fp);
}

int loadGameState(Tower *t, const char *filename) {
    ezxml_t wtxml = ezxml_parse_file(filename);
	if (!wtxml){
		printf("File not found: %s \n", filename);
		return 0;
	}
    
    for(ezxml_t towerFloor = ezxml_child(wtxml, "tower-floor"); towerFloor; towerFloor = towerFloor->next) {
        int rows = atoi(ezxml_attr(towerFloor, "numrows"));
        int cols = atoi(ezxml_attr(towerFloor, "numcols"));
        printf("gerando andar %d por %d\n", rows, cols);
        //(*tf) = generateTowerFloor(rows, cols);
        TowerFloor *tf = addFloor(t, rows, cols);

        for(ezxml_t towerRoom = ezxml_child(towerFloor, "tower-room"); towerRoom; towerRoom = towerRoom->next) {
            TowerRoom tr;
            tr.name = (char*)ezxml_attr(towerRoom, "name");
            tr.numrows = atoi(ezxml_child(towerRoom, "numrows")->txt);
            tr.numcols = atoi(ezxml_child(towerRoom, "numcols")->txt);

            char *color = ezxml_child(towerRoom, "color")->txt;
            float rgb[3];
            char *tok = strtok(color, " ");
            int k = 0;
            while(tok) {
                rgb[k] = atof(tok);
                tok = strtok(NULL, " ");
                k++;
            }
            tr.r = rgb[0];
            tr.g = rgb[1];
            tr.b = rgb[2];

            tr.row = atoi(ezxml_child(towerRoom, "row")->txt);
            tr.col = atoi(ezxml_child(towerRoom, "col")->txt);

            //printf("antes de placeroom\n");
            placeRoom(tf, &tr, tr.row, tr.col);
        }
    }

    //printf("Fim de loadGameStage, numero de salas: %d\n", (*tf)->roomList->size);

    return 1;
}
