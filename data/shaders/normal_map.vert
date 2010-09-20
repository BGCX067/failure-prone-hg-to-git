attribute vec3 Tangent;
attribute vec3 Binormal;

uniform vec3 LightPosition;

varying vec3 lightDir;
varying vec3 position;

void main() {
    gl_Position = ftransform();
    gl_TexCoord[0] = gl_MultiTexCoord0;
    position = gl_Vertex.xyz;

    vec3 lightDirection = LightPosition - position.xyz;
   
    vec3 normal = gl_NormalMatrix*gl_Normal;
    vec3 tangent = gl_NormalMatrix*Tangent;
    vec3 binormal = cross(normal, tangent);

    lightDir.x = dot(lightDirection, tangent);
    lightDir.y = dot(lightDirection, binormal);
    lightDir.z = dot(lightDirection, normal);
    //lightDir = normalize(lightDir);
}
