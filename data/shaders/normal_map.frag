varying vec3 lightDir;
varying vec3 position;

uniform vec3 EyePosition;
uniform vec4 Ka;
uniform vec4 Kd;
uniform vec4 Ks;
uniform float shininess;
uniform vec4 globalAmbient;
uniform vec4 LightColor;

uniform sampler2D normalMap;
uniform sampler2D baseTexture;

void main() {
    vec3 lightVec = normalize(lightDir);
    vec3 N = normalize((texture2D(normalMap, gl_TexCoord[0].xy).xyz*2.0) - 1.0);
    vec3 viewVec = normalize(EyePosition - position);
    vec3 halfVec = normalize(lightDir + viewVec);

    float diffCoef = max(dot(N, lightVec), 0.0);
    float specCoef = pow(max(dot(N, halfVec), 0.0), shininess);
    
    if (diffCoef <= 0.0)
        specCoef = 0.0;

    vec4 ambient = globalAmbient*Ka;
    vec4 diffuse = Kd*LightColor*diffCoef;
    vec4 specular = Ks*LightColor*specCoef;

    vec4 baseColor = texture2D(baseTexture, gl_TexCoord[0].xy);
    gl_FragColor = (ambient + diffuse + specular)*baseColor;
}
