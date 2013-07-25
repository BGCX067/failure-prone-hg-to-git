#ifndef COLLADALOADER_H
#define COLLADALOADER_H

#include "../renderer/scene.h"


/**
 * 1- Algumas tags do arquivo são modificadas para facilitar a leitura, como
 * nome do material em <triangles>
 * 2- Não está lendo luz corretamente
 * 3- Ainda não lê scenegraph
 */

Scene* ReadColladaFile(const char *filename);

#endif
