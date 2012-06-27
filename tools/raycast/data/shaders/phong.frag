#version 330

in vec3 vertexpos;
in vec3 normal;
in vec3 texcoord;
in vec3 camerapos;

out vec4 outcolor;

uniform sampler3D tex;
//uniform vec3 LightPosition;
//uniform vec3 LightColor;
//uniform vec3 ka;
//uniform vec3 ks;
//uniform float shininess;
//uniform vec3 eyepos;

const int steps = 50;
const vec3 volmin = vec3(0.0, 0.0, 0.0);
const vec3 volmax = vec3(1.0, 1.0, 1.0);


void main() {
    vec4 dst = vec4(0.0, 0.0, 0.0, 0.0);
    vec3 position = texcoord;
    vec3 cameraposition = (camerapos - volmin)/(volmax - volmin);
    vec3 direction = normalize(position - cameraposition);

    const float stepSize = 0.02;//1.0/steps;

    for(int i = 0; i < steps; i++) {
        vec4 value = texture(tex, position);

        float scalar = value.r;
        //TODO transfer function
        vec4 src = value;//vec4(scalar, scalar, scalar, scalar);
        src.a *= 0.8;
        src.rgb *= src.a;
        
        dst = (1.0 - dst.a)*src + dst;

        position = position + direction*stepSize;
            
        //if(dst.a > 0.9)
        //    break;

        vec3 temp1 = sign(position - volmin);
        vec3 temp2 = sign(volmax - position);

        if(dot(temp1, temp2) < 3)
            break;

        //if(position.x > 1.0 || position.y > 1.0 || position.z > 1.0)
        //    break;
        //if(position.x < 0.0 || position.y < 0.0 || position.z < 0.0)
        //    break;
    }
    
    outcolor = dst;
    //outcolor = vec4(texcoord, 1.0);
    //outcolor = vec4(normalize(abs(direction)), 1.0);
    //outcolor = texture(tex, texcoord);
    //float gamma = 2.2;
    //outcolor = vec4(pow(outcolor.xyz, vec3(1.0/gamma)), 1.0 - outcolor.a);
}
