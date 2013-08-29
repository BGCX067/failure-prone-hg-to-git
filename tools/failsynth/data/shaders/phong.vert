#version 330

layout(location = 0) in vec3 inpos;
layout(location = 2) in vec3 innormal;

out vec3 vertexpos;
out vec3 normal;
out vec3 lPos;

uniform mat4 modelview;
uniform mat4 view;
uniform mat3 normalmatrix;
uniform mat4 mvp;

uniform vec3 LightPosition;

void main(void)
{
    normal = normalize(normalmatrix*innormal);
    vertexpos = vec3(modelview*vec4(inpos, 1.0));
    lPos = vec3(view*vec4(LightPosition, 1.0));
	gl_Position = mvp*vec4(inpos, 1.0);
}
