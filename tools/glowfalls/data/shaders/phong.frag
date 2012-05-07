#version 330

in vec2 texcoord;
in vec3 pos;
out vec4 outcolor;

//uniform sampler2D normalmap;
uniform sampler2D tex;

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
    vec3 lightpos = vec3(300, 200, 0.8);
    float radius = 400;
    float d = distance(pos.xy, lightpos.xy);
    vec3 s = lightpos - pos;
    float factor = max(dot(-s, vec3(0.0, 0.0, -1.0)), 0.0);
    
//    if(pos.z < lightpos.z)
/q        outcolor = vec4(kd*factor*max(lightintensity*smoothstep(radius, 0.0, d), ka), texcolor.a);
//    else
//        outcolor = vec4(kd*ka, texcolor.a);
    outcolor = vec4(ambientlight*kd + 50*lightintensity*(kd*max(dot(s, n), 0.0))*smoothstep(radius, 0.0, d), texcolor.a);
}
