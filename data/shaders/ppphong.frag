varying vec3 normal;
varying vec4 position;

uniform vec3 LightPosition;
uniform vec3 EyePosition;
uniform vec4 LightColor;
uniform float shininess;

void main() {
    vec3 lightvec = normalize(LightPosition - position.xyz);
    float diffuse = max(0.0, dot(normal, lightvec));

    vec3 viewvec = normalize(EyePosition - position.xyz);
    vec3 halfvec = normalize(lightvec + viewvec);
    float spec = pow(max(dot(normal, halfvec), 0.0), shininess);

    gl_FragColor = diffuse*LightColor + spec*LightColor;
}
