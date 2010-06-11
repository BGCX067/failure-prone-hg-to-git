#version 330 core

uniform mat4 mvp;

out vec4 color;
layout(location = 0) in vec3 Pos;
void main() {
    gl_Position = mvp*vec4(Pos, 1.0);

    color = vec4(0.2, 0.4, 0.6, 1.0);
}
