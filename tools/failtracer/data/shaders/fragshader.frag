#version 330

in vec2 texcoord;

out vec4 outcolor;

uniform float time;

float intersect(in vec3 ro, in vec3 rd){

	float r = 1.0;
	//float a = dot(rd, rd); //nao precisa pq vai ser sempre 1.0 ja que rd sao normalizados
	float b = 2.0*dot(rd, ro);
	float c = dot(ro,ro) - r*r;
	float delta = b*b - 4.0*c;
	if (delta < 0.0)
		return 0.0;
	float x1 = (-b - sqrt(delta)) / 2.0;
	float x2 = (-b + sqrt(delta)) / 2.0;

	return x1;
}

void main() {

	vec2 resolution = vec2(800.0, 600.0);
	vec2 uv = texcoord*vec2(resolution.x/resolution.y, 1.0);

	vec3 ro = vec3(0.0, 0.0, 2.0);
	vec3 rd = normalize(vec3( (-1.0 + 2.0*uv), -1.0  ));

	float result = intersect(ro, rd);

	outcolor = vec4(0.0, 0.0, 0.0, 1.0);

	vec3 lightcolor = vec3(0.0, 0.9, 0.2);
	vec3 spherecolor = vec3(0.8, 0.3, 0.2);

	if ( result > 0.0 ){	

		vec3 point = ro + result*rd;
		vec3 spherepos = vec3(0.0, 1.0, 0.0);
		vec3 normal = point - spherepos;
		vec3 lightpos = vec3(cos(time),  1.0 + cos(time)*2.0, 5.0);

		float diffuse = max(0.0, dot(normal, normalize(lightpos - point)));
		float attenuation = 1.0 - smoothstep(0.0, 100.0, length(lightpos - point));
		diffuse = diffuse * attenuation;

		outcolor = vec4( lightcolor*diffuse + spherecolor , 1.0);
	}
}
