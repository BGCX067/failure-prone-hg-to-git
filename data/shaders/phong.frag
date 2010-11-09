#version 330 core

out vec4 fragColor;

in vec3 normal;
in vec3 position;

uniform mat4 modelview;
uniform vec3 LightPosition;
uniform vec3 eyePosition;
uniform vec4 Ka;
uniform vec4 Kd;
uniform vec4 Ks;
uniform float shininess;
uniform vec4 globalAmbient;
uniform vec4 LightColor;

void main() {
	//vec3 n = normalize(normal);
    vec3 n = normal;
	vec3 lightVec = normalize(normalize((modelview*vec4(LightPosition, 1.0)).xyz) - normalize(position));
    vec3 viewVec = normalize(/*(modelview*vec4(eyePosition, 1.0)).xyz*/  position);
	vec3 halfVec = normalize(lightVec + viewVec);

    float diffCoef = max(dot(n, lightVec), 0.0);
	float specCoef = pow(max(dot(viewVec, halfVec), 0.0), shininess);
	if (diffCoef <= 0.0)
		specCoef = 0.0;
    
    vec4 ambient = Ka*globalAmbient;
	vec4 diffuse = Kd*LightColor*diffCoef;
	vec4 specular = Ks*LightColor*specCoef;
	fragColor = ambient + diffuse + specular;

//	fragColor = vec4(n, 1.0);
//    fragColor = specular;
}

