#version 330

in vec2 texcoord;
in vec2 pos;
out vec4 outcolor;

uniform sampler2D normalmap;
uniform sampler2D tex;

void main() {
    vec3 lightintensity = vec3(0.4, 0.4, 0.4);
    float ka = 0.05;
    vec3 ambientlight = vec3(ka, ka, ka);
    vec2 lightpos = vec2(0.5, 0.5);
    float radius = 0.5;
    float d = distance(pos, lightpos);
    vec3 kd = vec3(texture(tex, texcoord));
    vec3 n = vec3(texture(normalmap, texcoord));

    n = n*2.0 - vec3(1.0, 1.0, 1.0);
    vec3 s = vec3(lightpos, 6.0) - vec3(pos, 0.0);
    float factor = max(dot(s, n), 0.0);
    
    outcolor = vec4(kd*factor*max(lightintensity*smoothstep(radius, 0.0, d), ka), 1.0);
}
