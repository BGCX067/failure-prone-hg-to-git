varying vec3 lightDir;
varying vec4 color;
varying vec2 texCoord;

uniform vec4 LightColor;
uniform float shininess;

uniform sampler2D normalMap;
uniform sampler2D baseTexture;
uniform sampler2D baseTex0;
void main() {
    vec4 ambLight = vec4(0.3, 0.3, 0.3, 1.0);
    vec4 difLight = vec4(0.8, 0.8, 0.8, 1.0);
    vec4 specLight = vec4(0.4, 0.8, 0.4, 1.0);

    vec3 normal = normalize((texture2D(normalMap, texCoord).xyz * 2.0) - 1.0);
    vec4 baseColor = texture2D(baseTexture, texCoord);

    gl_FragColor = dot(normal, lightDir)*LightColor*baseColor + ambLight*baseColor;
    //gl_FragColor = mix(vec4(normal,1.0), baseColor, 0.8);
    //gl_FragColor = vec4(normal, 1.0);//vec4(lightDir, 1.0);
}
