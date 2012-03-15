/*
 * md5mesh.c -- md5mesh model loader + animation
 * last modification: aug. 14, 2007
 *
 * Doom3's md5mesh viewer with animation.  Mesh portion.
 * Dependences: md5model.h, md5anim.c.
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

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "md5model.h"
#include "math/vec3.h"
#include "math/boundingbox.h"
#include "math/quaternion.h"
#include "renderer.h"

typedef struct {
  char name[64];
  int parent;
  int flags;
  int startIndex;
}joint_info_t;

/* Base frame joint */
typedef struct {
  vec3 pos;
  quaternion orient;
}baseframe_joint_t;

//Joint
typedef struct {
  int parent;
  vec3 pos;
  quaternion orient;
  char name[64];
} md5_joint_t;


//Animation data 
typedef struct {
  int num_frames;
  int num_joints;
  int frameRate;

  md5_joint_t **skelFrames;
  BoundingBox *bboxes;
} md5_anim_t;

/* Vertex */
typedef struct  {
  float s, t;

  int start; /* start weight */
  int count; /* weight count */
}md5_vertex_t;

/* Triangle */
typedef struct  {
  int index[3];
}md5_triangle_t;

/* Weight */
typedef struct {
  int joint;
  float bias;

  vec3 pos;
}md5_weight_t;


/* MD5 mesh */
typedef struct {
    int num_verts;
    int num_tris;
    int num_weights;

    char shader[256];

    md5_weight_t *weights;
    md5_triangle_t *triangles;
    md5_vertex_t *vertices;
}md5_mesh_t;

/* MD5 model structure */
typedef struct {
  md5_joint_t *baseSkel;
  md5_mesh_t *meshes;

  int num_joints;
  int num_meshes;
} md5_model_t;

/**
 * Load an MD5 model from file.
 */
