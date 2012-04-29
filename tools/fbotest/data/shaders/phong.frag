#version 330

in vec3 vertexpos;
in vec3 normal;
in vec2 texcoord;

out vec4 outcolor;

uniform sampler2D tex;
uniform vec3 lightpos;
uniform vec3 lightintensity;
uniform vec3 ka;
uniform vec3 ks;
//uniform vec3 kd;
uniform float shininess;
uniform vec3 eyepos;
 
void main() {
    vec3 n = normalize(normal);
    vec3 s = normalize(lightpos - vertexpos);
    vec3 v = normalize(eyepos - vertexpos);
    vec3 r = reflect(-s, n);
    
    vec3 kd = vec3(texture(tex, texcoord));
    vec3 outcolor3 = lightintensity*(ka + kd*max(dot(s, n), 0.0) + ks*pow(max(dot(r, v), 0.0), shininess));

    float gamma = 2.2;
    outcolor = vec4(pow(outcolor3, vec3(1.0/gamma)), 1.0);
}
