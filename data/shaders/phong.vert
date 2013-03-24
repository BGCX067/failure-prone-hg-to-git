#version 330 core

layout(location = 0) in vec3 vertPos;
layout(location = 2) in vec3 vertNormal;

out vec3 position;
out vec3 normal;

uniform mat4 mvp;
uniform mat4 modelview;

/* calcula normal a partir da modelview */
/*mat3 normalMatrix(mat4 mv) {
    mat3 n;
    n[0][0] = mv[1][1]*mv[2][2] - mv[2][1]*mv[1][2];
    n[0][1] = -(mv[1][0]*mv[2][2] - mv[2][0]*mv[1][2]);
    n[0][2] = mv[1][0]*mv[2][1] - mv[2][0]*mv[1][1];
    n[1][0] = -(mv[0][1]*mv[2][2] - mv[2][1]*mv[0][2]);
    n[1][1] = mv[0][0]*mv[2][2] - mv[2][0]*mv[0][2];
    n[1][2] = -(mv[0][0]*mv[1][2] - mv[1][0]*mv[0][2]);
    n[2][0] = mv[1][0]*mv[2][1] - mv[2][0]*mv[1][1];
    n[2][1] = -(mv[0][0]*mv[2][1] - mv[2][0]*mv[0][1]);
    n[2][2] = mv[0][0]*mv[1][1] - mv[1][0]*mv[0][1];

    return n;
} */

mat3 normalMatrix(mat4 mv) {	
	vec3 x0;
	x0[0] = mv[0][0];
	x0[1] = mv[1][0];
	x0[2] = mv[2][0];

	vec3 x1;
	x1[0] = mv[0][1];
	x1[1] = mv[1][1];
	x1[2] = mv[2][1];	

	vec3 x2;
	x2[0] = mv[0][2];
	x2[1] = mv[1][2];
	x2[2] = mv[2][2];	
	
	float det = dot(x0, cross(x1, x2));
	
	mat3 ret;
	vec3 line0 = (1.0/det)*cross(x1, x2);
	vec3 line1 = (1.0/det)*cross(x2, x0);
	vec3 line2 = (1.0/det)*cross(x0, x1);
	
	ret[0] = line0;
	ret[1] = line1;
	ret[2] = line2;
	
    return transpose(ret);
}


void main() {
	gl_Position = mvp*vec4(vertPos, 1.0);
	position = (modelview*vec4(vertPos, 1.0)).xyz;

    mat3 normalmatrix = normalMatrix(modelview);

	normal = normalize(normalmatrix*vertNormal);
}