static md5_model_t* ReadMD5Model(const char *filename) {
    md5_model_t *mdl = malloc(sizeof(md5_model_t));
    FILE *fp;
    char buff[512];
    int version;
    int curr_mesh = 0;
    int i;

    fp = fopen (filename, "rb");
    if (!fp) {
        fprintf (stderr, "Error: couldn't open \"%s\"!\n", filename);
        return 0;
    }

    while (!feof (fp)) {
        /* Read whole line */
        fgets (buff, sizeof (buff), fp);

        if (sscanf (buff, " MD5Version %d", &version) == 1) {
            if (version != 10) {
                /* Bad version */
                fprintf (stderr, "Error: bad model version\n");
                fclose (fp);
                return 0;
            }
        }
        else if (sscanf (buff, " numJoints %d", &mdl->num_joints) == 1) {
            if (mdl->num_joints > 0) {
                /* Allocate memory for base skeleton joints */
                mdl->baseSkel = calloc(mdl->num_joints, sizeof (md5_joint_t));
            }
        }
        else if (sscanf (buff, " numMeshes %d", &mdl->num_meshes) == 1) {
            if (mdl->num_meshes > 0) {
                /* Allocate memory for meshes */
                mdl->meshes = calloc (mdl->num_meshes, sizeof (md5_mesh_t));
            }
        }
        else if (strncmp (buff, "joints {", 8) == 0) {
            /* Read each joint */
            for (i = 0; i < mdl->num_joints; ++i) {
                md5_joint_t *joint = &mdl->baseSkel[i];

                /* Read whole line */
                fgets (buff, sizeof (buff), fp);

                if (sscanf (buff, "%s %d ( %f %f %f ) ( %f %f %f )",
                            joint->name, &joint->parent, &joint->pos[0],
                            &joint->pos[1], &joint->pos[2], &joint->orient[0],
                            &joint->orient[1], &joint->orient[2]) == 8) {
                    /* Compute the w component */
                    quatComputeAngle(joint->orient);
                }
            }
        }
        else if (strncmp (buff, "mesh {", 6) == 0) {
            md5_mesh_t *mesh = &mdl->meshes[curr_mesh];
            int vert_index = 0;
            int tri_index = 0;
            int weight_index = 0;
            float fdata[4];
            int idata[3];

            while ((buff[0] != '}') && !feof (fp)) {
                /* Read whole line */
                fgets (buff, sizeof (buff), fp);

                if (strstr (buff, "shader ")) {
                    int quote = 0, j = 0;

                    /* Copy the shader name whithout the quote marks */
                    for (i = 0; i < sizeof (buff) && (quote < 2); ++i) {
                        if (buff[i] == '\"')
                            quote++;

                        if ((quote == 1) && (buff[i] != '\"')) {
                            mesh->shader[j] = buff[i];
                            j++;
                        }
                    }
                }
                else if (sscanf (buff, " numverts %d", &mesh->num_verts) == 1) {
                    if (mesh->num_verts > 0) {
                        /* Allocate memory for vertices */
                        mesh->vertices = malloc (sizeof (md5_vertex_t) * mesh->num_verts);
                    }
                }
                else if (sscanf (buff, " numtris %d", &mesh->num_tris) == 1) {
                    if (mesh->num_tris > 0) {
                        /* Allocate memory for triangles */
                        mesh->triangles = malloc(sizeof(md5_triangle_t)*mesh->num_tris);
                    }
                }
                else if (sscanf (buff, " numweights %d", &mesh->num_weights) == 1) {
                    if (mesh->num_weights > 0) {
                        /* Allocate memory for vertex weights */
                        mesh->weights = malloc (sizeof(md5_weight_t)*mesh->num_weights);
                    }
                }
                else if (sscanf (buff, " vert %d ( %f %f ) %d %d", &vert_index,
                            &fdata[0], &fdata[1], &idata[0], &idata[1]) == 5) {
                    /* Copy vertex data */
                    mesh->vertices[vert_index].s = fdata[0];
                    mesh->vertices[vert_index].t = fdata[1];
                    mesh->vertices[vert_index].start = idata[0];
                    mesh->vertices[vert_index].count = idata[1];
                }
                else if (sscanf (buff, " tri %d %d %d %d", &tri_index,
                            &idata[0], &idata[1], &idata[2]) == 4) {
                    /* Copy triangle data */
                    mesh->triangles[tri_index].index[0] = idata[0];
                    mesh->triangles[tri_index].index[1] = idata[1];
                    mesh->triangles[tri_index].index[2] = idata[2];
                }
                else if (sscanf (buff, " weight %d %d %f ( %f %f %f )",
                            &weight_index, &idata[0], &fdata[3],
                            &fdata[0], &fdata[1], &fdata[2]) == 6) {
                    /* Copy vertex data */
                    mesh->weights[weight_index].joint = idata[0];
                    mesh->weights[weight_index].bias = fdata[3];
                    mesh->weights[weight_index].pos[0] = fdata[0];
                    mesh->weights[weight_index].pos[1] = fdata[1];
                    mesh->weights[weight_index].pos[2] = fdata[2];
                }
            }

            curr_mesh++;
        }
    }

    fclose (fp);
    
    return mdl;
}

