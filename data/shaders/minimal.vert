#version 330  core

uniform mat4 mvp;

layout(location = 0) in vec3 Pos;
layout(location = 8) in vec2 inTexCoord;

out vec2 texCoord; 

void main() {
    texCoord = inTexCoord.xy;
    gl_Position = mvp*vec4(Pos, 1.0);
}
