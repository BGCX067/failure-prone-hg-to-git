#version 330  core

//uniform mat4 mvp;
uniform mat4 modelview;
uniform mat4 projection;

layout(location = 0) in vec3 Pos;
layout(location = 2) in vec3 Normal;
layout(location = 8) in vec2 inTexCoord;

out vec2 texCoord;
<<<<<<< local
out vec3 normal;
=======
out vec3 pos;
>>>>>>> other

void main() {
    mat4 mvp = projection*modelview;
    texCoord = inTexCoord.xy;
    normal = Normal.xyz;
    gl_Position = mvp*vec4(Pos, 1.0);

    pos = (modelview*vec4(Pos, 1.0)).xyz;
}