static void BuildFrameSkeleton(const joint_info_t *jointInfos, 
                               const baseframe_joint_t *baseFrame,
		                       const float *animFrameData, md5_joint_t *skelFrame, 
                               int num_joints)
{
    for (int i = 0; i < num_joints; ++i) {
        const baseframe_joint_t *baseJoint = &baseFrame[i];
        vec3 animatedPos;
        quaternion animatedOrient;
        int j = 0;

        memcpy(animatedPos, baseJoint->pos, sizeof (vec3));
        memcpy(animatedOrient, baseJoint->orient, sizeof (quaternion));

        if (jointInfos[i].flags & 1) { /* Tx */
            animatedPos[0] = animFrameData[jointInfos[i].startIndex + j];
            ++j;
        }

        if (jointInfos[i].flags & 2) { /* Ty */
            animatedPos[1] = animFrameData[jointInfos[i].startIndex + j];
            ++j;
        }

        if (jointInfos[i].flags & 4) { /* Tz */
            animatedPos[2] = animFrameData[jointInfos[i].startIndex + j];
            ++j;
        }

        if (jointInfos[i].flags & 8) { /* Qx */
            animatedOrient[0] = animFrameData[jointInfos[i].startIndex + j];
            ++j;
        }

        if (jointInfos[i].flags & 16) { /* Qy */
            animatedOrient[1] = animFrameData[jointInfos[i].startIndex + j];
            ++j;
        }

        if (jointInfos[i].flags & 32) { /* Qz */
            animatedOrient[2] = animFrameData[jointInfos[i].startIndex + j];
            ++j;
        }

        /* Compute orient quaternion's w value */
        quatComputeAngle(animatedOrient);

        /* NOTE: we assume that this joint's parent has
           already been calculated, i.e. joint's ID should
           never be smaller than its parent ID. */
        md5_joint_t *thisJoint = &skelFrame[i];

        int parent = jointInfos[i].parent;
        thisJoint->parent = parent;
        strcpy (thisJoint->name, jointInfos[i].name);

        /* Has parent? */
        if (thisJoint->parent < 0) {
            memcpy(thisJoint->pos, animatedPos, sizeof (vec3));
            memcpy(thisJoint->orient, animatedOrient, sizeof (quaternion));
        } else {
            md5_joint_t *parentJoint = &skelFrame[parent];
            vec3 rpos; /* Rotated position */

            /* Add positions */
            rotateVec(animatedPos, parentJoint->orient, rpos);
            thisJoint->pos[0] = rpos[0] + parentJoint->pos[0];
            thisJoint->pos[1] = rpos[1] + parentJoint->pos[1];
            thisJoint->pos[2] = rpos[2] + parentJoint->pos[2];

            /* Concatenate rotations */
            quatMult(parentJoint->orient, animatedOrient, thisJoint->orient);
            quatNormalize(thisJoint->orient);
        }
    }
}

