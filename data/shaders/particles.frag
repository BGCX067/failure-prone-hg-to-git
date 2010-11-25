#version 330 core

uniform sampler2D tex;
out vec4 fragColor;
//in vec2 texCoord;

in vec3 size;

void main() {
    //fragColor = vec4(size, 0.0, 0.0, 1.0);//
  //  fragColor = size;
   vec4 texcolor = vec4(texture(tex, gl_PointCoord).rgb, 1.0) * vec4(0.5, 1.0, 0.1, size.z);
 //  vec3 color = vec3(0.5, 1.0, 0.1);
    fragColor = texcolor; //vec4( texcolor, 0.);
//      fragColor = vec4(texCoord.x, 0.0, 0.0, 1.0 );
}
