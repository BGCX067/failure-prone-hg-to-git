#version 330

in vec3 invertexpos;
in vec3 innormal;
in vec2 intexcoord;

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
