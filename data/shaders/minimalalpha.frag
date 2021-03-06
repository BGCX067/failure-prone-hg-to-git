#version 330 core

in vec2 texCoord;
in vec3 normal;
out vec4 fragColor;

uniform sampler2D tex;

void main() {
    float color = texture(tex, texCoord).a;
//    vec2 color2 = texture(tex, texCoord).rg;
    fragColor = vec4(color, color, color, color);
   //   fragColor = vec4(normal, 1.0);
}
