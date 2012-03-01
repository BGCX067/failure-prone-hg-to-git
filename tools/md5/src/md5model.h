/*
 * md5model.h -- md5mesh model loader + animation
 * last modification: aug. 14, 2007
 *
 * Copyright (c) 2005-2007 David HENRY
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * gcc -Wall -ansi -lGL -lGLU -lglut md5anim.c md5anim.c -o md5model
 */

#ifndef __MD5MODEL_H__
#define __MD5MODEL_H__

#include "mesh.h"
#include "math/boundingbox.h"
#include "math/quaternion.h"
//Joint
typedef struct {
  char name[64];
  int parent;

  vec3 pos;
  quaternion orient;
} md5_joint_t;


//Animation data 
typedef struct
{
  int num_frames;
  int num_joints;
  int frameRate;

  md5_joint_t **skelFrames;
  BoundingBox *bboxes;

  Mesh *mesh;
} md5_anim_t;

/* Vertex */
typedef struct 
{
  float s, t;

  int start; /* start weight */
  int count; /* weight count */
}md5_vertex_t;

/* Triangle */
typedef struct 
{
  int index[3];
}md5_triangle_t;

/* Weight */
typedef struct
{
  int joint;
  float bias;

  vec3 pos;
}md5_weight_t;


/* MD5 mesh */
typedef struct
{
    int num_verts;
    int num_tris;
    int num_weights;

    char shader[256];

    md5_weight_t *weights;
    md5_triangle_t *triangles;
    md5_vertex_t *vertices;
}md5_mesh_t;

/* MD5 model structure */
typedef struct
{
  md5_joint_t *baseSkel;
  md5_mesh_t *meshes;

  int num_joints;
  int num_meshes;
} md5_model_t;



/**
 * md5mesh prototypes
 */
md5_model_t* ReadMD5Model(const char *filename);
int ReadMD5Anim(const char *filename, md5_anim_t *anim);
Mesh* prepareMD5Mesh(md5_model_t *mdl, md5_joint_t *skeleton);


//void FreeModel (struct md5_model_t *mdl);
//void PrepareMesh (const struct md5_mesh_t *mesh, const struct md5_joint_t *skeleton);
//void AllocVertexArrays ();
//void FreeVertexArrays ();
//void DrawSkeleton (const struct md5_joint_t *skeleton, int num_joints);

/**
 * md5anim prototypes
 */
/*int CheckAnimValidity (const struct md5_model_t *mdl,
		       const struct md5_anim_t *anim);
int ReadMD5Anim (const char *filename, struct md5_anim_t *anim);
void FreeAnim (struct md5_anim_t *anim);
void InterpolateSkeletons (const struct md5_joint_t *skelA,
			   const struct md5_joint_t *skelB,
			   int num_joints, float interp,
			   struct md5_joint_t *out);
void Animate (const struct md5_anim_t *anim,
	      struct anim_info_t *animInfo, double dt);*/

#endif /* __MD5MODEL_H__ */
