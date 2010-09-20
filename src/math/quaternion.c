#include "quaternion.h"
#include "math.h"

float quatLength(quaternion q) {
    return sqrt(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3]);
}

void quatNormalize(quaternion q) {
    float len = quatLength(q);
    q[0] /= len;
    q[1] /= len;
    q[2] /= len;
    q[3] /= len;
}

void conjugate(quaternion q, quaternion conj) {
    conj[0] = -q[0];
    conj[1] = -q[1];
    conj[2] = -q[2];
    conj[3] =  q[3];
}

void mult(quaternion q2, quaternion q1, quaternion res) {
    res[0] = q2[1]*q1[2] - q2[2]*q1[1] + q2[3]*q1[0] + q2[0]*q1[3];
	res[1] = q2[2]*q1[0] - q2[0]*q1[2] + q2[3]*q1[1] + q2[1]*q1[3];
	res[2] = q2[0]*q1[1] - q2[1]*q1[0] + q2[3]*q1[2] + q2[2]*q1[3];
	res[3] = q2[3]*q1[3] - q2[0]*q1[0] - q2[1]*q1[1] - q2[2]*q1[2];
}

void fromAxisAngle(vec3 v, float theta, quaternion q) {
    float angle = theta*0.5;
    q[0] = v[0]*sin(angle);
    q[1] = v[1]*sin(angle);
    q[2] = v[2]*sin(angle);
    q[3] = cos(angle);
    quatNormalize(q);
}

void quatToMatrix(quaternion q, mat4 m) {
    float xx, yy, zz, wx, wy, wz, xy, xz, yz;
	xx = 2*q[0]*q[0];	yy = 2*q[1]*q[1];	zz = 2*q[2]*q[2];
	wx = 2*q[3]*q[0];	wy = 2*q[3]*q[1];	wz = 2*q[3]*q[2];
	xy = 2*q[0]*q[1];	xz = 2*q[0]*q[2];	yz = 2*q[1]*q[2];

	m[0] = 1.0 - yy - zz;
	m[1] = xy - wz;
	m[2] = xz + wy;

	m[4] = xy + wz;
	m[5] = 1.0 - xx - zz;
	m[6] = yz - wx;

	m[8] = xz - wy;
	m[9] = yz + wx;
	m[10] = 1.0 - xx - yy;
    
    m[15] = 1.0;
	m[3] = m[7] = m[11] =  m[12] = m[13] = m[14] = 0.0;
	//m[15] = 1.0;
}

void rotateVec(vec3 v, quaternion q, vec3 res) {
    quaternion q2;
    q2[0] = v[0];
    q2[1] = v[1];
    q2[2] = v[2];
    q2[3] = 0.0;

    quaternion result;
    
    quaternion conjq;
    conjugate(q, conjq);
    mult(q, q2, result);
    mult(result, conjq, result);
    
    res[0] = result[0];
    res[1] = result[1];
    res[2] = result[2];
}
