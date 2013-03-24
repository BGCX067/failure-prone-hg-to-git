#ifndef __MD5MODEL_H__
#define __MD5MODEL_H__

#include "mesh.h"

Mesh* readMD5Mesh(const char *filename);
SkeletalAnim* readMD5Anim(const char *filename);

#endif
