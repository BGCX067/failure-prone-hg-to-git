#version 330

layout(location = 0) in vec3 inpos;
layout(location = 8) in vec2 intexcoord;

out vec2 texcoord;
out vec3 pos;

uniform mat4 mvp;
uniform mat4 modelview;

void main(void)
{
    texcoord = intexcoord;
    pos = vec3(modelview*vec4(inpos, 1.0));
	gl_Position = mvp*vec4(inpos, 1.0);
}
