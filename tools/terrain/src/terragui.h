#ifndef _TERRAGUI_H_
#define _TERRAGUI_H_

#include "terrasys.h"
#include "renderer/gui.h"

typedef struct _NodeGui GUINode;

struct _NodeGui{
	TerrainParam* n;
	rect r;
	float menux, menuy;
	GUINode* inputs[2];
};

GUINode* InitGUINode();

void DrawGUINode();

#endif
