#version 330  core

uniform mat4 mvp;
uniform vec3 eyeposition;
layout(location = 0) in vec3 Pos;
layout(location = 2) in vec3 inNormal;

out vec3 size;
//out vec2 texCoord;

void main() {

    vec4 eyepos = mvp*vec4(eyeposition, 1.0);
    gl_Position = mvp*vec4(Pos, 1.0);
 
//    gl_PointSize = clamp(10.0*1.0/pow(distance(eyepos,gl_Position), 2.0), 1.0, 10.0);
//      gl_PointSize = (distance(eyepos,gl_Position);
	gl_PointSize = 10.0;
    size = inNormal;
   
  //  texCoord = inTexCoord.xy;
}
