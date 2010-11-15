#version 330  core

uniform mat4 mvp;
layout(location = 0) in vec3 Pos;

void main() {
    gl_PointSize = 10.0;
    
    gl_Position = mvp*vec4(Pos, 1.0);
}
