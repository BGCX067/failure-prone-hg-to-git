varying vec3 normal;
varying vec3 position;

void main() {
    gl_Position = ftransform();
    position = gl_Vertex.xyz;//vec3(gl_ModelViewMatrix*gl_Vertex);
    normal = gl_NormalMatrix*gl_Normal;   
}
