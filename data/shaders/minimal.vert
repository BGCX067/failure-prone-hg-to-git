#version 330 core

uniform mat4 mvp;
out vec2 texCoord;

layout(location = 0) in vec3 Pos;
layout(location = 2) in vec3 inTexCoord;
void main() {
    texCoord = vec2(inTexCoord.x, inTexCoord.y );

    gl_Position = mvp*vec4(Pos, 1.0);
}
