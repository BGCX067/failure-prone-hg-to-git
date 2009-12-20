
uniform vec3 LightPosition;
uniform vec3 EyePosition;
uniform vec4 LightColor;

varying vec4 color;

void  main(){

	float diffuse;

	vec3 position = vec3(gl_ModelViewMatrix *gl_Vertex);
	vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
	vec3 lightvec = normalize(LightPosition - position);
	
	diffuse = max(0.0, dot(normal, lightvec));

	vec3 viewvec = normalize(EyePosition - position );
	vec3 halfvec = normalize(lightvec + viewvec);
	float spec  = pow(max( dot(normal, halfvec) ,0.0) ,16.0);

	if (diffuse <= 0.0) spec = 0.0;

	color = diffuse*LightColor + spec*LightColor;
	//color = LightColor;

	gl_Position = ftransform();

}
