uniform vec3 LightPosition;
uniform vec3 EyePosition;

varying vec3 lightDir;
varying vec4 color;
varying vec2 texCoord;
attribute vec3 Tangent;
attribute vec3 Binormal;

void main() {
    gl_Position = ftransform();
    texCoord = gl_MultiTexCoord0.xy;
    vec4 objPosition = gl_ModelViewMatrix*gl_Vertex;

    vec3 lightDirection = LightPosition - objPosition.xyz;
   
    vec3 normal = gl_NormalMatrix*gl_Normal;
    vec3 tangent = gl_NormalMatrix*Tangent;
    vec3 binormal = cross(normal, tangent);

    lightDir.x = dot(lightDirection, tangent);
    lightDir.y = dot(lightDirection, binormal);
    lightDir.z = dot(lightDirection, normal);
    lightDir = normalize(lightDir);
}
