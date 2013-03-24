#version 330

layout(location = 0) in vec3 inpos;
layout(location = 2) in vec3 innormal;
layout(location = 8) in vec3 intexcoord;

out vec3 vertexpos;
out vec3 normal;
out vec3 texcoord;
out vec3 camerapos;

uniform mat4 modelview;
uniform mat3 normalmatrix;
uniform mat4 mvp;
uniform vec3 eyepos;
uniform mat4 invModelview;

void main(void)
{
    normal = normalize(normalmatrix*innormal);
    vertexpos = inpos;
    //vertexpos = vec3(modelview*vec4(inpos, 1.0));
    camerapos = vec3(invModelview*vec4(eyepos, 1.0));
    //camerapos = eyepos;
    vertexpos = inpos;
    texcoord = intexcoord;
	gl_Position = mvp*vec4(inpos, 1.0);
}
