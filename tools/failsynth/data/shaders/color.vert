#version 330

layout(location = 0) in vec3 inpos;
layout(location = 3) in vec4 incolor;

out vec4 color;

uniform mat4 mvp;

void main(void)
{
    color = incolor;
	gl_Position = mvp*vec4(inpos, 1.0);
}
