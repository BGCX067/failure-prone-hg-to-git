#version 330 core

uniform mat4 MVP;

layout(location = 0) in vec3 Pos;
void main() {
    gl_Position = MVP*vec4(Pos, 1.0);
}
