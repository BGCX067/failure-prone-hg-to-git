varying vec3 normal;
varying vec4 position;

uniform vec3 LightPosition;
uniform vec3 EyePosition;
uniform vec4 LightColor;
uniform float shininess;
uniform sampler2D texture;

vec3 expand(vec3 v) {
    return (v - 0.5)*2.0;
}

void main() {
    vec4 color = texture2D(texture, gl_TexCoord[0].st);
    vec3 lightvec = normalize(LightPosition - position.xyz);
    float diffuse = max(0.0, dot(normal, lightvec));

    vec3 viewvec = normalize(EyePosition - position.xyz);
    vec3 halfvec = normalize(lightvec + viewvec);
    float spec = pow(max(dot(normal, halfvec), 0.0), shininess);
    vec3 norm = normalize(vec3(color.g - color.r, color.g - color.a, 1));
    vec3 light = expand(lightvec);
    norm = expand(norm);
    diffuse *= dot(norm, lightvec);

    gl_FragColor = diffuse*LightColor + spec*LightColor;
}
