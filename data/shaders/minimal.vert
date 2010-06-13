#version 330 compatibility

uniform mat4 mvp;
out vec2 texCoord;

layout(location = 0) in vec3 Pos;
layout(location = 8) in vec2 inTexCoord;
void main() {
    texCoord = inTexCoord;

    gl_Position = mvp*vec4(Pos, 1.0);
}
