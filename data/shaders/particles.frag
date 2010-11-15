#version 330 core

uniform sampler2D tex;
out vec4 fragColor;

void main() {
    fragColor = /*vec4(0.5, 1.0, 0.1, 1.0);*/texture(tex, gl_PointCoord);
}
