#version 330

in vec2 texcoord;

out vec4 outcolor;


uniform sampler2D tex;

void main()
{
	outcolor = texture(tex, texcoord);
    //outcolor = vec4(1.0, 0.0, 0.0, 1.0);
}
