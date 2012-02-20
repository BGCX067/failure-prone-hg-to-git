#include "matrix.h"
#include <math.h>


/* Por algum motivo M_PI nao aparece definido em math.h */
#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

void fpIdentity(mat4 m) {
    for(int i = 0; i < 16; i++)
        m[i] = 0.0;
    m[0] = m[5] = m[10] = m[15] = 1.0;
}

void fpMultMatrix(mat4 res, mat4 a, mat4 b) {
    res[0] = a[0]*b[0] + a[4]*b[1] + a[8]*b[2] + a[12]*b[3];
    res[1] = a[1]*b[0] + a[5]*b[1] + a[9]*b[2] + a[13]*b[3];
    res[2] = a[2]*b[0] + a[6]*b[1] + a[10]*b[2] + a[14]*b[3];
    res[3] = a[3]*b[0] + a[7]*b[1] + a[11]*b[2] + a[15]*b[3];
    res[4] = a[0]*b[4] + a[4]*b[5] + a[8]*b[6] + a[12]*b[7];
    res[5] = a[1]*b[4] + a[5]*b[5] + a[9]*b[6] + a[13]*b[7];
    res[6] = a[2]*b[4] + a[6]*b[5] + a[10]*b[6] + a[14]*b[7];
    res[7] = a[3]*b[4] + a[7]*b[5] + a[11]*b[6] + a[15]*b[7];
    res[8] = a[0]*b[8] + a[4]*b[9] + a[8]*b[10] + a[12]*b[11];
    res[9] = a[1]*b[8] + a[5]*b[9] + a[9]*b[10] + a[13]*b[11];
    res[10] = a[2]*b[8] + a[6]*b[9] + a[10]*b[10] + a[14]*b[11];
    res[11] = a[3]*b[8] + a[7]*b[9] + a[11]*b[10] + a[15]*b[11];
    res[12] = a[0]*b[12] + a[4]*b[13] + a[8]*b[14] + a[12]*b[15];
    res[13] = a[1]*b[12] + a[5]*b[13] + a[9]*b[14] + a[13]*b[15];
    res[14] = a[2]*b[12] + a[6]*b[13] + a[10]*b[14] + a[14]*b[15];
    res[15] = a[3]*b[12] + a[7]*b[13] + a[11]*b[14] + a[15]*b[15];
}


void fpTranspose(mat4 t, mat4 m) {
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            t[i + 4*j] = m[j + 4*i];
}

//FIXME conferir determinante
float fpDeterminant(mat4 m) {
    return m[12]*m[9]*m[6]*m[3] - m[8]*m[13]*m[6]*m[3] - m[12]*m[5]*m[10]*m[3] +  
           m[4]*m[13]*m[10]*m[3] + m[8]*m[5]*m[14]*m[3] - m[4]*m[9]*m[14]*m[3] -
           m[12]*m[9]*m[2]*m[7] + m[8]*m[13]*m[2]*m[7] + m[12]*m[1]*m[10]*m[7] -
           m[0]*m[13]*m[10]*m[7] - m[8]*m[1]*m[14]*m[7] + m[0]*m[9]*m[14]*m[7] +
           m[12]*m[5]*m[2]*m[11] - m[4]*m[13]*m[2]*m[11] - m[12]*m[1]*m[6]*m[11] + 
           m[0]*m[13]*m[6]*m[11] + m[4]*m[1]*m[14]*m[11] - m[0]*m[5]*m[14]*m[11] -
           m[8]*m[5]*m[2]*m[15] + m[4]*m[9]*m[2]*m[15] + m[8]*m[1]*m[6]*m[15] - 
           m[0]*m[9]*m[6]*m[15] - m[4]*m[1]*m[10]*m[15] + m[0]*m[5]*m[10]*m[15];
}

