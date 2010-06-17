#version 330 core

out vec4 fragColor;
uniform mat4 modelview;
in vec3 normal;
in vec3 position;
uniform vec4 LightPosition;
uniform vec3 EyePosition;
uniform vec4 Ka;
uniform vec4 Kd;
uniform vec4 Ks;
uniform float shininess;
uniform vec4 globalAmbient;
uniform vec4 LightColor;

/*vec4 phong(vec3 n, vec3 lightDir) {
	//vec3 viewVec = normalize(EyePosition - position);
	//vec3 halfVec = normalize(lightDir + viewVec);
	//float diffCoef = max(dot(n, lightDir), 0.0);
	//float specCoef = pow(max(dot(n, halfVec), 0.0), shininess);
	//if (diffCoef <= 0.0)
	//	specCoef = 0.0;
	//vec4 ambient = Ka*globalAmbient;
	//vec4 diffuse = Kd*LightColor;
	//vec4 specular = Ks*LightColor*specCoef;
	vec4 color = vec4(0.0, 1.0, 0.0, 1.0);//ambient + diffuse + specular;
	//color.w = 1.0;
	return color;
}*/
void main() {
	//vec3 N = normalize(normal);
	//vec3 lightPos = (mvp*LightPosition).xyz;
	//vec3 lightVec = normalize(position);

	//vec4 phongColor = phong(N, lightVec.xyz);
	fragColor = vec4(normalize(position), 1.0);//phongColor;//LightPosition;//vec4(N, 1.0);
}

