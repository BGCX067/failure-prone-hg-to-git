#version 330 core

in vec2 texCoord;
out vec4 fragColor;
uniform sampler2D tex;
uniform float time;

void main() {

	vec2 resolution = vec2(800.0, 600.0);
	vec2 p = -1.0 + 2.0 * gl_FragCoord.xy / resolution.xy;

	float a = atan(p.y,p.x);
	float r = sqrt(dot(p,p));
	float s = r*(1.0+0.8*cos(time*1.0));

        vec2 uv;

	uv.x = 0.02*p.y+0.3*cos(-time+a*3.0)/s;
	uv.y = .1 +.02*p.x+.03*sin(-time+a*3.0)/s;
	float w = .9 + pow(max(1.5-r,0.0),4.0);
	w*=0.6+0.4*cos(time+3.0*a);

	vec3 col =  texture2D(tex,uv).xyz;
	fragColor = vec4(col*w,1.0);


//	fragColor =  texture2D( tex, texCoord);
}
