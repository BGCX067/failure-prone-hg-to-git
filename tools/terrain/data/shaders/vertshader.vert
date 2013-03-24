#version 330

layout(location = 0) in vec3 inpos;

uniform mat4 mvp;

void main(void)
{
	gl_Position = mvp*vec4(inpos, 1.0);
	//gl_Position = vec4(inpos, 1.0);
}
