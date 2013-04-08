#version 330

in vec2 texcoord;

out vec4 outcolor;
uniform sampler2D tex;

void main() {
	vec4 blurred;
	vec2 center = vec2(0.5, 0.5);
	for (int i = 0; i<32; i++){
		vec2 scale = vec2( 1.0 /*1.0 + (0.3)*(i/(33.0))*/, 1.0+ (0.3)*(i/(33.0)));
		blurred += texture(tex, texcoord*scale);
	}
	blurred /= 32.0;
	blurred.r = pow(blurred.r, 1.6);
	blurred.g = pow(blurred.g, 1.6);
	blurred.b = pow(blurred.b, 1.6);
	blurred.rgb *= 6.0;
	blurred.rgb = clamp(blurred.rgb, 0.0, 1.0);
	vec4 originaltex = texture(tex, texcoord);
	vec3 newC = originaltex.rgb + (1.0 - 0.7)*blurred.rgb;
	float newA = max(originaltex.a, blurred.a);
//	outcolor = vec4(newC, newA);
    outcolor = texture(tex, texcoord);
}
