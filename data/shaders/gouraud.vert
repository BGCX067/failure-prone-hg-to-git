uniform vec3 LightPosition;
uniform vec3 EyePosition;
uniform vec4 Ka;
uniform vec4 Kd;
uniform vec4 Ks;
uniform float shininess;
uniform vec4 globalAmbient;
uniform vec4 LightColor;

varying vec4 color;

void main() {
    gl_Position = ftransform();
    vec3 position = vec3(gl_ModelViewMatrix*gl_Vertex);
    vec3 normal = gl_NormalMatrix*gl_Normal;

    vec3 lightVec = normalize(LightPosition - position);
    vec3 viewVec = normalize(EyePosition - position);
    vec3 halfVec = normalize(lightVec + viewVec);

    float diffCoef = max(dot(normal, lightVec), 0.0);
    float specCoef = pow(max(dot(normal, halfVec), 0.0), shininess);
    
    if (diffCoef <= 0.0)
        specCoef = 0.0;

    vec4 ambient = globalAmbient*Ka;
    vec4 diffuse = Kd*LightColor*diffCoef;
    vec4 specular = Ks*LightColor*specCoef;

    color = ambient + diffuse + specular;
    color.w = 1.0;
}
