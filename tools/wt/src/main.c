#include <stdio.h>
#include <stdlib.h>
#include "glapp.h"
#include "math/vec3.h"
#include "renderer/renderer.h"
#include "renderer/scene.h"
#include "renderer/mesh.h"
#include "renderer/camera.h"
#include "renderer/gui.h"
#include "tower.h"
#include "math/util.h"
#include "companion.h"
#include "util/utlist.h"

Scene* cena;
renderer *mainrenderer;
Camera c;


Tower *tower;
int currFloor = 0;
TowerRoom *tr;
int numRooms;

Character *chars = NULL;

//GUI-related
int menux, menuy;
char** salasComboBox;//[] = {"Sala 1", "Sala 2", "Sala 3", "Sala 4"};
int comboboxState = 0;
int currRoom = 0;
int prevRoom = 0;

//------
int rowsCBoxState = 0, colsCBoxState = 0;
int rowsNewValue = 0, colsNewValue = 0;

void initializeGame(){
    tower = initTower();
    if(!loadRoomFile("data/roomtest.xml", &numRooms, &tr)) {
        printf("Nao foi possivel ler o arquivo de salas\n");
        exit(0);
    }
    salasComboBox = malloc(sizeof(char*)*numRooms);
    for(int i = 0; i < numRooms; i++){
        salasComboBox[i] = tr[i].name;
//	salasComboBox[i] = malloc(sizeof(wchar_t)*strlen(tr[i].name));
//	mbstowcs(salasComboBox[i], tr[i].name, sizeof(wchar_t)*strlen(tr[i].name));
    }

    menux = menuy = 0;
    initializeGUI(800, 600);
    initCamera(&c, TRACKBALL);
   
    TowerFloor *tf = addFloor(tower, 6, 6);
    cena = initializeScene();
    addMesh(cena, tf->m);

	loadNames();
	//gera os goons iniciais
	Character* ch = generateCharacter(HANDMAIDEN, "germanf");
    DL_APPEND(chars, ch);
	ch = generateCharacter(GUARD, "germanm");
    DL_APPEND(chars, ch);

    camerafit(&c, cena->b, 45.0, 800.0f/600.0f, 0.1, 1000.0);
}

float menux1, menux2 = 0;
int gamebutton = 0;

