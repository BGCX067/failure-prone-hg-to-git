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




typedef struct
{
  char name[64];
  int parent;
  int flags;
  int startIndex;
}joint_info_t;

/* Base frame joint */
typedef struct
{
  vec3 pos;
  quaternion orient;
}baseframe_joint_t;



/**
 * Basic quaternion operations.
 */

/*
    void
Quat_multQuat (const quat4_t qa, const quat4_t qb, quat4_t out)
{
    out[W] = (qa[W] * qb[W]) - (qa[X] * qb[X]) - (qa[Y] * qb[Y]) - (qa[Z] * qb[Z]);
    out[X] = (qa[X] * qb[W]) + (qa[W] * qb[X]) + (qa[Y] * qb[Z]) - (qa[Z] * qb[Y]);
    out[Y] = (qa[Y] * qb[W]) + (qa[W] * qb[Y]) + (qa[Z] * qb[X]) - (qa[X] * qb[Z]);
    out[Z] = (qa[Z] * qb[W]) + (qa[W] * qb[Z]) + (qa[X] * qb[Y]) - (qa[Y] * qb[X]);
}

    void
Quat_multVec (const quat4_t q, const vec3_t v, quat4_t out)
{
    out[W] = - (q[X] * v[X]) - (q[Y] * v[Y]) - (q[Z] * v[Z]);
    out[X] =   (q[W] * v[X]) + (q[Y] * v[Z]) - (q[Z] * v[Y]);
    out[Y] =   (q[W] * v[Y]) + (q[Z] * v[X]) - (q[X] * v[Z]);
    out[Z] =   (q[W] * v[Z]) + (q[X] * v[Y]) - (q[Y] * v[X]);
}

    void
Quat_rotatePoint (const quat4_t q, const vec3_t in, vec3_t out)
{
    quat4_t tmp, inv, final;

    inv[X] = -q[X]; inv[Y] = -q[Y];
    inv[Z] = -q[Z]; inv[W] =  q[W];

    Quat_normalize (inv);

    Quat_multVec (q, in, tmp);
    Quat_multQuat (tmp, inv, final);

    out[X] = final[X];
    out[Y] = final[Y];
    out[Z] = final[Z];
}*/

/**
 * Load an MD5 model from file.
 */
