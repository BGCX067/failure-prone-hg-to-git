#version 330  core

uniform mat4 lightmvp;

layout(location = 0) in vec3 Pos;
layout(location = 2) in vec3 Normal;
layout(location = 8) in vec2 inTexCoord;

out vec2 texCoord;
out vec3 normal;

void main() {
    texCoord = inTexCoord.xy;
    normal = Normal;
    gl_Position = lightmvp*vec4(Pos, 1.0);
}
