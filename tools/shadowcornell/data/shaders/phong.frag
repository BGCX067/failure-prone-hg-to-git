#version 330

in vec3 vertexpos;
in vec3 normal;
in vec3 lPos;

out vec4 outcolor;

//uniform vec3 LightPosition;
uniform vec3 LightColor;
uniform vec3 ka;
uniform vec3 ks;
uniform vec3 kd;
uniform float shininess;
//Não precisa do eyepos pq já tá transformando tudo pro eyespace, logo eyepos = (0, 0, 0)
//uniform vec3 eyepos;

void main() {
    vec3 n = normalize(normal);
    vec3 l = normalize(lPos - vertexpos);
    vec3 v = normalize(-vertexpos);
    vec3 h = normalize(l + v);
    
    float diffuseLight = max(dot(n, l), 0.0);
    float specularLight = pow(max(dot(n, h), 0.0), shininess);
    if(diffuseLight == 0.0)
        specularLight = 0.0;
    vec3 globalAmbient = vec3(0.1, 0.1, 0.1);
    vec3 amb = globalAmbient*ka;
    vec3 diff = LightColor*kd*diffuseLight;
    vec3 spec = LightColor*ks*specularLight;
    
    vec3 outcolor3 = amb + diff + spec;

    float gamma = 2.2;
    outcolor = vec4(pow(outcolor3, vec3(1.0/gamma)), 1.0);
    //outcolor = vec4(outcolor3, 1.0);
    //fragdepth = gl_FragCoord.z;
}
