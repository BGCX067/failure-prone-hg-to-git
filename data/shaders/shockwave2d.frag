uniform sampler2D sceneTex;
uniform vec2 center;
uniform float Time;
uniform vec3 shockParams;

void main(){
	vec2 uv = gl_TexCoord[0].xy;
	vec2 texCoord = uv;
	float distance = distance(uv, center);
	if ( (distance <= (Time + shockParams.z)) && (distance >= (Time - shockParams.z)) ){
		float diff = (distance - Time);
		float powDiff = 1.0 - pow(abs(diff*shockParams.x), shockParams.y);
		float diffTime = diff  * powDiff;
		vec2 diffUV = normalize(uv - center);
		texCoord = uv + (diffUV * diffTime);
	}
	gl_FragColor = texture2D(sceneTex, texCoord);
}
