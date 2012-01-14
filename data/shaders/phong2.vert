#version 330

layout(location = 0) in vec3 invertexpos;
layout(location = 2) in vec3 innormal;
layout(location = 8) in vec2 intexcoord;

out vec3 vertexpos;
out vec3 normal;
out vec2 texcoord;

uniform mat4 modelview;
uniform mat3 normalmatrix;
uniform mat4 mvp;

void main() {
    normal = normalize(normalmatrix*innormal);
    vertexpos = vec3(modelview*vec4(invertexpos, 1.0));
    texcoord = intexcoord;

    gl_Position = mvp*vec4(invertexpos, 1.0);
}