md5_model_t* ReadMD5Model(const char *filename) {
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
    int i;

    for (i = 0; i < num_joints; ++i)
    {
        const baseframe_joint_t *baseJoint = &baseFrame[i];
        vec3 animatedPos;
        quaternion animatedOrient;
        int j = 0;

        memcpy(animatedPos, baseJoint->pos, sizeof (vec3));
        memcpy(animatedOrient, baseJoint->orient, sizeof (quaternion));

        if (jointInfos[i].flags & 1) /* Tx */
        {
            animatedPos[0] = animFrameData[jointInfos[i].startIndex + j];
            ++j;
        }

        if (jointInfos[i].flags & 2) /* Ty */
        {
            animatedPos[1] = animFrameData[jointInfos[i].startIndex + j];
            ++j;
        }

        if (jointInfos[i].flags & 4) /* Tz */
        {
            animatedPos[2] = animFrameData[jointInfos[i].startIndex + j];
            ++j;
        }

        if (jointInfos[i].flags & 8) /* Qx */
        {
            animatedOrient[0] = animFrameData[jointInfos[i].startIndex + j];
            ++j;
        }

        if (jointInfos[i].flags & 16) /* Qy */
        {
            animatedOrient[1] = animFrameData[jointInfos[i].startIndex + j];
            ++j;
        }

        if (jointInfos[i].flags & 32) /* Qz */
        {
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
        if (thisJoint->parent < 0)
        {
            memcpy(thisJoint->pos, animatedPos, sizeof (vec3));
            memcpy(thisJoint->orient, animatedOrient, sizeof (quaternion));
        }
        else
        {
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

int ReadMD5Anim (const char *filename, md5_anim_t *anim) {
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


void InterpolateSkeletons(const md5_joint_t *skelA, const md5_joint_t *skelB,
                          int num_joints, float interp, md5_joint_t *out)
{
    for(int i = 0; i < num_joints; i++) {
        /* Copy parent index */
        out[i].parent = skelA[i].parent;

        /* Linear interpolation for position */
        out[i].pos[0] = skelA[i].pos[0] + interp*(skelB[i].pos[0] - skelA[i].pos[0]);
        out[i].pos[1] = skelA[i].pos[1] + interp*(skelB[i].pos[1] - skelA[i].pos[1]);
        out[i].pos[2] = skelA[i].pos[2] + interp*(skelB[i].pos[2] - skelA[i].pos[2]);

        /* Spherical linear interpolation for orientation */
        quatSlerp(skelA[i].orient, skelB[i].orient, interp, out[i].orient);
    }
}



Mesh* prepareMD5Mesh(md5_model_t *mdl, md5_joint_t *skeleton) {
    Mesh *m = initMesh();

    //Transformar pra mesh da engine
    for(int i = 0; i < mdl->num_meshes; i++) {
        Triangles *t = addTris(m);

        md5_mesh_t *md5mesh = &mdl->meshes[i];
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
        for(int j = 0; j < md5mesh->num_verts; j++) {
            tTexCoords[2*j] = md5mesh->vertices[j].s;
            tTexCoords[2*j + 1] = md5mesh->vertices[j].t;
            tVerts[3*j] = 0.0;
            tVerts[3*j + 1] = 0.0;
            tVerts[3*j + 2] = 0.0;
            for(int k = 0; k < md5mesh->vertices[j].count; k++) {
                md5_weight_t *w = &md5mesh->weights[md5mesh->vertices[j].start + k];
                //md5_joint_t *joint = &mdl->baseSkel[w->joint];
                md5_joint_t *joint = &skeleton[w->joint];

                vec3 wv;
                rotateVec(w->pos, joint->orient, wv);
                tVerts[3*j] += (joint->pos[0]+ wv[0])*w->bias;
                tVerts[3*j + 1] += (joint->pos[1]+ wv[1])*w->bias;
                tVerts[3*j + 2] += (joint->pos[2]+ wv[2])*w->bias;
            }
        }
        setboundingbox(&t->b, tVerts, md5mesh->num_verts);
        addVertices(t, md5mesh->num_verts*3, 3, tVerts);
        addTexCoords(t, md5mesh->num_verts*2, 2, 0, tTexCoords);
        float *tNormals = malloc(sizeof(float)*md5mesh->num_verts*3); 
        memset(tNormals, 0, sizeof(float)*md5mesh->num_verts*3);

        for(int j = 0; j < md5mesh->num_tris; j++) {
            const int ia = tIndices[3*j];
            const int ib = tIndices[3*j + 1];
            const int ic = tIndices[3*j + 2];

            vec3 iapos = { tVerts[3*ia], tVerts[3*ia + 1], tVerts[3*ia + 2]  };
            vec3 ibpos = { tVerts[3*ib], tVerts[3*ib + 1], tVerts[3*ib + 2]  };
            vec3 icpos = { tVerts[3*ic], tVerts[3*ic + 1], tVerts[3*ic + 2]  };

            vec3 e1; //vert[ia].pos - vert[ib].pos;
            vecSub(iapos, ibpos, e1);
            vec3 e2; //vert[ic].pos - vert[ib].pos;
            vecSub(icpos, ibpos, e2);
            vec3 no; //cross( e1, e2 );
            cross(e1, e2, no);

            //vert[ia].normal += no;
            //vert[ib].normal += no;
            //vert[ic].normal += no;
            tNormals[3*ia] = no[0];
            tNormals[3*ia + 1] = no[1];
            tNormals[3*ia + 2] = no[2];
            tNormals[3*ib] = no[0];
            tNormals[3*ib + 1] = no[1];
            tNormals[3*ib + 2] = no[2];
            tNormals[3*ic] = no[0];
            tNormals[3*ic + 1] = no[1];
            tNormals[3*ic + 2] = no[2];
        }
        for(int j = 0; j < md5mesh->num_verts; j++) {
            vec3 n = { tNormals[3*j], tNormals[3*j + 1], tNormals[3*j + 2] };
            vecNormalize(n);
            tNormals[3*j] = n[0];
            tNormals[3*j + 1] = n[1];
            tNormals[3*j + 2] = n[2];
        }
        addNormals(t, md5mesh->num_verts*3, 3, tNormals);
        //TODO material
    }
    prepareMesh(m);
    return m;
}

/**
 * Prepare a mesh for drawing.  Compute mesh's final vertex positions
 * given a skeleton.  Put the vertices in vertex arrays.
 */
/*void PrepareMesh (const struct md5_mesh_t *mesh,
        const struct md5_joint_t *skeleton)
{
    int i, j, k;

    //Setup vertex indices
    for (k = 0, i = 0; i < mesh->num_tris; ++i)
    {
        for (j = 0; j < 3; ++j, ++k)
            vertexIndices[k] = mesh->triangles[i].index[j];
    }

    //Setup vertices
    for (i = 0; i < mesh->num_verts; ++i)
    {
        vec3 finalVertex = { 0.0f, 0.0f, 0.0f };

        //Calculate final vertex to draw with weights
        for (j = 0; j < mesh->vertices[i].count; ++j)
        {
            const struct md5_weight_t *weight
                = &mesh->weights[mesh->vertices[i].start + j];
            const struct md5_joint_t *joint
                = &skeleton[weight->joint];

            //Calculate transformed vertex for this weight
            vec3 wv;
            //Quat_rotatePoint (joint->orient, weight->pos, wv);

            //The sum of all weight->bias should be 1.0
            finalVertex[0] += (joint->pos[0] + wv[0]) * weight->bias;
            finalVertex[1] += (joint->pos[1] + wv[1]) * weight->bias;
            finalVertex[2] += (joint->pos[2] + wv[2]) * weight->bias;
        }

        vertexArray[i][0] = finalVertex[0];
        vertexArray[i][1] = finalVertex[1];
        vertexArray[i][2] = finalVertex[2];
    }
}*/



/**
 * Free resources allocated for the model.
 */
/*void FreeModel (struct md5_model_t *mdl)
{
    int i;

    if (mdl->baseSkel)
    {
        free (mdl->baseSkel);
        mdl->baseSkel = NULL;
    }

    if (mdl->meshes)
    {
        //Free mesh data
        for (i = 0; i < mdl->num_meshes; ++i)
        {
            if (mdl->meshes[i].vertices)
            {
                free (mdl->meshes[i].vertices);
                mdl->meshes[i].vertices = NULL;
            }

            if (mdl->meshes[i].triangles)
            {
                free (mdl->meshes[i].triangles);
                mdl->meshes[i].triangles = NULL;
            }

            if (mdl->meshes[i].weights)
            {
                free (mdl->meshes[i].weights);
                mdl->meshes[i].weights = NULL;
            }
        }

        free (mdl->meshes);
        mdl->meshes = NULL;
    }
}*/

/*void AllocVertexArrays ()
{
    vertexArray = (vec3 *)malloc (sizeof (vec3) * max_verts);
    vertexIndices = (GLuint *)malloc (sizeof (GLuint) * max_tris * 3);
}*/


/*void FreeVertexArrays ()
{
    if (vertexArray)
    {
        free (vertexArray);
        vertexArray = NULL;
    }

    if (vertexIndices)
    {
        free (vertexIndices);
        vertexIndices = NULL;
    }
}*/

/**
 * Draw the skeleton as lines and points (for joints).
 */
/*void DrawSkeleton (const struct md5_joint_t *skeleton, int num_joints)
{
    int i;

    glPointSize (5.0f);
    glColor3f (1.0f, 0.0f, 0.0f);
    glBegin (GL_POINTS);
    for (i = 0; i < num_joints; ++i)
        glVertex3fv (skeleton[i].pos);
    glEnd ();
    glPointSize (1.0f);

    glColor3f (0.0f, 1.0f, 0.0f);
    glBegin (GL_LINES);
    for (i = 0; i < num_joints; ++i)
    {
        if (skeleton[i].parent != -1)
        {
            glVertex3fv (skeleton[skeleton[i].parent].pos);
            glVertex3fv (skeleton[i].pos);
        }
    }
    glEnd();
}*/

/*void init (const char *filename, const char *animfile)
{
    glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
    glShadeModel (GL_SMOOTH);

    glEnable (GL_DEPTH_TEST);

    if (!ReadMD5Model (filename, &md5file))
        exit (EXIT_FAILURE);

    AllocVertexArrays ();

    if (animfile)
    {
        if (!ReadMD5Anim (animfile, &md5anim))
        {
            FreeAnim (&md5anim);
        }
        else
        {
            animInfo.curr_frame = 0;
            animInfo.next_frame = 1;

            animInfo.last_time = 0;
            animInfo.max_time = 1.0 / md5anim.frameRate;

            skeleton = (struct md5_joint_t *)
                malloc (sizeof (struct md5_joint_t) * md5anim.num_joints);

            animated = 1;
        }
    }

    if (!animated)
        printf ("init: no animation loaded.\n");
}*/

/*void cleanup ()
{
    FreeModel (&md5file);
    FreeAnim (&md5anim);

    if (animated && skeleton)
    {
        free (skeleton);
        skeleton = NULL;
    }

    FreeVertexArrays ();
}

    void
reshape (int w, int h)
{
    if (h == 0)
        h = 1;

    glViewport (0, 0, (GLsizei)w, (GLsizei)h);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective (45.0, w/(GLdouble)h, 0.1, 1000.0);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
}

    void
display ()
{
    int i;
    static float angle = 0;
    static double curent_time = 0;
    static double last_time = 0;

    last_time = curent_time;
    curent_time = (double)glutGet (GLUT_ELAPSED_TIME) / 1000.0;

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity ();

    glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

    glTranslatef (0.0f, -35.0f, -150.0f);
    glRotatef (-90.0f, 1.0, 0.0, 0.0);
    glRotatef (angle, 0.0, 0.0, 1.0);

    angle += 25 * (curent_time - last_time);

    if (angle > 360.0f)
        angle -= 360.0f;

    if (animated)
    {
        Animate (&md5anim, &animInfo, curent_time - last_time);

        InterpolateSkeletons (md5anim.skelFrames[animInfo.curr_frame],
                md5anim.skelFrames[animInfo.next_frame],
                md5anim.num_joints,
                animInfo.last_time * md5anim.frameRate,
                skeleton);
    }
    else
    {
        skeleton = md5file.baseSkel;
    }

    DrawSkeleton (skeleton, md5file.num_joints);

    glColor3f (1.0f, 1.0f, 1.0f);

    glEnableClientState (GL_VERTEX_ARRAY);

    for (i = 0; i < md5file.num_meshes; ++i)
    {
        PrepareMesh (&md5file.meshes[i], skeleton);

        glVertexPointer (3, GL_FLOAT, 0, vertexArray);

        glDrawElements (GL_TRIANGLES, md5file.meshes[i].num_tris * 3,
                GL_UNSIGNED_INT, vertexIndices);
    }

    glDisableClientState (GL_VERTEX_ARRAY);

    glutSwapBuffers ();
    glutPostRedisplay ();
}

*/
