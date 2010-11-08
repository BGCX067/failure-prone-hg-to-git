#version 330  core

uniform mat4 mvp;

layout(location = 0) in vec3 Pos;
layout(location = 2) in vec3 Normal;
layout(location = 8) in vec2 inTexCoord;

out vec2 texCoord;
out vec3 normal;

void main() {
    texCoord = inTexCoord.xy;
    normal = Normal.xyz;
    gl_Position = mvp*vec4(Pos, 1.0);
}
