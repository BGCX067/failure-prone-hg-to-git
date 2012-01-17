#version 330

layout(location = 0) in vec3 inpos;

void main(void)
{
	gl_Position = vec4(inpos, 1.0);
}
