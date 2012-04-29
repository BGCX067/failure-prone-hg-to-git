#version 330

layout(location = 0) in vec3 inpos;
layout(location = 8) in vec2 intexcoord;

out vec2 texcoord;
out vec2 pos;

void main(void)
{
    texcoord = intexcoord;
    pos = vec2(inpos);
	gl_Position = vec4(inpos, 1.0);
}
