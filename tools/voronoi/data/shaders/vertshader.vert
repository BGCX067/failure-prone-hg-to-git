#version 330

layout(location = 0) in vec3 inpos;
layout(location = 8) in vec2 intexcoord;

out vec2 texcoord;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp*vec4(inpos, 1.0);
    texcoord = intexcoord;
	//gl_Position = vec4(inpos, 1.0);
}
