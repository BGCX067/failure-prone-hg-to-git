#include "matrix.h"
#include <math.h>

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
    float xmax = znear * tan(fovy/2.0);
    float xmin = -xmax;

    float ymax = xmax/ratio;
    float ymin = -ymax;

    float doubleznear = 2.0f * znear;
    float one_deltax = 1.0f/(xmax - xmin);
    float one_deltay = 1.0f/(ymax - ymin);
    float one_deltaz = 1.0f/(zfar - znear);

    for(int i = 0; i < 16; i++)
        m[i] = 0.0;

    m[0] = doubleznear*one_deltax;
    m[5] = doubleznear*one_deltay;
    m[8] = (xmax + xmin)*one_deltax;
    m[9] = (ymax + ymin)*one_deltay;
    m[8] = -(zfar + znear)*one_deltaz;
    m[11] = -1.0f;
    m[14] = -(zfar*doubleznear) * one_deltaz;
}
