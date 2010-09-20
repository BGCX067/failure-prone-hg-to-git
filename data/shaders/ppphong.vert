varying vec3 normal;
varying vec4 position;

void main() {
    normal = gl_Normal;
    gl_Position = ftransform();
    position = gl_Position;
}
