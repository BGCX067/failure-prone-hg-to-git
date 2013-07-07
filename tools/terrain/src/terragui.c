#include "terragui.h"



GUINode* InitGUINode(){
	GUINode* guinode = malloc(sizeof(GUINode));
	if (!guinode)
		return NULL;
	guinode->r.x =  200;
	guinode->r.y = 200;
	guinode->r.w = 250;
	guinode->r.h = 150;
	guinode->inputs[0] = NULL;
	guinode->inputs[1] = NULL;
	guinode->menux = 0;
	guinode->menuy = 0;
	return guinode;
}

static int init = 0;
int comboselected = 0;
int combostate = 0;
int inputid = 0;
int outputid = 0;
GUINode* nodes[100];


void DrawGUINode(){

	if (!init){
		for(int i = 0; i < 100; i++)
			nodes[i] = NULL;
 		TerrainParam* node = malloc(sizeof(TerrainParam));
    		node->inputs[0] = NULL;
		node->inputs[1] = NULL;
		node->type = ROOT;
		GUINode* guinode = InitGUINode();
		guinode->r.w = 105;
		guinode->n = node;
		guinode->r.x = 600;
		nodes[0] = guinode;
		init = 1;
	}

    	rect r5, r6, r7, r8, r9, r10, r11;
	char* options[] = { "Noise", "Add", "Mul", "Min", "Max", "Mix" };
	r10.x = 50;
	r10.y = 910;
        doComboBox(10, &r10, 6, options, &comboselected, &combostate );
	r11.x = 120;
	r11.y = 910;
	if (doButton(11, &r11, "Add Node")) {
		if ( comboselected == 0){
    			TerrainParam* node3 = InitNoiseParam(512, 512, 0.1, 0.2, 0.8, 0.6 );
			GUINode* guinode = InitGUINode();
			guinode->n = node3;
			for (int i = 0; i < 100; i++)
				if (nodes[i] == NULL){
					nodes[i] = guinode;
					break;
				}
		
		} else if (comboselected == 1 || comboselected == 2 || comboselected == 3
				|| comboselected == 4 || comboselected == 5 ) {
			TerrainParam* node3 = InitTerrainParam(ADD);
			GUINode* guinode = InitGUINode();
			guinode->r.w = 110;
			guinode->r.h = 100;
			guinode->n = node3;
			for (int i = 0; i < 100; i++)
				if (nodes[i] == NULL){
					nodes[i] = guinode;
					break;
				}
		}
	}

	//desenha linhas
	for (int i = 0; i < 100; i++){
		if (nodes[i] != NULL){
			if (nodes[i]->inputs[0] != NULL){
				doLine( (int)(nodes[i]->r.x) + nodes[i]->menux, (int)(nodes[i]->r.y) + nodes[i]->menuy, (int)(nodes[i]->inputs[0]->r.x) + nodes[i]->inputs[0]->menux, (int)(nodes[i]->inputs[0]->r.y) + nodes[i]->inputs[0]->menuy );
			}
		}
	}

	for (int i = 0; i < 100; i++){
		if (nodes[i] != NULL) {
			if ( nodes[i]->n->type == NOISE){
				int x = nodes[i]->r.x;
				int y = nodes[i]->r.y;
				beginMenu(i+1000, x, y, nodes[i]->r.w, nodes[i]->r.h, &(nodes[i]->menux), &(nodes[i]->menuy), "Noise", NULL);
					doLabel(x+100, y+130, "Noise");
					doLabel(x+10, y+110, "Persistence");
					r6.x = x+110; r6.y = y+110;
					doHorizontalSlider(i+100, &r6, &(nodes[i]->n->persistence));
					doLabel(x+10, y+90, "Lacunarity");
					r7.x = x+110; r7.y = y+90;
					doHorizontalSlider(i+200, &r7, &(nodes[i]->n->lacunarity));
					doLabel(x+10, y+70, "Frequency");
					r8.x = x+110; r8.y = y+70;
					doHorizontalSlider(i+300, &r8, &(nodes[i]->n->frequency));
					doLabel(x+10, y+50, "Octaves");
					r9.x = x+110; r9.y = y+50;
					doHorizontalSlider(i+400, &r9, &(nodes[i]->n->octaves));
					r5.x = x+190; r5.y = y+10;
					if (doButton(i+500, &r5, "Out")){
						if (outputid == 0){
							outputid = i;
							if (inputid != 0){
								if (nodes[inputid]->n->inputs[0] == NULL){
									nodes[inputid]->n->inputs[0] = nodes[i]->n;
									nodes[inputid]->inputs[0] = nodes[i];
								}else{
									nodes[inputid]->n->inputs[1] = nodes[i]->n;
									nodes[inputid]->inputs[0] = nodes[i];
								}
								inputid = 0;
								outputid = 0;
							}
						}
					}
				endMenu(i+1000, x, y, nodes[i]->r.w, nodes[i]->r.h, &(nodes[i]->menux), &(nodes[i]->menuy));
			}
			else if (nodes[i]->n->type == ADD ){
				int x = nodes[i]->r.x;
				int y = nodes[i]->r.y;
				beginMenu(i+1000, x, y, nodes[i]->r.w, nodes[i]->r.h, &(nodes[i]->menux), &(nodes[i]->menuy), "Root", NULL);
					doLabel(x+40, y+80, "ADD");
					//doLabel(x+10, y+110, "Geometry");
					r6.x = x+10; r6.y = y+50;
					if (doButton(i+600, &r6, "In")){
						if (inputid == 0){
							inputid = i;
							if (outputid != 0){
								nodes[i]->n->inputs[0] = nodes[outputid]->n;
								nodes[i]->inputs[0] = nodes[outputid];
							}
						}
					}
					r6.x = x+60; r6.y = y +50;
					doButton(i + 700, &r6, "Out");
				endMenu(i+1000, x, y, nodes[i]->r.w, nodes[i]->r.h, &(nodes[i]->menux), &(nodes[i]->menuy));
				


			} else if (nodes[i]->n->type == ROOT) {
				int x = nodes[i]->r.x;
				int y = nodes[i]->r.y;
				beginMenu(i+1000, x, y, nodes[i]->r.w, nodes[i]->r.h, &(nodes[i]->menux), &(nodes[i]->menuy), "Root", NULL);
					doLabel(x+40, y+130, "Root");
					//doLabel(x+10, y+110, "Geometry");
					r6.x = x+10; r6.y = y+100;
					if (doButton(i+800, &r6, "Geometry")){
						if (inputid == 0){
							inputid = i;
							if (outputid != 0){
								nodes[i]->n->inputs[0] = nodes[outputid]->n;
								nodes[i]->inputs[0] = nodes[outputid];
							}
						}
					}
					r6.x = x+10; r6.y = y + 70;
					doButton(i+900, &r6, "Material");
					r6.x = x+20; r6.y = y + 40;
					if (doButton(i + 1100, &r6, "Apply")){
						generateTerrain(nodes[0]->n, 512,512);
					}
				endMenu(i+1000, x, y, nodes[i]->r.w, nodes[i]->r.h, &(nodes[i]->menux), &(nodes[i]->menuy));
			}
		}
	}
} 