int Render(event *e, double* dt){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Mesh *it;
    DL_FOREACH(cena->meshList, it) {
        bindShader(it->material->shdr);
        drawIndexedVAO(it->vaoId, it->indicesCount, GL_TRIANGLES);
    }

    int newRoom = prevRoom;
    rect r1,  r2, r3, r4, r5, r6, r7, r8, r9;
    beginGUI(e);
	r1.x = 10; r1.y = 575;
	r2.x = 60; r2.y = 575;
	r3.x = 120; r3.y = 575;
	r4.x = 220; r4.y = 575;
	r5.x = 285; r5.y = 575;

	doToggleButton(1, &r1, "Game", &gamebutton);
	doToggleButton(2, &r2, "Tower", &gamebutton);
	doToggleButton(3, &r3, "Characters", &gamebutton);
	doToggleButton(4, &r4, "Agenda", &gamebutton);
	doToggleButton(5, &r5, "Spells", &gamebutton);

	if (gamebutton == 1 ) {
		beginMenu(6, 10, 223, 250, 350, &menux1, &menux2, "Game", NULL);
			r6.x = 100; r6.y = 540;
        	if(doButton(7, &r6, "Save"))
                saveGameState(tower, "teste.xml");
			r7.x = 100; r7.y = 500;
            if(doButton(8, &r7, "Load")) {
                if(currFloor != -1) {
                    TowerFloor *tf = *((TowerFloor**)utarray_eltptr(tower->floorList, currFloor));
                    rmMesh(cena, tf->m);
                    utarray_clear(tower->floorList); 
                }
                loadGameState(tower, "teste.xml");
                if(utarray_len(tower->floorList) > 0) {
                    currFloor = 0;
                    TowerFloor *tf = *((TowerFloor**)utarray_front(tower->floorList));
                    addMesh(cena, tf->m);
                    updateMeshColors(tf->m, tf->colors);
                } else
                    currFloor = -1;
            }
			r8.x = 83; r8.y = 460;
			if (doButton(9, &r8, "End Game")){

			}
			r9.x = 100; r9.y = 420;
			if (doButton(10, &r9, "Quit")){

			}
			//resolution, sound on/off, sound volume, tooltips, rendering quality		
		endMenu(6, 10, 223, 250, 150, &menux1, &menux2);
	}else if (gamebutton == 2 ) {
		beginMenu(6, 60, 223, 250, 350, &menux1, &menux2, "Tower", NULL);
 			doLabel(80, 543, "Sala");
        	r7.x = 120; r7.y = 540;
        	doComboBox(5, &r7, numRooms, salasComboBox, &newRoom, &comboboxState);
            
            char currFloorStr[16];
            sprintf(currFloorStr, "Floor: %d", currFloor);
            doLabel(80, 487, currFloorStr);
            rect r10, r11, r12, r13, r14, r15;
            r10.x = 190; r10.y = 510;
            if(doButton(11, &r10, "Next Floor")) {
                if(currFloor < utarray_len(tower->floorList) - 1 && utarray_len(tower->floorList)> 0) {
                    TowerFloor *tf = *((TowerFloor**)utarray_eltptr(tower->floorList, currFloor));
                    rmMesh(cena, tf->m);
                    currFloor++;
                    tf = *((TowerFloor**)utarray_eltptr(tower->floorList, currFloor));
                    addMesh(cena, tf->m);
                    updateMeshColors(tf->m, tf->colors);
                }
            }
            r11.x = 190; r11.y = 480;
            if(doButton(12, &r11, "Prev Floor")) {
                if(currFloor > 0) {
                    TowerFloor *tf = *((TowerFloor**)utarray_eltptr(tower->floorList, currFloor));
                    rmMesh(cena, tf->m);
                    currFloor--;
                    tf = *((TowerFloor**)utarray_eltptr(tower->floorList, currFloor));
                    addMesh(cena, tf->m);
                    updateMeshColors(tf->m, tf->colors);
                }
            }
            r15.x = 190; r15.y = 450;
            if(doButton(16, &r15, "Remove Floor")) {
                if(currFloor != -1) {
                    TowerFloor *tf = *((TowerFloor**)utarray_eltptr(tower->floorList, currFloor));
                    rmMesh(cena, tf->m);
                    utarray_erase(tower->floorList, currFloor, 1);
                    currFloor--;
                }
                
                if(currFloor != -1) {
                    TowerFloor *tf = *((TowerFloor**)utarray_eltptr(tower->floorList, currFloor));
                    addMesh(cena, tf->m);
                    updateMeshColors(tf->m, tf->colors);
                }
            }
            doLabel(80, 420, "Rows");
            doLabel(80, 390, "Cols");
            const char *nums[] = {"1", "2", "3", "4", "5", "6"};
            r12.x = 120; r12.y = 415;
        	doComboBox(13, &r12, 6, nums, &rowsNewValue, &rowsCBoxState);
            r13.x = 120; r13.y = 385;
        	doComboBox(14, &r13, 6, nums, &colsNewValue, &colsCBoxState);
            r14.x = 190; r14.y = 400;
            if(doButton(15, &r14, "Add Floor")) {
                TowerFloor *tf = addFloor(tower, rowsNewValue + 1, colsNewValue + 1);
                if(currFloor == -1) {
                    addMesh(cena, tf->m);
                    currFloor = 0;
                }
            }
            //printf("rowsNewValue: %d\n", rowsNewValue);
		endMenu(6, 60, 223, 250, 150, &menux1, &menux2);
	} else if (gamebutton == 3){
		beginMenu(6, 120, 223, 250, 350, &menux1, &menux2, "Characters", NULL);
			doLabel(180, 543, "Tower characters");
			float posy = 523;
            Character *it;
            DL_FOREACH(chars, it) {
                doLabel(160, posy, it->name);
				posy -= 20;
            }
		endMenu(6, 120, 223, 250, 150, &menux1, &menux2);
	} else if (gamebutton == 4){
		beginMenu(6, 220, 223, 250, 350, &menux1, &menux2, "Agenda", NULL);

		endMenu(6, 220, 223, 250, 150, &menux1, &menux2);
	} else if (gamebutton == 5){
		beginMenu(6, 285, 223, 250, 350, &menux1, &menux2, "Grimorio", NULL);

		endMenu(6, 285, 223, 250, 150, &menux1, &menux2);
	}
    endGUI();

    if(currRoom != newRoom) {
        //So pra constar
        prevRoom = currRoom;
        currRoom = newRoom;
        printf("prevRoom: %d\ncurrRoom: %d\n", prevRoom, currRoom);
        prevRoom = currRoom;
    }

    glFlush();

    return 1;
}

