#version 330  core

uniform mat4 mvp;
vec3 eyeposition;
layout(location = 0) in vec3 Pos;
layout (location = 2) in vec3 acceleration;
uniform float time;

void main() {

    
    //vec3 d = Pos + 0.5*vec3(0, 10.1, 0)*time*time;
	
    gl_PointSize = 40.0;
    
    gl_Position = mvp*vec4(Pos, 1.0);
}
