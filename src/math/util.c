#include "util.h"

float max(float a, float b){
	if (a>b)
		return a;
	else
		return b;
}

float min(float a, float b){
	if (a<b)
		return a;
	else
		return b;
}

int rayTriangleIntersection(vec3 ro, vec3 rd, vec3 va, vec3 vb, vec3 vc, float *t) {
    vec3 edge1, edge2, tvec, pvec, qvec;
    
    vecSub(vb, va, edge1);
    vecSub(vc, va, edge2);

    cross(rd, edge2, pvec);

    float det = dot(edge1, pvec);
    float invdet = 1.0f/det;

    //TODO considerar culling?
    vecSub(ro, va, tvec);
    float u = dot(tvec, pvec)*invdet;

    if(u < 0.0f || u > 1.0f)
        return 0;

    cross(tvec, edge1, qvec);
    float v = dot(rd, qvec)*invdet;
    if(v < 0.0f || u + v > 1.0f)
        return 0;

   *t =  dot(edge2, qvec)*invdet;

   return 1;
}

int rayMeshIntersection(vec3 ro, vec3 rd, Mesh *m, int *indices) {
    //TODO testar logo com bounding box pra otimizar casos em que não há interseção
    int intersect = 0;
    float minT = 99999999.0f;
    //percorrer a lista de triangulos mesmo
    for(unsigned int j = 0; j < m->indicesCount; j+=3) {
        unsigned int ia = m->indices[j], ib = m->indices[j + 1], ic = m->indices[j + 2];
        vec3 va, vb, vc;
        va[0] = m->vertices[3*ia]; va[1] = m->vertices[3*ia + 1]; va[2] = m->vertices[3*ia + 2];
        vb[0] = m->vertices[3*ib]; vb[1] = m->vertices[3*ib + 1]; vb[2] = m->vertices[3*ib + 2];
        vc[0] = m->vertices[3*ic]; vc[1] = m->vertices[3*ic + 1]; vc[2] = m->vertices[3*ic + 2];

        float t;
        if(rayTriangleIntersection(ro, rd, va, vb, vc, &t)) {
            if(t < minT) {
                minT = t;
                intersect = 1;
                indices[0] = ia; indices[1] = ib; indices[2] = ic;
            }
        }
    }

    return intersect;
}

int picking(int mouseX, int mouseY, mat4 modelviewMatrix, mat4 projectionMatrix, int viewPort[4], Mesh *m, int *indices) {
    float pNear[3], pFar[3];
    fpUnproject(mouseX, mouseY, 0.0, modelviewMatrix, projectionMatrix, viewPort, &pNear[0], &pNear[1], &pNear[2]);
    fpUnproject(mouseX, mouseY, 1.0, modelviewMatrix, projectionMatrix, viewPort, &pFar[0], &pFar[1], &pFar[2]);
   
    //Pega a posição da camera a partir da modelview
    vec3 rayOrigin;
    fpGetPosFromMatrix(modelviewMatrix, rayOrigin);

    vec3 rayDir;
    rayDir[0] = pFar[0] - pNear[0];
    rayDir[1] = pFar[1] - pNear[1];
    rayDir[2] = pFar[2] - pNear[2];
    vecNormalize(rayDir);

    return rayMeshIntersection(rayOrigin, rayDir, m, indices);
}
