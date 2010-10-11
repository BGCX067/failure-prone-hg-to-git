#version 330 core

in vec2 texCoord;
out vec4 fragColor;
uniform sampler2D tex;

void main() {
    fragColor = vec4(texture(tex, texCoord).rgb, 1.0);
//    fragColor = vec4(texCoord.x, texCoord.y, 0.0, 1.0);
}
