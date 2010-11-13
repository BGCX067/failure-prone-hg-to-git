#version 330  core

layout(location = 0) in vec3 Pos;
uniform mat4 mvp;

void main() {
    texCoord = inTexCoord.xy;
    gl_Position = mvp*vec4(Pos, 1.0);
}