static int ReadMD5Anim(const char *filename, md5_anim_t *anim) {
    FILE *fp = NULL;
    char buff[512];
    joint_info_t *jointInfos = NULL;
    baseframe_joint_t *baseFrame = NULL;
    float *animFrameData = NULL;
    int version;
    int numAnimatedComponents;
    int frame_index;
    int i;

    fp = fopen(filename, "rb");
    if (!fp) {
        fprintf (stderr, "error: couldn't open \"%s\"!\n", filename);
        return 0;
    }

    while(!feof (fp)) {
        /* Read whole line */
        fgets (buff, sizeof (buff), fp);

        if (sscanf (buff, " MD5Version %d", &version) == 1) {
            if (version != 10) {
                /* Bad version */
                fprintf (stderr, "Error: bad animation version\n");
                fclose (fp);
                return 0;
            }
        }
        else if (sscanf (buff, " numFrames %d", &anim->num_frames) == 1) {
            /* Allocate memory for skeleton frames and bounding boxes */
            if (anim->num_frames > 0) {
                anim->skelFrames = malloc (sizeof(md5_joint_t*) * anim->num_frames);
                //anim->bboxes = malloc (sizeof(md5_bbox_t) * anim->num_frames);
                anim->bboxes = malloc(sizeof(BoundingBox)*anim->num_frames);
            }
        }
        else if (sscanf (buff, " numJoints %d", &anim->num_joints) == 1) {
            if (anim->num_joints > 0) {
                for (i = 0; i < anim->num_frames; ++i) {
                    /* Allocate memory for joints of each frame */
                    anim->skelFrames[i] = malloc (sizeof(md5_joint_t)*anim->num_joints);
                }

                /* Allocate temporary memory for building skeleton frames */
                jointInfos = malloc (sizeof (joint_info_t) * anim->num_joints);
                baseFrame = malloc (sizeof (baseframe_joint_t) * anim->num_joints);
            }
        }
        else if (sscanf (buff, " frameRate %d", &anim->frameRate) == 1) {
            /*
               printf ("md5anim: animation's frame rate is %d\n", anim->frameRate);
               */
        }
        else if (sscanf (buff, " numAnimatedComponents %d", &numAnimatedComponents) == 1) {
            if (numAnimatedComponents > 0) {
                /* Allocate memory for animation frame data */
                animFrameData = (float *)malloc (sizeof (float) * numAnimatedComponents);
            }
        }
        else if (strncmp (buff, "hierarchy {", 11) == 0) {
            for (i = 0; i < anim->num_joints; ++i) {
                /* Read whole line */
                fgets (buff, sizeof (buff), fp);

                /* Read joint info */
                sscanf(buff, " %s %d %d %d", jointInfos[i].name, &jointInfos[i].parent,
                       &jointInfos[i].flags, &jointInfos[i].startIndex);
            }
        }
        else if (strncmp (buff, "bounds {", 8) == 0) {
            for (i = 0; i < anim->num_frames; ++i) {
                /* Read whole line */
                fgets (buff, sizeof (buff), fp);

                /* Read bounding box */
                sscanf (buff, " ( %f %f %f ) ( %f %f %f )",
                        &anim->bboxes[i].pmin[0], &anim->bboxes[i].pmin[1],
                        &anim->bboxes[i].pmin[2], &anim->bboxes[i].pmax[0],
                        &anim->bboxes[i].pmax[1], &anim->bboxes[i].pmax[2]);
            }
        }
        else if (strncmp (buff, "baseframe {", 10) == 0) {
            for (i = 0; i < anim->num_joints; ++i) {
                /* Read whole line */
                fgets (buff, sizeof (buff), fp);

                /* Read base frame joint */
                if (sscanf (buff, " ( %f %f %f ) ( %f %f %f )",
                            &baseFrame[i].pos[0], &baseFrame[i].pos[1],
                            &baseFrame[i].pos[2], &baseFrame[i].orient[0],
                            &baseFrame[i].orient[1], &baseFrame[i].orient[2]) == 6)
                {
                    /* Compute the w component */
                    quatComputeAngle(baseFrame[i].orient);
                }
            }
        }
        else if (sscanf (buff, " frame %d", &frame_index) == 1) {
            /* Read frame data */
            for (i = 0; i < numAnimatedComponents; ++i)
                fscanf (fp, "%f", &animFrameData[i]);

            /* Build frame skeleton from the collected data */
            BuildFrameSkeleton(jointInfos, baseFrame, animFrameData,
                    anim->skelFrames[frame_index], anim->num_joints);
        }
    }

    fclose (fp);

    /* Free temporary data allocated */
    if (animFrameData)
        free(animFrameData);

    if (baseFrame)
        free(baseFrame);

    if (jointInfos)
        free(jointInfos);

    return 1;
}


