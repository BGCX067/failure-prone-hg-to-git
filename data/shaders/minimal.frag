#version 330 core

in vec3  normal;
in vec2 texCoord;
out vec4 fragColor;
uniform sampler2D tex;


void main() {
    fragColor = vec4(texture(tex, texCoord).rgb, 1.0);
//    fragColor = vec4(normal.x, normal.y, normal.z, 1.0);
//	fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
