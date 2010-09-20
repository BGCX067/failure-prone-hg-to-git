#version 330 core

uniform mat4 mvp;
uniform mat4 modelview;
layout(location = 0) in vec3 vertPos;
layout(location = 2) in vec3 vertNormal;

out vec3 position;
out vec3 normal;
uniform mat4 normalmatrix;
void main() {
	gl_Position = mvp*vec4(vertPos, 1.0);
	position = (modelview*vec4(vertPos, 1.0)).xyz;
	normal = normalize((normalmatrix*vec4(vertNormal, 1.0)).xyz);
}