void fpInverse(mat4 inv, mat4 m) {
    inv[0] = m[9]*m[14]*m[7] - m[13]*m[10]*m[7] + m[13]*m[6]*m[11] - 
           m[5]*m[14]*m[11] - m[9]*m[6]*m[15] + m[5]*m[10]*m[15];

    inv[4] = m[12]*m[10]*m[7] - m[8]*m[14]*m[7] - m[12]*m[6]*m[11] + 
           m[4]*m[14]*m[11] + m[8]*m[6]*m[15] - m[4]*m[10]*m[15];

    inv[8] = m[8]*m[13]*m[7] - m[12]*m[9]*m[7] + m[12]*m[5]*m[11] - 
           m[4]*m[13]*m[11] - m[8]*m[5]*m[15] + m[4]*m[9]*m[15];

    inv[12] = m[12]*m[9]*m[6] - m[8]*m[13]*m[6] - m[12]*m[5]*m[10] + 
            m[4]*m[13]*m[10] + m[8]*m[5]*m[14] - m[4]*m[9]*m[14];

    inv[1] = m[13]*m[10]*m[3] - m[9]*m[14]*m[3] - m[13]*m[2]*m[11] + 
           m[1]*m[14]*m[11] + m[9]*m[2]*m[15] - m[1]*m[10]*m[15];

    inv[5] = m[8]*m[14]*m[3] - m[12]*m[10]*m[3] + m[12]*m[2]*m[11] - 
           m[0]*m[14]*m[11] - m[8]*m[2]*m[15] + m[0]*m[10]*m[15];

    inv[9] = m[12]*m[9]*m[3] - m[8]*m[13]*m[3] - m[12]*m[1]*m[11] + 
           m[0]*m[13]*m[11] + m[8]*m[1]*m[15] - m[0]*m[9]*m[15];

    inv[13] = m[8]*m[13]*m[2] - m[12]*m[9]*m[2] + m[12]*m[1]*m[10] - 
            m[0]*m[13]*m[10] - m[8]*m[1]*m[14] + m[0]*m[9]*m[14];

    inv[2] = m[5]*m[14]*m[3] - m[13]*m[6]*m[3] + m[13]*m[2]*m[7] - 
           m[1]*m[14]*m[7] - m[5]*m[2]*m[15] + m[1]*m[6]*m[15];

    inv[6] = m[12]*m[6]*m[3] - m[4]*m[14]*m[3] - m[12]*m[2]*m[7] + 
           m[0]*m[14]*m[7] + m[4]*m[2]*m[15] - m[0]*m[6]*m[15];

    inv[10] = m[4]*m[13]*m[3] - m[12]*m[5]*m[3] + m[12]*m[1]*m[7] - 
            m[0]*m[13]*m[7] - m[4]*m[1]*m[15] + m[0]*m[5]*m[15];

    inv[14] = m[12]*m[5]*m[2] - m[4]*m[13]*m[2] - m[12]*m[1]*m[6] + 
            m[0]*m[13]*m[6] + m[4]*m[1]*m[14] - m[0]*m[5]*m[14];

    inv[3] = m[9]*m[6]*m[3] - m[5]*m[10]*m[3] - m[9]*m[2]*m[7] + 
           m[1]*m[10]*m[7] + m[5]*m[2]*m[11] - m[1]*m[6]*m[11];

    inv[7] = m[4]*m[10]*m[3] - m[8]*m[6]*m[3] + m[8]*m[2]*m[7] - 
           m[0]*m[10]*m[7] - m[4]*m[2]*m[11] + m[0]*m[6]*m[11];
    inv[11] = m[8]*m[5]*m[3] - m[4]*m[9]*m[3] - m[8]*m[1]*m[7] + 
            m[0]*m[9]*m[7] + m[4]*m[1]*m[11] - m[0]*m[5]*m[11];

    inv[15] = m[4]*m[9]*m[2] - m[8]*m[5]*m[2] + m[8]*m[1]*m[6] - 
            m[0]*m[9]*m[6] - m[4]*m[1]*m[10] + m[0]*m[5]*m[10];

    float invdet = 1.0/fpDeterminant(m);
    for(int i = 0; i < 16; i++) {
        inv[i] /= invdet;
    }
}

void fptranslatefv(mat4 m, vec3 v) {
    m[12]= v[0]*m[0] + v[1]*m[4] + v[2]*m[8] + m[12];
	m[13]= v[0]*m[1] + v[1]*m[5] + v[2]*m[9] + m[13]; 
    m[14]= v[0]*m[2] + v[1]*m[6] + v[2]*m[10] + m[14];
    m[15]= v[0]*m[3] + v[1]*m[7] + v[2]*m[11] + m[15];
} 

