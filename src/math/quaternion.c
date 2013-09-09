#include "quaternion.h"
#include <math.h>
#include <string.h> //memcpy

float Lengthq(const quat q) {
    return sqrt(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3]);
}

void Normalizeq(quat q) {
    float len = Lengthq(q);
    q[0] /= len;
    q[1] /= len;
    q[2] /= len;
    q[3] /= len;
}

void Conjugateq(const quat q, quat conj) {
    conj[0] = -q[0];
    conj[1] = -q[1];
    conj[2] = -q[2];
    conj[3] =  q[3];
}

void Multq(const quat q2, const quat q1, quat res) {
    quat temp;
    temp[0] = q2[1]*q1[2] - q2[2]*q1[1] + q2[3]*q1[0] + q2[0]*q1[3];
	temp[1] = q2[2]*q1[0] - q2[0]*q1[2] + q2[3]*q1[1] + q2[1]*q1[3];
	temp[2] = q2[0]*q1[1] - q2[1]*q1[0] + q2[3]*q1[2] + q2[2]*q1[3];
	temp[3] = q2[3]*q1[3] - q2[0]*q1[0] - q2[1]*q1[1] - q2[2]*q1[2];
    memcpy(res, temp, sizeof(quat));
}


void Multqv(const quat q, const vec3 v, quat res) {
    res[3] = -(q[0]*v[0]) - (q[1]* v[1]) - (q[2]*v[2]);
    res[0] = (q[3]*v[0]) + (q[1]*v[2]) - (q[2]*v[1]);
    res[1] = (q[3]*v[1]) + (q[2]*v[0]) - (q[0]*v[2]);
    res[2] = (q[3]*v[2]) + (q[0]*v[1]) - (q[1]*v[0]);

}

void FromAxisAngle(const vec3 v, float theta, quat q) {
    float angle = theta*0.5;
    q[0] = v[0]*sin(angle);
    q[1] = v[1]*sin(angle);
    q[2] = v[2]*sin(angle);
    q[3] = cos(angle);
    Normalizeq(q);
}

void ToMatrixq(const quat q, mat4 m) {
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
}

void Rotateq(const vec3 v, const quat q, vec3 res) {
    /*quat q2;
    q2[0] = v[0];
    q2[1] = v[1];
    q2[2] = v[2];
    q2[3] = 0.0;*/

    quat result, tmp;
    
    quat conjq;
    Conjugateq(q, conjq);
    ////quatMult(q, q2, tmp);
    Multqv(q, v, tmp);
    Multq(tmp, conjq, result);
    
    res[0] = result[0];
    res[1] = result[1];
    res[2] = result[2];
}

void Angleq(quat q) {
    float t = 1.0f - q[0]*q[0] - q[1]*q[1] - q[2]*q[2];
    if (t < 0.0f)
        q[3] = 0.0f;
    else
        q[3] = -sqrt(t);
}

float Dotq(const quat q1, const quat q2) {
    return q1[0]*q2[0] + q1[1]*q2[1] + q1[2]*q2[2] + q1[3]*q2[3];
}

void Slerpq(const quat q1, const quat q2, float t, quat res) {
    /* Check for out-of range parameter and return edge points if so */
    if (t <= 0.0) {
        memcpy(res, q1, sizeof(quat));
        return;
    }

    if (t >= 1.0) {
        memcpy(res, q2, sizeof (quat));
        return;
    }

    /* Compute "cosine of angle between quaternions" using dot product */
    float cosOmega = Dotq(q1, q2);

    /* If negative dot, use -q1.  Two quaternions q and -q
       represent the same rotation, but may produce
       different slerp.  We chose q or -q to rotate using
       the acute angle. */
    float q1w = q2[3];
    float q1x = q2[0];
    float q1y = q2[1];
    float q1z = q2[2];

    if (cosOmega < 0.0f) {
        q1w = -q1w;
        q1x = -q1x;
        q1y = -q1y;
        q1z = -q1z;
        cosOmega = -cosOmega;
    }

    /* We should have two unit quaternions, so dot should be <= 1.0 */
    //assert(cosOmega < 1.1f);

    /* Compute interpolation fraction, checking for quaternions
       almost exactly the same */
    float k0, k1;
    if (cosOmega > 0.9999f) {
        /* Very close - just use linear interpolation,
           which will protect againt a divide by zero */
        k0 = 1.0f - t;
        k1 = t;
    } else {
        /* Compute the sin of the angle using the
           trig identity sin^2(omega) + cos^2(omega) = 1 */
        float sinOmega = sqrt(1.0f - (cosOmega * cosOmega));

        /* Compute the angle from its sin and cosine */
        float omega = atan2(sinOmega, cosOmega);

        /* Compute inverse of denominator, so we only have
           to divide once */
        float oneOverSinOmega = 1.0f/sinOmega;

        /* Compute interpolation parameters */
        k0 = sin((1.0f - t)*omega)*oneOverSinOmega;
        k1 = sin(t*omega)*oneOverSinOmega;
    }

    /* Interpolate and return new quat */
    res[3] = (k0*q1[3]) + (k1*q1w);
    res[0] = (k0*q1[0]) + (k1*q1x);
    res[1] = (k0*q1[1]) + (k1*q1y);
    res[2] = (k0*q1[2]) + (k1*q1z);
}

void Setq(quat dest, const quat src) {
    memcpy(dest, src, sizeof(quat));
}

void Setqf(quat q, float x, float y, float z, float w) {
    q[0] = x; q[1] = y; q[2] = z; q[3] = w;
}

