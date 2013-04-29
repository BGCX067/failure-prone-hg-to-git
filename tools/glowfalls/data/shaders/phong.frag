#version 330

in vec2 texcoord;
in vec3 pos;
out vec4 outcolor;

//uniform sampler2D normalmap;
uniform sampler2D tex;
uniform mat4 view;

void main() {
    /* Material/Texture info */
    vec4 texcolor = texture(tex, texcoord);
    vec3 kd = vec3(texcolor);
    vec3 n = vec3(0.0, 0.0, 1.0);
    float ka = 0.1;
    vec3 ambientlight = vec3(ka, ka, ka);

    /* SPOT LIGHT */
    /*vec3 lightpos = vec3(300, 200, 2.0);
    vec3 lightintensity = vec3(0.7, 0.7, 0.7);
    vec3 direction = vec3(0.0, 0.0, -1.0);
    float exponent = 1.0;
    float cutoff = radians(80.0);
    
    vec3 s = normalize(lightpos - pos);
    float dotSDir = dot(-s, direction);
    float angle = acos(dotSDir);

    //TODO considerar specular - view vec
    
    if(angle < cutoff) {
        float spotFactor = pow(dotSDir, exponent);
        outcolor = vec4(ambientlight + spotFactor*lightintensity*(kd*max(dot(s, n), 0.0)), texcolor.a);
    } else {
        outcolor = vec4(ambientlight*kd, texcolor.a);
    }*/


    /* FAIL LIGHT */
    vec3 lightintensity = vec3(0.7, 0.7, 0.7);
    vec3 lightpos = vec3(view*vec4(450, 200, 0.0, 1.0));
    float radius = 500;
    float d = distance(pos, lightpos);
    
    //Direção para a luz a partir do ponto sendo iluminado
    vec3 lightDir = vec3(0.0, 0.0, 1.0);
    //Fator diffuso só terá algum impacto se for usado normalmap
    float diffuse = max(dot(n, lightDir), 0.0);
    
    outcolor = vec4(ka*kd + kd*diffuse*lightintensity*(1.0 - smoothstep(0.0, radius, d)),texcolor.a);
}