void fptranslatef(mat4 m, float x, float y, float z) {
    m[12]= x*m[0] + y*m[4] + z*m[8] + m[12];
	m[13]= x*m[1] + y*m[5] + z*m[9] + m[13]; 
    m[14]= x*m[2] + y*m[6] + z*m[10] + m[14];
    m[15]= x*m[3] + y*m[7] + z*m[11] + m[15];
}

void fpperspective(mat4 m, float fovy, float ratio, float znear, float zfar) {
    float xmax = znear*tan(0.5*fovy*M_PI/180.0);
    float xmin = -xmax;

    float ymax = xmax/ratio;
    float ymin = -ymax;

    float doubleznear = 2.0f*znear;
    float one_deltax = 1.0f/(xmax - xmin);
    float one_deltay = 1.0f/(ymax - ymin);
    float one_deltaz = 1.0f/(zfar - znear);
    
    for(int i = 0; i < 16; i++)
        m[i] = 0;

    m[0] = doubleznear*one_deltax;
    m[5] = doubleznear*one_deltay;
    m[8] = (xmax + xmin)*one_deltax;
    m[9] = (ymax + ymin)*one_deltay;
    m[10] = -(zfar + znear)*one_deltaz;
    m[11] = -1.0f;
    m[14] = -(zfar*doubleznear) * one_deltaz;
}

void fpLookAt(mat4 result, vec3 pos, vec3 look, vec3  up){

    vec3 right;
    cross(look, up, right);
    vecNormalize(right);
    
    vec3 newup;
    cross(right, look, newup);
    vecNormalize(newup);

    result[0] = right[0];
    result[4] = right[1];
    result[8] = right[2];

    result[1] = newup[0];
    result[5] = newup[1];
    result[9] = newup[2];

    result[2] = -look[0];
    result[6] = -look[1];
    result[10] = -look[2];

    result[3] = result[7] = result[11] = 0.0;
    result[12] = result[13] = result[14] = 0.0; 
    result[15] = 1.0;


    fptranslatef(result, -pos[0], -pos[1], -pos[2]);


}

void fpOrtho( mat4 m, float xMin, float xMax, float yMin, float yMax, float zMin, float zMax){

	fpIdentity(m);

	m[0] = 2.0f / (xMax - xMin);
        m[5] = 2.0f / (yMax - yMin);
	m[10] = -2.0f / (zMax - zMin);
	m[12] = -((xMax + xMin)/(xMax - xMin));
	m[13] = -((yMax + yMin)/(yMax - yMin));
	m[14] = -((zMax + zMin)/(zMax - zMin));
	m[15] = 1.0f;
}

void fpNormalMatrix(mat3 normalmatrix, mat4 mv) {
    vec3 x0;
    x0[0] = mv[0];
    x0[1] = mv[1];
    x0[2] = mv[2];

    vec3 x1;
    x1[0] = mv[4];
    x1[1] = mv[5];
    x1[2] = mv[6];

    vec3 x2;
    x2[0] = mv[8];
    x2[1] = mv[9];
    x2[2] = mv[10];

    vec3 x1crossx2;
    cross(x1, x2, x1crossx2);
    vec3 x0crossx1;
    cross(x0, x1, x0crossx1);
    vec3 x2crossx0;
    cross(x2, x0, x2crossx0);
    
     
    float invdet = 1.0/dot(x0, x1crossx2);
    normalmatrix[0] = invdet*x1crossx2[0];
    normalmatrix[1] = invdet*x1crossx2[1];
    normalmatrix[2] = invdet*x1crossx2[2];
    
    normalmatrix[3] = invdet*x2crossx0[0];
    normalmatrix[4] = invdet*x2crossx0[1];
    normalmatrix[5] = invdet*x2crossx0[2];

    normalmatrix[6] = invdet*x0crossx1[0];
    normalmatrix[7] = invdet*x0crossx1[1];
    normalmatrix[8] = invdet*x0crossx1[2];
}
