varying vec3 normal;
varying vec3 position;

uniform vec3 LightPosition;
uniform vec3 EyePosition;
uniform vec4 Ka;
uniform vec4 Kd;
uniform vec4 Ks;
uniform float shininess;
uniform vec4 globalAmbient;
uniform vec4 LightColor;

uniform sampler2D texture;

void main() {
    vec3 N = normalize(normal);
    
    vec3 lightVec = normalize(LightPosition - position);
    vec3 viewVec = normalize(EyePosition - position);
    vec3 halfVec = normalize(lightVec + viewVec);

    float diffCoef = max(dot(N, lightVec), 0.0);
    float specCoef = pow(max(dot(N, halfVec), 0.0), shininess);
    
    if (diffCoef <= 0.0)
        specCoef = 0.0;

    vec4 ambient = globalAmbient*Ka;
    vec4 diffuse = Kd*LightColor*diffCoef;
    vec4 specular = Ks*LightColor*specCoef;

    gl_FragColor = (ambient + diffuse + specular)*texture2D(texture, gl_TexCoord[0].st);
    gl_FragColor.w = 1.0;
}
