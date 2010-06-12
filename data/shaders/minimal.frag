#version 330 core

uniform sampler2D tex;

in vec2 texCoord;
out vec4 fragColor;

void main() {
    fragColor = vec4(texCoord.x, texCoord.y, 0.0, 1.0);//color;//vec4(1.0, 0.0, 0.0, 1.0);
}