static Mesh* prepareMD5Mesh(md5_model_t *mdl, md5_joint_t *skeleton) {
    Mesh *m = initMesh();
    //Transformar pra mesh da engine
    for(int i = 0; i < mdl->num_meshes; i++) {
        Triangles *t = addTris(m);
        md5_mesh_t *md5mesh = &mdl->meshes[i];
        t->numweights = md5mesh->num_weights;
        t->weights = malloc(sizeof(Weight)*t->numweights);
        for(int j = 0; j < t->numweights; j++) {
            t->weights[j].joint = md5mesh->weights[j].joint;
            t->weights[j].factor = md5mesh->weights[j].bias;
            t->weights[j].pos[0] = md5mesh->weights[j].pos[0];
            t->weights[j].pos[1] = md5mesh->weights[j].pos[1];
            t->weights[j].pos[2] = md5mesh->weights[j].pos[2];
        }

        //Indices     
        unsigned int *tIndices = malloc(sizeof(unsigned int)*3*md5mesh->num_tris);
        for(int j = 0; j < md5mesh->num_tris; j++) {
            tIndices[3*j] = md5mesh->triangles[j].index[0];
            tIndices[3*j + 1] = md5mesh->triangles[j].index[1];
            tIndices[3*j + 2] = md5mesh->triangles[j].index[2];
        }
        addIndices(t, 3*md5mesh->num_tris, tIndices);
        //Vertices e TexCoords
        float *tVerts = malloc(sizeof(float)*md5mesh->num_verts*3); 
        float *tTexCoords = malloc(sizeof(float)*md5mesh->num_verts*2);

        t->weightInfo = malloc(sizeof(VertexWeightInfo)*md5mesh->num_verts);
        for(int j = 0; j < md5mesh->num_verts; j++) {
            tTexCoords[2*j] = md5mesh->vertices[j].s;
            tTexCoords[2*j + 1] = md5mesh->vertices[j].t;
            tVerts[3*j] = 0.0;
            tVerts[3*j + 1] = 0.0;
            tVerts[3*j + 2] = 0.0;

            t->weightInfo[j].start = md5mesh->vertices[j].start;
            t->weightInfo[j].count = md5mesh->vertices[j].count;
            for(int k = 0; k < t->weightInfo[j].count; k++) {
                Weight *w = &t->weights[t->weightInfo[j].start + k];
                Joint *joint = &skeleton[w->joint];

                vec3 wv;
                rotateVec(w->pos, joint->orientation, wv);
                tVerts[3*j] += (joint->pos[0]+ wv[0])*w->factor;
                tVerts[3*j + 1] += (joint->pos[1]+ wv[1])*w->factor;
                tVerts[3*j + 2] += (joint->pos[2]+ wv[2])*w->factor;
            }
        }
        setboundingbox(&t->b, tVerts, md5mesh->num_verts);
        addVertices(t, md5mesh->num_verts*3, 3, tVerts);
        addTexCoords(t, md5mesh->num_verts*2, 2, 0, tTexCoords);
        float *tNormals = malloc(sizeof(float)*md5mesh->num_verts*3); 
        memset(tNormals, 0, sizeof(float)*md5mesh->num_verts*3);
        setNormals(tIndices, tVerts, tNormals, md5mesh->num_tris, md5mesh->num_verts);
        addNormals(t, md5mesh->num_verts*3, 3, tNormals);
        //TODO material
    }
    prepareMesh(m);
    return m;
}

Mesh* readMD5Mesh(const char *filename) {
    md5_model_t *mdl = ReadMD5Model(filename);

    //Inicializar Mesh
    Mesh *m = prepareMD5Mesh(mdl, mdl->baseSkel);

    //Liberar memória do mdl
    if(mdl) {
        if(mdl->meshes) {
            if(mdl->meshes->weights)
                free(mdl->meshes->weights);
            if(mdl->meshes->triangles)
                free(mdl->meshes->triangles);
            if(mdl->meshes->vertices)
                free(mdl->meshes->vertices);
            free(mdl->meshes);
        }
        if(mdl->baseSkel)
            free(mdl->baseSkel);
        free(mdl);
    }
    return m;
}

SkeletalAnim* readMD5Anim(const char *filename) {
    md5_anim_t md5anim;
    ReadMD5Anim(filename, &md5anim);

    SkeletalAnim *anim = malloc(sizeof(SkeletalAnim));

    //Converter pra anim da engine
    anim = malloc(sizeof(SkeletalAnim));
    anim->numFrames = md5anim.num_frames;
    anim->numJoints = md5anim.num_joints;
    anim->frameRate = md5anim.frameRate;

    anim->skelFrames = malloc(sizeof(Joint*)*anim->numFrames);
    for(int i = 0; i < anim->numFrames; i++) {
        anim->skelFrames[i] = malloc(sizeof(Joint)*anim->numJoints);
        for(int j = 0; j < anim->numJoints; j++) {
            anim->skelFrames[i][j].parent = md5anim.skelFrames[i][j].parent;
            //Copia pos
            memcpy(anim->skelFrames[i][j].pos, md5anim.skelFrames[i][j].pos, sizeof(vec3));
            //Copia orientação
            memcpy(anim->skelFrames[i][j].orientation, md5anim.skelFrames[i][j].orient, sizeof(quaternion));
        }
    }

    return anim;
}
