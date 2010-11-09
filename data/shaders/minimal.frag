#version 330 core

in vec3  normal;
in vec2 texCoord;
in vec3 pos;
out vec4 fragColor;
uniform sampler2D tex;


void main() {
<<<<<<< local
    fragColor = vec4(texture(tex, texCoord).rgb, 1.0);
//    fragColor = vec4(normal.x, normal.y, normal.z, 1.0);
//	fragColor = vec4(1.0, 0.0, 0.0, 1.0);
=======
    //fragColor = vec4(normalize(pos), 1.0);
    fragColor = vec4(0.0, 0.0, 1.0, 1.0);
    //fragColor = vec4(texture(tex, texCoord).rgb, 1.0) ;
//    fragColor = vec4(texCoord.x, texCoord.y, 0.0, 1.0);
>>>>>>> other
}