int Update(event* e, double *dt){
    cameraHandleEvent(&c, e);
    setupViewMatrix(&c);
    vec3 bboxcenter;
    bbcenter(cena->b, bboxcenter);
    //translada para o centro
    fptranslatef(c.modelview, -bboxcenter[0], -bboxcenter[1], -bboxcenter[2]);
    setView(c.modelview);
    setProjection(c.projection); //TODO isso so precisaria ser calculado/setado 1x
    
    if(currFloor != -1) {
        TowerFloor *tf = *((TowerFloor**)utarray_eltptr(tower->floorList, currFloor));
        //Volta a cor das salas que estão livres
        for(unsigned int i = 0; i < tf->numrows; i++)
            for(unsigned int j = 0; j < tf->numcols; j++)
                if(tf->grid[i][j] == 0) {
                    int currtile = j*tf->numrows + i;
                    for(int k = 0; k < 6; k++) {
                        tf->colors[3*(6*currtile + k)] = 0.0;
                        tf->colors[3*(6*currtile + k) + 1] = 1.0;
                        tf->colors[3*(6*currtile + k) + 2] = 0.0;
                    }
                }
        updateMeshColors(tf->m, tf->colors);

        int tempViewport[4] = {0, 0, mainrenderer->viewPortWidth, mainrenderer->viewPortHeight};
        int indices[3];
        if(picking(e->x, mainrenderer->viewPortHeight - e->y, c.modelview, c.projection, tempViewport, tf->m, indices)) {
            unsigned int tilenum = indices[0]/6;
            if(testRoom(tf, &tr[currRoom], tilenum%tf->numcols, tilenum/tf->numrows)) {
                changeTileColor(tf, &tr[currRoom], tilenum%tf->numcols, tilenum/tf->numrows);
                updateMeshColors(tf->m, tf->colors);
                //Desocupar grid 
                int col = tilenum/tf->numrows;
                int row = tilenum%tf->numcols;
                //Verificar se algum tile já está ocupado
                for(unsigned int i = 0; i < tr[currRoom].numrows; i++)
                    for(unsigned int j = 0; j < tr[currRoom].numcols; j++)
                        tf->grid[row + i][col + j] = 0;
            }

        } 

        if(e->type & MOUSE_BUTTON_PRESS && e->buttonLeft ) {
            if(picking(e->x, mainrenderer->viewPortHeight - e->y, c.modelview, c.projection, tempViewport, tf->m, indices)) {
                unsigned int tilenum = indices[0]/6;
                int row = tilenum%tf->numcols;
                int col = tilenum/tf->numrows;
                if(placeRoom(tf, &tr[currRoom], row, col))
                    updateMeshColors(tf->m, tf->colors);
                else
                    printf("Não é possível colocar a sala nessa posição (%d, %d)\n", row, col);
            }
        }
    }
    return 1;
}

int main(){
	setVideoMode(800, 600, 0);
	warpmouse(0);
	setWindowTitle("Wizard Tower");
	//initializeRenderer(1024, 768, 0.1, 10000.0, 45.0);
	mainrenderer = initializeRenderer(800, 600, 0.1, 10000.0, 45.0);
	srand(time(0));
	initializeGame();
	MainLoop();
	closeVideo();
	// deleta coisas
    Character *it, *tmp;
    DL_FOREACH_SAFE(chars, it, tmp)
        DL_DELETE(chars, it);
	return 0;
}
